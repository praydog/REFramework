#include <cstdint>
#include <concepts>
#include <algorithm>
#include <bit>
#include <memory>
#include <unordered_map>
#include <vector>

#include <hde64.h>

#include "HookManager.hpp"
#include "sdk/REContext.hpp"
#include "sdk/REManagedObject.hpp"
#include "sdk/REDelegate.hpp"
#include "sdk/RETypeDB.hpp"
#include "sdk/SceneManager.hpp"
#include "sdk/ResourceManager.hpp"
#include "sdk/MotionFsm2Layer.hpp"
#include "sdk/TDBVer.hpp"
#include "sdk/GameIdentity.hpp"
#include "utility/Memory.hpp"

#include "../ScriptRunner.hpp"

#include <lstate.h> // weird include order because of sol

#include "Sdk.hpp"

namespace api {
namespace sdk {
static std::unordered_map<::sdk::RETypeDefinition*, uint32_t> s_fnv_cache{};

struct BehaviorTreeCoreHandle : public ::REManagedObject {
    int unused;
};

struct BehaviorTree : public ::REManagedObject {
    int unused;
    int unused2;
};
}
}

namespace detail {
constexpr uintptr_t FAKE_OBJECT_ADDR = 12345;

// Registry keys: only their addresses matter. Each Lua state's registry holds its own tables under them (set up in
// open_sdk), so the push path and the reference bookkeeping reach them with lua_rawgetp instead of looking up globals
// by name or going through sol objects (each of which takes and drops a registry reference).
char s_objects_key{};             // _sol_lua_push_objects: weak cache of wrappers, keyed by object address
char s_ref_counts_key{};          // _sol_lua_push_ref_counts: engine references Lua holds, keyed by object address
char s_lifetime_key{};            // WrapperLifetime userdata: the addresses Lua holds a reference for (see below)

// The bookkeeping tables hold Lua integers keyed by object address; a missing entry reads as nullopt.
std::optional<lua_Integer> get_count(lua_State* l, const void* key, lua_Integer addr) {
    std::optional<lua_Integer> result{};

    lua_rawgetp(l, LUA_REGISTRYINDEX, key);

    if (lua_rawgeti(l, -1, addr) == LUA_TNUMBER) {
        result = lua_tointeger(l, -1);
    }

    lua_pop(l, 2);
    return result;
}

bool has_entry(lua_State* l, const void* key, lua_Integer addr) {
    lua_rawgetp(l, LUA_REGISTRYINDEX, key);
    const bool present = lua_rawgeti(l, -1, addr) != LUA_TNIL;
    lua_pop(l, 2);
    return present;
}

void set_count(lua_State* l, const void* key, lua_Integer addr, std::optional<lua_Integer> value) {
    lua_rawgetp(l, LUA_REGISTRYINDEX, key);

    if (value) {
        lua_pushinteger(l, *value);
    } else {
        lua_pushnil(l);
    }

    lua_rawseti(l, -2, addr);
    lua_pop(l, 1);
}

// Lifetime of managed object wrappers (experiment E-lite). Wrappers have no __gc: the collector frees a dead wrapper
// like any other userdata (no finalizer call, no resurrection, no second cycle) and clears its weak cache entry. Engine
// references are kept per address instead of per wrapper: WrapperLifetime knows every address Lua holds a reference
// for, and scan_wrapper_references (once per frame, after the frame's GC step) visits them round-robin. A visit that
// finds no cached wrapper marks the address; a later visit at least RELEASE_AFTER_FRAMES frames on that still finds
// none releases its reference. A push that misses the cache while the address is still tracked reuses the reference
// instead of adding one.
//
// Invariants:
// - An address is tracked exactly while ref_counts[addr] > 0: track_reference and untrack_reference run where the
//   ref_counts entry is created or removed (add_ref, release, the scan, close). Pushes reuse a tracked reference, so
//   ref_counts[addr] stays 1.
// - While a wrapper that relies on an address's reference is alive, the cache holds a wrapper for that address: every
//   push caches the wrapper it makes, add_ref() caches the wrapper it's called on if release() had dropped the entry,
//   and release() untracks the address when it drops the entry. One exception: Lua clears weak values before it
//   resurrects objects for finalization (lgc.c, atomic), so a wrapper reachable only from objects waiting for their
//   finalizers loses its entry while those finalizers can still use it. The scan releases nothing while any finalizer
//   is pending (finalizers_pending): the generational collector runs them at the end of the collection, the
//   incremental one only after the whole sweep, possibly many frames later, and an emergency collection leaves them
//   for the next one. A finalizer that stores the wrapper away keeps a wrapper without a reference, as it did when the
//   wrapper's own __gc released the reference.
// - So a visit that finds no cached wrapper while no finalizer is pending could release at once. The second visit is
//   for objects a script pushes again and again whose wrappers keep dying (minor collections): a push between the
//   visits caches a wrapper again, takes the reference over, and the next visit clears the mark, instead of paying an
//   engine release and add_ref each time.
// - Passes. A pass starts with the scan after the last one ended and covers the N slots tracked then: a slot tracked
//   during the pass counts as visited (it goes in at the cursor, see track_reference), and untracking moves no slot
//   not visited yet behind the cursor (see free_slot). Each scan of the pass visits
//   max(MIN_VISITS, ceil(N / PASS_FRAMES)) slots, plus INFLOW_VISITS * I where I is the smaller of the numbers of
//   addresses tracked anew in the last two frames, so a pass ends within PASS_FRAMES scans whatever is tracked or
//   released meanwhile.
// - Latency. Whatever collection freed a wrapper (minor or major, generational or incremental), the first visit of
//   its slot at or after the next scan s marks it: within the rest of the current pass, or within the next pass if the
//   slot was visited just before. Its first later visit at least RELEASE_AFTER_FRAMES frames after the mark that finds
//   no finalizer pending releases it, normally in the pass after. A release() at a pass boundary moves the last slot
//   into the hole, and the slot the last scan of a pass marked can land where the next pass visits it one frame later,
//   too early; its next visit is then up to two passes away. So the reference is released within 4 * PASS_FRAMES scans
//   counting s, and each visit that finds a finalizer pending postpones it by up to 2 * PASS_FRAMES scans. While all
//   slots fit in one scan's visits, the release comes RELEASE_AFTER_FRAMES scans after s. A script whose wrappers die
//   at I per frame keeps at most 4 * PASS_FRAMES * I addresses tracked beyond the ones it holds.
// - Inflow. Without the INFLOW_VISITS term, more than about MIN_VISITS / 2 short-lived addresses tracked a frame would
//   make passes, the backlog and the latency jump: a slot is marked in the pass after the one it was tracked in and
//   released in the one after that, so a pass of L scans leaves about 3 * I * L slots for the next one, and passes
//   grow from 2 scans to PASS_FRAMES once 3 * I exceeds the visits of a scan. With the term, a scan visits more than
//   that: with nothing held, passes take at most 2 scans, so about 8 * I addresses stay tracked and each is released
//   within about 6 scans. Held addresses H lengthen passes to about H / (INFLOW_VISITS * I) scans. A burst in a
//   single frame adds no visits: its addresses are released within the bound above, at the pass's share per scan.
// - Cost. A scan does max(MIN_VISITS, ceil(N / PASS_FRAMES)) + INFLOW_VISITS * I cache lookups plus its releases,
//   with no time budget: MIN_VISITS while fewer than MIN_VISITS * PASS_FRAMES addresses are tracked and none come in,
//   growing with N, held or not. One lookup walks the hash chain of the address's integer key: Lua 5.4.3 hashes
//   integers by their low bits (ltable.c, hashint), which a 16-byte aligned address leaves zero, so chains are about
//   16 times longer than with spread keys; master's pushes pay the same.
//   MIN_VISITS 1024 against 256 costs a few microseconds more a scan and cuts the release latency of up to 64k tracked
//   addresses fourfold; PASS_FRAMES 32 would halve the latency and the backlog but double the scans after a burst of
//   500k and the cost of holding many wrappers.
// - The cache and the reference counts are the script-visible globals _sol_lua_push_objects and
//   _sol_lua_push_ref_counts (the registry keys point at the same tables). A script that clears a live wrapper's cache
//   entry makes the scan release its reference two visits later; one that sets a count n makes the scan or close
//   release n references. Master's __gc read the same tables. Nothing in this repository writes them; private registry
//   tables with read-only views under the old names would close this (a decision for the maintainer).
constexpr uint32_t RELEASE_AFTER_FRAMES = 2;
constexpr size_t MIN_VISITS = 1024;
constexpr size_t PASS_FRAMES = 64;
constexpr size_t INFLOW_VISITS = 4;
constexpr size_t TRIM_MIN = 4096; // containers smaller than this keep their capacity

// True while the collector has objects whose finalizers it queued but hasn't run. Reads global_State::tobefnz of the
// Lua 5.4.3 built into REFramework (dependencies/lua): atomic() moves unreachable finalizable objects there
// (separatetobefnz) and GCTM takes them off as it calls their finalizers. No public API tells: lua_gc reports memory,
// whether the collector runs and whether a step finished a cycle, none of which says finalizers are still pending.
// Read only.
bool finalizers_pending(lua_State* l) {
    return G(l)->tobefnz != nullptr;
}

// Maps an object address (never 0) to its slot. Open addressing with linear probing and backward-shift deletion (no
// tombstones), so tracking an address allocates nothing unless the table grows.
class AddressTable {
public:
    uint32_t* find(lua_Integer key) {
        if (m_size == 0) {
            return nullptr;
        }

        for (size_t i = home(key);; i = (i + 1) & m_mask) {
            if (m_keys[i] == key) {
                return &m_values[i];
            }

            if (m_keys[i] == 0) {
                return nullptr;
            }
        }
    }

    // Returns the value slot for key and whether it was inserted (with value).
    std::pair<uint32_t*, bool> try_emplace(lua_Integer key, uint32_t value) {
        if ((m_size + 1) * 4 > m_keys.size() * 3) {
            rehash(m_keys.empty() ? 16 : m_keys.size() * 2);
        }

        for (size_t i = home(key);; i = (i + 1) & m_mask) {
            if (m_keys[i] == key) {
                return {&m_values[i], false};
            }

            if (m_keys[i] == 0) {
                m_keys[i] = key;
                m_values[i] = value;
                ++m_size;
                return {&m_values[i], true};
            }
        }
    }

    void erase(lua_Integer key) {
        if (m_size == 0) {
            return;
        }

        auto hole = home(key);

        while (m_keys[hole] != key) {
            if (m_keys[hole] == 0) {
                return;
            }

            hole = (hole + 1) & m_mask;
        }

        // Moves later entries of the probe run into the hole unless their home lies after it.
        for (auto j = (hole + 1) & m_mask; m_keys[j] != 0; j = (j + 1) & m_mask) {
            if (((j - home(m_keys[j])) & m_mask) >= ((j - hole) & m_mask)) {
                m_keys[hole] = m_keys[j];
                m_values[hole] = m_values[j];
                hole = j;
            }
        }

        m_keys[hole] = 0;
        --m_size;
    }

    size_t size() const { return m_size; }
    size_t capacity() const { return m_keys.size(); }

    // Empties the table, sized for `expected` entries.
    void reset(size_t expected) {
        m_keys = {};
        m_values = {};
        m_size = 0;

        if (expected > 0) {
            auto capacity = (size_t)16;

            while (capacity * 3 < expected * 4 + 4) {
                capacity *= 2;
            }

            rehash(capacity);
        }
    }

private:
    size_t home(lua_Integer key) const { return (size_t)(((uint64_t)key * 0x9E3779B97F4A7C15ull) >> m_shift); }

    void rehash(size_t capacity) {
        std::vector<lua_Integer> keys(capacity, 0);
        std::vector<uint32_t> values(capacity);
        std::swap(keys, m_keys);
        std::swap(values, m_values);
        m_mask = capacity - 1;
        m_shift = 64 - std::countr_zero(capacity);

        for (size_t k = 0; k < keys.size(); ++k) {
            if (keys[k] != 0) {
                auto i = home(keys[k]);

                while (m_keys[i] != 0) {
                    i = (i + 1) & m_mask;
                }

                m_keys[i] = keys[k];
                m_values[i] = values[k];
            }
        }
    }

    std::vector<lua_Integer> m_keys{};
    std::vector<uint32_t> m_values{};
    size_t m_mask{0};
    int m_shift{64};
    size_t m_size{0};
};

struct WrapperLifetime {
    struct Slot {
        lua_Integer addr{0};
        uint32_t dead_since{0}; // frame a visit first found the address without a wrapper; 0: it has one
    };

    std::vector<Slot> slots{};                                    // dense: untracking moves the last slot into the hole
    AddressTable slot_of{};                                       // address -> index in `slots`
    std::vector<std::pair<lua_Integer, lua_Integer>> releasing{}; // scan scratch: address, reference count
    size_t cursor{0};                                             // [0, cursor) visited this pass
    size_t pass_visits{MIN_VISITS};                               // visits per scan of the pass under way
    size_t tracked_since_scan{0};                                 // new slots since the last scan
    size_t tracked_last_scan{0};                                  // new slots between the two scans before
    uint32_t frame{1};

    void start_pass() {
        cursor = 0;
        pass_visits = std::max(MIN_VISITS, (slots.size() + PASS_FRAMES - 1) / PASS_FRAMES);
    }

    Slot* find(lua_Integer addr) {
        const auto index = slot_of.find(addr);
        return index != nullptr ? &slots[*index] : nullptr;
    }
};

// The registry userdata holds a pointer: code that runs after the close handler (other finalizers in lua_close) finds
// nullptr instead of a destroyed object, and tracks nothing.
struct WrapperLifetimeBox {
    WrapperLifetime* lifetime;
};

WrapperLifetime* wrapper_lifetime(lua_State* l) {
    lua_rawgetp(l, LUA_REGISTRYINDEX, &s_lifetime_key);
    const auto box = (WrapperLifetimeBox*)lua_touserdata(l, -1);
    lua_pop(l, 1);

    return box != nullptr ? box->lifetime : nullptr;
}

// Untracks the slot at index. The last slot moves into the hole; if that would move a slot the round-robin hasn't
// visited yet behind the cursor, it goes to cursor - 1 instead (whose visited slot fills the hole) and the cursor steps
// back, so a pass still visits every slot that stays tracked through it exactly once.
void free_slot(WrapperLifetime& lt, size_t index) {
    lt.slot_of.erase(lt.slots[index].addr);
    const auto last = lt.slots.size() - 1;

    const auto move = [&](size_t from, size_t to) {
        lt.slots[to] = lt.slots[from];
        *lt.slot_of.find(lt.slots[to].addr) = (uint32_t)to;
    };

    if (index != last) {
        if (index < lt.cursor && last >= lt.cursor) {
            const auto behind = lt.cursor - 1;

            if (behind != index) {
                move(behind, index);
            }

            move(last, behind);
            lt.cursor = behind;
        } else {
            move(last, index);
        }
    }

    lt.slots.pop_back();
    lt.cursor = std::min(lt.cursor, lt.slots.size());
}

// ref_counts[addr] was created (Lua took an engine reference for addr). A new slot counts as visited by the pass
// under way: it goes in at the cursor, the slot there (not visited yet) moves to the end, and the cursor steps past
// it. Appended at the end, new slots would keep a pass from ending while addresses are tracked faster than the
// visits go, and the slots it already visited would never be checked again.
void track_reference(lua_State* l, lua_Integer addr) {
    const auto lifetime = wrapper_lifetime(l);

    if (lifetime == nullptr) {
        return;
    }

    auto& lt = *lifetime;
    const auto [index, inserted] = lt.slot_of.try_emplace(addr, (uint32_t)lt.cursor);

    if (inserted) {
        if (lt.cursor < lt.slots.size()) {
            const auto unvisited = lt.slots[lt.cursor];
            lt.slots.push_back(unvisited);
            *lt.slot_of.find(unvisited.addr) = (uint32_t)(lt.slots.size() - 1);
            lt.slots[lt.cursor] = {addr};
        } else {
            lt.slots.push_back({addr});
        }

        ++lt.cursor;
        ++lt.tracked_since_scan;
    }

    lt.slots[*index].dead_since = 0;
}

// ref_counts[addr] was removed.
void untrack_reference(lua_State* l, lua_Integer addr) {
    const auto lifetime = wrapper_lifetime(l);

    if (lifetime == nullptr) {
        return;
    }

    if (const auto index = lifetime->slot_of.find(addr); index != nullptr) {
        free_slot(*lifetime, *index);
    }
}

// A push missed the cache. Returns true if Lua still holds addr's reference: the new wrapper takes it over.
bool reuse_reference(lua_State* l, lua_Integer addr) {
    const auto lifetime = wrapper_lifetime(l);

    if (lifetime == nullptr) {
        return false;
    }

    const auto slot = lifetime->find(addr);

    if (slot == nullptr) {
        return false;
    }

    slot->dead_since = 0;
    return true;
}

// Gives back the capacity of a vector that holds less than an eighth of it after a burst. `in_use` is the fill to plan
// for; twice that is kept, so a steady load doesn't shrink and regrow every frame. The copy is at most an eighth of the
// capacity the burst allocated.
template <typename T>
void trim(std::vector<T>& v, size_t in_use) {
    if (v.capacity() > TRIM_MIN && v.capacity() > 8 * in_use) {
        std::vector<T> trimmed{};
        trimmed.reserve(2 * in_use);
        trimmed.assign(v.begin(), v.end());
        v.swap(trimmed);
    }
}

// Once per frame, after the frame's GC step: the next round-robin visits (see the invariants above). The visits run no
// Lua code; the engine releases come last, after the bookkeeping is final, as they can re-enter Lua.
void scan_wrapper_references(lua_State* l) {
    const auto lifetime = wrapper_lifetime(l);

    if (lifetime == nullptr) {
        return;
    }

    auto& lt = *lifetime;
    ++lt.frame;

    if (lua_rawgetp(l, LUA_REGISTRYINDEX, &s_objects_key) != LUA_TTABLE) {
        lua_pop(l, 1);
        return;
    }

    const auto cache = lua_gettop(l);
    const bool may_release = !finalizers_pending(l);
    auto releasing = std::move(lt.releasing);

    // Addresses tracked anew in each of the last two frames: the smaller of the two is an inflow that has lasted (a
    // burst in a single frame adds nothing). Each scan visits INFLOW_VISITS times that many slots more than the pass's
    // share: otherwise, with more than about MIN_VISITS / 2 short-lived addresses tracked a frame, passes would grow
    // from 2 scans to PASS_FRAMES and the backlog with them (see the invariants above).
    const auto inflow = std::min(lt.tracked_since_scan, lt.tracked_last_scan);
    lt.tracked_last_scan = lt.tracked_since_scan;
    lt.tracked_since_scan = 0;

    if (lt.cursor >= lt.slots.size()) {
        lt.start_pass();
    }

    // A pass that ends here leaves the rest of this scan's visits unused: the next pass starts with the next scan.
    for (auto visits = lt.pass_visits + INFLOW_VISITS * inflow; visits > 0 && lt.cursor < lt.slots.size(); --visits) {
        auto& slot = lt.slots[lt.cursor];
        const bool cached = lua_rawgeti(l, cache, slot.addr) != LUA_TNIL;
        lua_pop(l, 1);

        if (cached) {
            slot.dead_since = 0;
        } else if (slot.dead_since == 0) {
            slot.dead_since = lt.frame;
        } else if (may_release && lt.frame - slot.dead_since >= RELEASE_AFTER_FRAMES) {
            releasing.emplace_back(slot.addr, get_count(l, &s_ref_counts_key, slot.addr).value_or(0));
            set_count(l, &s_ref_counts_key, slot.addr, std::nullopt);
            free_slot(lt, lt.cursor); // the last slot, not visited yet, takes its place under the cursor
            continue;
        }

        ++lt.cursor;
    }

    lua_pop(l, 1);

    // After a burst: rebuild the address table at its live size once it holds less than an eighth of its capacity, so
    // the copy is at most an eighth of what the burst allocated. Giving a burst-sized buffer back to the system costs
    // about 0.6 ms per 12 MB (measured), so this and the slots trim below don't run in the same scan.
    const bool rebuild = lt.slot_of.capacity() > TRIM_MIN && lt.slot_of.capacity() > 8 * lt.slots.size();

    if (rebuild) {
        lt.slot_of.reset(lt.slots.size());

        for (size_t i = 0; i < lt.slots.size(); ++i) {
            lt.slot_of.try_emplace(lt.slots[i].addr, (uint32_t)i);
        }
    }

    for (const auto& [addr, count] : releasing) {
        for (lua_Integer i = 0; i < count; ++i) {
            ((::REManagedObject*)(uintptr_t)addr)->release();
        }
    }

    trim(releasing, releasing.size());
    releasing.clear();
    lt.releasing = std::move(releasing);

    if (!rebuild) {
        trim(lt.slots, lt.slots.size());
    }
}

// __gc of the WrapperLifetime userdata: runs when the Lua state closes. Releases every reference Lua still holds,
// once, and stops tracking.
int wrapper_lifetime_gc(lua_State* l) {
    // A script can reach the metamethod (debug.getregistry) and call it with anything: act only on the registry's box.
    lua_rawgetp(l, LUA_REGISTRYINDEX, &s_lifetime_key);
    const bool is_box = lua_type(l, 1) == LUA_TUSERDATA && lua_rawequal(l, 1, -1);
    lua_pop(l, 1);

    const auto box = is_box ? (WrapperLifetimeBox*)lua_touserdata(l, 1) : nullptr;

    if (box == nullptr || box->lifetime == nullptr) {
        return 0;
    }

    const std::unique_ptr<WrapperLifetime> lifetime{box->lifetime};
    box->lifetime = nullptr;

    std::vector<std::pair<lua_Integer, lua_Integer>> releasing{};
    releasing.reserve(lifetime->slot_of.size());

    for (const auto& slot : lifetime->slots) {
        if (slot.addr != 0) {
            releasing.emplace_back(slot.addr, get_count(l, &s_ref_counts_key, slot.addr).value_or(0));
            set_count(l, &s_ref_counts_key, slot.addr, std::nullopt);
        }
    }

    for (const auto& [addr, count] : releasing) {
        for (lua_Integer i = 0; i < count; ++i) {
            ((::REManagedObject*)(uintptr_t)addr)->release();
        }
    }

    return 0;
}
}

namespace api::cached_usertype {
namespace detail {
    
}
}

namespace api::re_managed_object {
namespace detail {
void add_ref(lua_State* l, ::REManagedObject* obj, bool force = false) {
    // we shouldn't really do this very much
    // so it shouldn't be too terrible on performance
    if (!REManagedObject::is_managed_object(obj)) {
        throw sol::error{(std::stringstream{} << "sol_lua_push: " << (uintptr_t)obj << " is not a managed object").str()};
    }

    const auto addr = (lua_Integer)(uintptr_t)obj;

    // only add a reference if it's a "local" object, indicated by a negative reference count
    //if ((int32_t)obj->referenceCount < 0 || (current_ref_count && *current_ref_count > 0)) {
    // addendum: only do it when reference count is > 0, local objects seem buggy...
    // The engine count, not get_visible_ref_count(): an object whose last release REFramework has queued is still a
    // counted global object, so a push takes a reference on it like on any other.
    if (force || (int32_t)obj->get_ref_count() > 0) {
        if (!force) {
            obj->add_ref();
        }

        const auto current_ref_count = ::detail::get_count(l, &::detail::s_ref_counts_key, addr);

        if (current_ref_count && *current_ref_count > 0) {
            // don't unnecessarily increase the ref count
            // if the user is the one doing it
            if (force) {
                return;
            }

            ::detail::set_count(l, &::detail::s_ref_counts_key, addr, *current_ref_count + 1);
        } else {
            // only add the ref once when the user requests it
            // so they don't screw something up
            if (force) {
                obj->add_ref();
            }

            ::detail::set_count(l, &::detail::s_ref_counts_key, addr, 1);
            ::detail::track_reference(l, addr);
        }
    }
}
} // namespace detail

// used by metatable for REManagedObject
::REManagedObject* add_ref(sol::this_state s, ::REManagedObject* obj) {
    const auto l = s.lua_state();
    const auto addr = (lua_Integer)(uintptr_t)obj;

    detail::add_ref(l, obj, true);

    // release() drops the cache entry. If the script kept that wrapper and calls add_ref() on it, it has to become the
    // cached wrapper again: the scan keeps the reference only while the address has a cached wrapper.
    // Misuse this can't tell apart: after release() the engine may free the object and put a new one at the address.
    // add_ref() on the old wrapper then takes a reference on the new object and makes the old wrapper its cached one;
    // later pushes of the address return it, with the old object's usertype, until it is gone. The wrapper doesn't
    // record the static type it was pushed as, so comparing usertypes would also flag legitimate wrappers.
    if (lua_type(l, 1) == LUA_TUSERDATA && !::detail::has_entry(l, &::detail::s_objects_key, addr)) {
        lua_rawgetp(l, LUA_REGISTRYINDEX, &::detail::s_objects_key);
        lua_pushvalue(l, 1);
        lua_rawseti(l, -2, addr);
        lua_pop(l, 1);
        ::detail::reuse_reference(l, addr);
    }

    return obj;
}

::REManagedObject* add_ref_permanent(sol::this_state s, ::REManagedObject* obj) {
    if (!REManagedObject::is_managed_object(obj)) {
        throw sol::error{(std::stringstream{} << "add_ref_permanent: " << (uintptr_t)obj << " is not a managed object").str()};
    } 

    obj->add_ref();

    return obj;
}

void release(lua_State* l, ::REManagedObject* obj, bool force = false) {
    const auto addr = (lua_Integer)(uintptr_t)obj;

    if (const auto ref_count = ::detail::get_count(l, &::detail::s_ref_counts_key, addr); ref_count && *ref_count > 0) {
        // because of our internal refcount keeping, we shouldn't need to double check
        // whether it's an actual object or not. hopefully?
        obj->release();

        const auto new_ref_count = *ref_count - 1;

        if (new_ref_count == 0) {
            ::detail::set_count(l, &::detail::s_objects_key, addr, std::nullopt);
            ::detail::set_count(l, &::detail::s_ref_counts_key, addr, std::nullopt);
            ::detail::untrack_reference(l, addr);
        } else {
            ::detail::set_count(l, &::detail::s_ref_counts_key, addr, new_ref_count);
        }
    } else if (::detail::has_entry(l, &::detail::s_objects_key, addr)) {
        // A cached wrapper without a reference: the wrapper of a local object. Nothing to give back (unless forced),
        // but the wrapper is dropped from the cache, so the next push of the address makes a new one.
        if (force && REManagedObject::is_managed_object(obj)) {
            obj->release();
        }

        ::detail::set_count(l, &::detail::s_objects_key, addr, std::nullopt);
    } else {
        if (force) {
            if (REManagedObject::is_managed_object(obj)) {
                obj->release();
            }
        } else {
            spdlog::warn("REManagedObject:release attempted to release an object that was not managed by our Lua state");
        }
    }
}

void release(sol::this_state s, ::REManagedObject* obj, bool force = false) {
    release(s.lua_state(), obj, force);
}
} // namespace api::re_managed_object

namespace detail {
// Pushes the cached wrapper for addr from the cache table under key. Returns false (and pushes nothing) if there is none.
bool push_cached_wrapper(lua_State* l, const void* key, lua_Integer addr) {
    if (lua_rawgetp(l, LUA_REGISTRYINDEX, key) != LUA_TTABLE) {
        lua_pop(l, 1);
        return false;
    }

    if (lua_rawgeti(l, -1, addr) == LUA_TNIL) {
        lua_pop(l, 2);
        return false;
    }

    lua_remove(l, -2);
    return true;
}

// Stores the wrapper at index in the cache table under key.
void cache_wrapper(lua_State* l, const void* key, int index, lua_Integer addr) {
    index = lua_absindex(l, index);

    if (lua_rawgetp(l, LUA_REGISTRYINDEX, key) != LUA_TTABLE) {
        lua_pop(l, 1);
        return;
    }

    lua_pushvalue(l, index);
    lua_rawseti(l, -2, addr);
    lua_pop(l, 1);
}

// Caches the new wrapper a push left at the top of the stack (count values), or, if the cache already holds one for
// addr, replaces it with that one and returns 1. Allocating the new wrapper can run a collection (only while Lua's own
// collector runs) whose finalizers push the same object: that push found no cached wrapper either, took the reference
// over and cached its own wrapper. Two wrappers would then share one reference while only one is cached, and the scan
// would release it once the cached one is gone.
int cache_new_wrapper(lua_State* l, const void* key, int count, lua_Integer addr) {
    if (push_cached_wrapper(l, key, addr)) {
        lua_replace(l, -1 - count);
        lua_pop(l, count - 1);
        return 1;
    }

    cache_wrapper(l, key, -count, addr);
    return count;
}

// Creates a weak-valued cache table under key if the registry doesn't have one yet.
void ensure_weak_cache(lua_State* l, const void* key) {
    if (lua_rawgetp(l, LUA_REGISTRYINDEX, key) == LUA_TTABLE) {
        lua_pop(l, 1);
        return;
    }

    lua_pop(l, 1);
    lua_newtable(l);
    lua_newtable(l);
    lua_pushliteral(l, "v");
    lua_setfield(l, -2, "__mode");
    lua_setmetatable(l, -2);
    lua_rawsetp(l, LUA_REGISTRYINDEX, key);
}

}

// specialization for REManagedObject to automatically add a reference
// when lua pushes a pointer to the object onto the stack
template<detail::ManagedObjectBased T>
int sol_lua_push(sol::types<T*>, lua_State* l, T* obj) {
    if (obj != nullptr) {
        const auto addr = (lua_Integer)(uintptr_t)obj;

        // Push the cached wrapper as it is. It used to be "touched" here by clearing its GC color bits directly, which
        // also stopped Lua's write barrier from seeing it: stored into an old table (held[#held + 1] = arr:add_ref()),
        // the wrapper was swept while still referenced and the engine object released under the script. Wrappers have
        // no __gc now; a wrapper the collector freed leaves its address tracked for a few frames (see WrapperLifetime),
        // and a new wrapper takes that reference over instead of adding another.
        if (detail::push_cached_wrapper(l, &detail::s_objects_key, addr)) {
            return 1;
        } else {
            if ((uintptr_t)obj != detail::FAKE_OBJECT_ADDR && !detail::reuse_reference(l, addr)) {
                api::re_managed_object::detail::add_ref(l, (::REManagedObject*)obj, false);
            }

            int32_t backpedal = 0;

            if ((uintptr_t)obj != detail::FAKE_OBJECT_ADDR) {
                uint32_t typename_hash = 0;
                const auto td = obj->get_type_definition();

                if (td != nullptr) {
                    if (auto it = api::sdk::s_fnv_cache.find(td); it != api::sdk::s_fnv_cache.end()) {
                        typename_hash = it->second;
                    } else {
                        typename_hash = utility::hash(td->get_full_name());
                        api::sdk::s_fnv_cache[td] = typename_hash;
                    }

                    switch (typename_hash) {
                    case "via.Transform"_fnv:
                        backpedal = sol::stack::push<sol::detail::as_pointer_tag<std::remove_pointer_t<::RETransform>>>(l, (::RETransform*)obj);
                        break;
                    case "via.behaviortree.BehaviorTree"_fnv:[[fallthrough]];
                    case "via.motion.MotionFsm2"_fnv:[[fallthrough]];
                    case "via.motion.MotionJackFsm2"_fnv:[[fallthrough]];
                    case "snow.PlayerMotionFsm"_fnv:
                        backpedal = sol::stack::push<sol::detail::as_pointer_tag<std::remove_pointer_t<api::sdk::BehaviorTree>>>(l, (api::sdk::BehaviorTree*)obj);
                        break;
                    case "via.behaviortree.BehaviorTree.CoreHandle"_fnv: [[fallthrough]];
                    case "via.motion.MotionFsm2Layer"_fnv: [[fallthrough]];
                    case "via.timeline.TimelineFsm2Layer"_fnv:
                        backpedal = sol::stack::push<sol::detail::as_pointer_tag<std::remove_pointer_t<api::sdk::BehaviorTreeCoreHandle>>>(l, (api::sdk::BehaviorTreeCoreHandle*)obj);
                        break;
                    default:
                        {
                            const auto vm_obj_type = td->get_vm_obj_type();
                            if (vm_obj_type == via::clr::VMObjType::Array) {
                                backpedal = sol::stack::push<sol::detail::as_pointer_tag<std::remove_pointer_t<sdk::SystemArray>>>(l, (sdk::SystemArray*)obj);
                            } else {
                                backpedal = sol::stack::push<sol::detail::as_pointer_tag<std::remove_pointer_t<T>>>(l, obj);
                            }
                        }
                        
                        break;
                    };
                } else {
                    backpedal = sol::stack::push<sol::detail::as_pointer_tag<std::remove_pointer_t<T>>>(l, obj);
                }

                // keep a weak reference to the object for caching
                return detail::cache_new_wrapper(l, &detail::s_objects_key, backpedal, addr);
            } else {
                backpedal = sol::stack::push<sol::detail::as_pointer_tag<std::remove_pointer_t<T>>>(l, obj);
            }

            return backpedal;
        }
    }

    return sol::stack::push(l, sol::nil);
}

// Other translation units (ScriptRunner.cpp, RE8VR.cpp) push these types too but only see the declaration in
// ScriptRunner.hpp, so they link against the instantiations emitted here. Instantiate them explicitly so the symbols
// don't depend on which of these types this file happens to push itself.
template int sol_lua_push<::REManagedObject>(sol::types<::REManagedObject*>, lua_State*, ::REManagedObject*);
template int sol_lua_push<::REComponent>(sol::types<::REComponent*>, lua_State*, ::REComponent*);
template int sol_lua_push<::RETransform>(sol::types<::RETransform*>, lua_State*, ::RETransform*);
template int sol_lua_push<::sdk::SystemArray>(sol::types<::sdk::SystemArray*>, lua_State*, ::sdk::SystemArray*);
template int sol_lua_push<::sdk::Delegate>(sol::types<::sdk::Delegate*>, lua_State*, ::sdk::Delegate*);
template int sol_lua_push<::api::sdk::BehaviorTreeCoreHandle>(sol::types<::api::sdk::BehaviorTreeCoreHandle*>, lua_State*, ::api::sdk::BehaviorTreeCoreHandle*);
template int sol_lua_push<::api::sdk::BehaviorTree>(sol::types<::api::sdk::BehaviorTree*>, lua_State*, ::api::sdk::BehaviorTree*);

// specialization for any custom usertype we exposed to sol to automatically add a reference
// when lua pushes a pointer to the object onto the stack
template<detail::CachedUserType T>
int sol_lua_push(sol::types<T*>, lua_State* l, T* obj) {
    if (obj != nullptr) {
        // One weak cache table per usertype, kept in the registry under this instantiation's key and created on first use.
        static char s_cache_key{};
        const auto addr = (lua_Integer)(uintptr_t)obj;

        // Push the cached wrapper as it is; see the managed object push above for why it isn't "touched". These
        // wrappers hold no engine reference, so nothing tracks them: they live as long as Lua reaches them.
        if (detail::push_cached_wrapper(l, &s_cache_key, addr)) {
            return 1;
        }

        const int32_t backpedal = sol::stack::push<sol::detail::as_pointer_tag<std::remove_pointer_t<T>>>(l, obj);

        if ((uintptr_t)obj != detail::FAKE_OBJECT_ADDR) {
            detail::ensure_weak_cache(l, &s_cache_key);
            detail::cache_wrapper(l, &s_cache_key, -backpedal, addr);
        }

        return backpedal;
    }

    return sol::stack::push(l, sol::nil);
}

namespace api::delegate {
void add_lua_delegate(sol::this_state s, ::sdk::Delegate* delegate, uint32_t index, sol::protected_function callback) {
    if (index >= delegate->num_methods) {
        throw sol::error("Delegate index out of bounds");
    }

    auto& invo = delegate->methods[index];
    auto sol_state = sol::state_view{s};
    auto state = sol_state.registry()["state"].get<ScriptState*>();

    state->add_delegate_callback(invo, callback);
}

// newindex
void new_index(sol::this_state s, ::sdk::Delegate* delegate, sol::object lua_obj, sol::variadic_args args) {
    if (lua_obj.is<uint32_t>()) {
        auto index = lua_obj.as<uint32_t>();

        if (args[0].is<sol::protected_function>()) {
            auto callback = args[0].get<sol::protected_function>();

            add_lua_delegate(s, delegate, index, callback);
        } else {
            throw sol::error("Delegate callback must be a function");
        }
    }
}
}

namespace api::sdk {
std::vector<void*>& build_args(sol::variadic_args va, ::sdk::REMethodDefinition* fn = nullptr);
sol::object parse_data(lua_State* l, void* data, ::sdk::RETypeDefinition* data_type, bool from_method);
sol::object get_native_field(sol::object obj, ::sdk::RETypeDefinition* ty, const char* name);
sol::object get_native_field_from_field(sol::object obj, ::sdk::RETypeDefinition* ty, ::sdk::REField* field);
sol::object get_field_or_method(sol::object obj, const char* name);
void set_native_field(sol::this_state s, sol::object obj, ::sdk::RETypeDefinition* ty, const char* name, sol::object value);

struct ValueType {
    std::vector<uint8_t> data{};
    ::sdk::RETypeDefinition* type{nullptr};

    ValueType(::sdk::RETypeDefinition* t)
        : type(t)
    {
        if (type != nullptr) {
            data.resize(type->get_size());
        }
    }

    ValueType(::sdk::RETypeDefinition* t, void* addr): type(t) {
        if (t != nullptr && addr != nullptr) {
            uint8_t* raw_data = reinterpret_cast<uint8_t*>(addr);
            data.reserve(t->get_size());
            data.insert(data.begin(), raw_data, raw_data + t->get_size());
        }
    }

    ValueType(const void* raw_data, size_t raw_data_size) {
        if (raw_data_size > 0 && raw_data != nullptr) {
            data.resize(raw_data_size);
            memcpy(data.data(), raw_data, raw_data_size);
        }
    }

    template <typename T>
    bool is_valid_offset(int32_t offset, T& value) const {
        return offset >= 0 && offset + sizeof(T) <= (int32_t)data.size();
    }

    template <typename T>
    void write_memory(int32_t offset, T value) {
        if (!is_valid_offset(offset, value)) {
            return;
        }

        *(T*)((uintptr_t)data.data() + offset) = value;
    }

    template <typename T>
    T read_memory(int32_t offset) {
        T value{};

        // Same bounds as write_memory: the whole T must fit, not just its first byte.
        if (!is_valid_offset(offset, value)) {
            return value;
        }

        return *(T*)((uintptr_t)data.data() + offset);
    }

    uintptr_t address() const {
        return (uintptr_t)data.data();
    }

    sol::object call(sol::this_state l, const char* name, sol::variadic_args va) {
        if (type == nullptr) {
            return sol::make_object(l, sol::nil);
        }

        auto def = type->get_method(name);

        if (def == nullptr) {
            return sol::make_object(l, sol::nil);
        }

        reframework::InvokeRet ret_val{};
        auto vec_args = ::api::sdk::build_args(va, def);

        const auto mo_runtime_size = REManagedObject::runtime_size();

        // `this` depends on who declares the method:
        // - The value type itself, since TDB 74: invoke() treats `this` of a value-type instance method as a boxed
        //   object and skips its 0x10 byte header, so we pass a fake boxed object:
        //     0x00: REObjectInfo* (type info / vtable pointer)
        //     0x08: uint32_t refcount + padding
        //     0x10: actual value type data starts here
        // - The value type itself, before TDB 74: invoke() passes `this` through unchanged and the method read the fake
        //   header as the value (seen with Guid.ToString and via.vec3 get_Item/set_Item in TDB 67-73: dmc5, re8, ggr,
        //   re2, re3, re7, re4, sf6, mhrise, kunitsu, gs456, drdr), so there the value's own data is `this`.
        // - A reference type (System.Object.GetType, System.Enum.ToString): the method uses `this` as a real object,
        //   and the fake header isn't one (both threw "Invoke threw an exception" in RE2 and DD2), so the value goes
        //   into a real boxed object of its type for the call.
        const auto declaring_type = def->get_declaring_type();
        const bool declared_by_reference_type = declaring_type != nullptr && !declaring_type->is_value_type();
        const auto type_vt_size = type->get_valuetype_size();

        if (def->is_static()) {
            ret_val = def->invoke((void*)address(), std::span(vec_args));
        } else if (declared_by_reference_type) {
            auto box = type->create_instance_full();

            if (box == nullptr) {
                throw sol::error("ValueType:call: could not box the value for " + std::string{name});
            }

            // The new object is local: the engine keeps it until it releases the thread's local objects, after this
            // call. No add_ref()/release() pair here: with one, 2000 of these calls per frame raised SEH exceptions
            // in RE9 within a frame and then crashed the game.

            memcpy((uint8_t*)box + mo_runtime_size, data.data(), type_vt_size);
            ret_val = def->invoke(box, std::span(vec_args));
            memcpy(data.data(), (uint8_t*)box + mo_runtime_size, type_vt_size);
        } else if (::sdk::GameIdentity::get().tdb_ver() >= 74) {
            // Reduce allocation pressure with thread_local.
            thread_local std::vector<uint8_t> fake_boxed_storage(mo_runtime_size + type_vt_size, 0);

            if (fake_boxed_storage.size() < mo_runtime_size + type_vt_size) {
                fake_boxed_storage.resize(mo_runtime_size + type_vt_size);
            }

            // REObject header: REObjectInfo* at offset 0x00
            *(void**)&fake_boxed_storage[0x00] = (void*)type;
            // REManagedObject: reference count at offset 0x08 (set high to prevent GC interference)
            *(uint32_t*)&fake_boxed_storage[0x08] = 9999;
            // Copy value type data at offset 0x10
            memcpy(&fake_boxed_storage[mo_runtime_size], data.data(), type_vt_size);

            auto real_obj = (void*)fake_boxed_storage.data();

            ret_val = def->invoke(real_obj, std::span(vec_args));

            // copy back any changes to the value type from the fake boxed storage
            memcpy(data.data(), &fake_boxed_storage[mo_runtime_size], type_vt_size);
        } else {
            ret_val = def->invoke((void*)address(), std::span(vec_args));
        }

        if (ret_val.exception_thrown) {
            throw sol::error("Invoke threw an exception");
        }

        // Convert return values to the correct Lua types.
        auto ret_ty = def->get_return_type();

        return ::api::sdk::parse_data(l, &ret_val, ret_ty, true);
    }

    sol::object get_field(sol::this_state l, const char* name) {
        if (type == nullptr) {
            return sol::make_object(l, sol::nil);
        }

        return ::api::sdk::get_native_field(sol::make_object(l, (void*)address()), type, name);
    }

    void set_field(sol::this_state l, const char* name, sol::object value) {
        if (type == nullptr) {
            return;
        }

        ::api::sdk::set_native_field(l, sol::make_object(l, (void*)address()), type, name, value);
    }

    sol::object index(sol::this_state s, const char* name) {
        if (type == nullptr) {
            return sol::make_object(s, sol::nil);
        }


        auto field = type->get_field(name);
        if (field != nullptr) {
            return api::sdk::get_native_field_from_field(sol::make_object(s, (void*)address()), type, field);
        }
        auto method = type->get_method(name);
        if (method != nullptr) {
            return sol::make_object(s, method);
        }

        return sol::make_object(s, sol::nil);
    }

    void new_index(sol::this_state s, const char* name, sol::object assign) {
        if (type == nullptr) {
            return;
        }
        return api::sdk::set_native_field(s, sol::make_object(s, (void*)address()), type, name, assign);
    };
};

struct MemoryView {
    uint8_t* data{nullptr};
    size_t size{0};

    MemoryView(uint8_t* d, size_t s)
        : data(d)
        , size(s)
    {
    }

    template <typename T>
    bool is_valid_offset(int32_t offset, T& value) const {
        return offset >= 0 && offset + sizeof(T) <= (int32_t)size;
    }

    template <typename T>
    void write_memory(int32_t offset, T value) {
        if (!is_valid_offset(offset, value)) {
            return;
        }

        *(T*)((uintptr_t)data + offset) = value;
    }

    template <typename T>
    T read_memory(int32_t offset) {
        T value{};

        if (!is_valid_offset(offset, value)) {
            return value;
        }

        return *(T*)((uintptr_t)data + offset);
    }

    uintptr_t address() const {
        return (uintptr_t)data;
    }

    void wipe() {
        memset(data, 0, size);
    }
};

void* get_thread_context() {
    return (void*)::sdk::get_thread_context();
}

auto find_type_definition(const char* name) {
    return ::sdk::find_type_definition(name);
}

sol::object typeof(sol::this_state s, const char* name) {
    auto type_definition = find_type_definition(name);

    if (type_definition == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, type_definition->get_runtime_type());
}

void* get_native_singleton(const char* name) {
    return (void*)::sdk::get_native_singleton<void>(name);
}

auto get_managed_singleton(sol::this_state s, const char* name) {
    if (name == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    auto out = ::sdk::get_managed_singleton<::REManagedObject>(name);

    if (out == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, out);
}

sol::object create_managed_string(sol::this_state s, const char* text) {
    if (text == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    auto new_str = ::sdk::VM::create_managed_string(utility::widen(text));

    if (new_str == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_str);
}

// Length is taken as int64_t, not uint32_t: a negative Lua number would wrap to
// ~4e9 and the VM would attempt a multi-GB allocation that hangs the process.
sol::object create_managed_array(sol::this_state s, sol::object t_obj, int64_t length) {
    constexpr int64_t max_length = 0x1000000; // 16.7M elements

    if (length < 0) {
        throw sol::error("create_managed_array: length cannot be negative");
    }

    if (length > max_length) {
        throw sol::error("create_managed_array: length " + std::to_string(length)
            + " exceeds the maximum of " + std::to_string(max_length));
    }

    ::REManagedObject* t{nullptr};

    if (t_obj.is<::REManagedObject*>()) {
        t = t_obj.as<::REManagedObject*>();
    } else if (t_obj.is<::sdk::RETypeDefinition*>()) {
        t = t_obj.as<::sdk::RETypeDefinition*>()->get_runtime_type();
    } else if (t_obj.is<const char*>()) {
        const auto tdef = ::sdk::find_type_definition(t_obj.as<const char*>());

        if (tdef != nullptr) {
            t = tdef->get_runtime_type();
        }
    } else {
        throw sol::error("Invalid type passed to create_managed_array. Must be a runtime type, a type definition, or a type name.");
    }

    if (t == nullptr) {
        throw sol::error("Type passed to create_managed_array resolved to null.");
    }

    auto out = ::sdk::VM::create_managed_array(t, (uint32_t)length);

    if (out == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::sdk::SystemArray*)out);
}

sol::object create_delegate(sol::this_state s, sol::object t_obj, uint32_t num_methods) {
    ::REManagedObject* t{nullptr};

    if (t_obj.is<::REManagedObject*>()) {
        t = t_obj.as<::REManagedObject*>();
    } else if (t_obj.is<::sdk::RETypeDefinition*>()) {
        t = t_obj.as<::sdk::RETypeDefinition*>()->get_runtime_type();
    } else if (t_obj.is<const char*>()) {
        const auto tdef = ::sdk::find_type_definition(t_obj.as<const char*>());

        if (tdef != nullptr) {
            t = tdef->get_runtime_type();
        }
    } else {
        throw sol::error("Invalid type passed to create_delegate. Must be a runtime type, a type definition, or a type name.");
    }

    if (t == nullptr) {
        throw sol::error("Type passed to create_delegate resolved to null.");
    }

    auto out = ::sdk::VM::create_delegate(t, num_methods);

    if (out == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, out);
}

sol::object create_sbyte(sol::this_state s, int8_t value) {
    auto new_obj = ::sdk::VM::create_sbyte(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_byte(sol::this_state s, uint8_t value) {
    auto new_obj = ::sdk::VM::create_byte(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_int16(sol::this_state s, int16_t value)  {
    auto new_obj = ::sdk::VM::create_int16(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_uint16(sol::this_state s, uint16_t value)  {
    auto new_obj = ::sdk::VM::create_uint16(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_int32(sol::this_state s, int32_t value)  {
    auto new_obj = ::sdk::VM::create_int32(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_uint32(sol::this_state s, int32_t value)  {
    auto new_obj = ::sdk::VM::create_uint32(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_int64(sol::this_state s, int64_t value)  {
    auto new_obj = ::sdk::VM::create_int64(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_uint64(sol::this_state s, int64_t value)  {
    auto new_obj = ::sdk::VM::create_uint64(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_single(sol::this_state s, float value)  {
    auto new_obj = ::sdk::VM::create_single(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_double(sol::this_state s, double value)  {
    auto new_obj = ::sdk::VM::create_double(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_resource(sol::this_state s, std::string type_name, std::string name) {
    auto& types = reframework::get_types();

    // NOT a type definition!!
    // this is a via::typeinfo::TypeInfo
    const auto t = types->get(type_name);

    if (t == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    auto resource_manager = ::sdk::ResourceManager::get();
    return sol::make_object(s, resource_manager->create_resource(t, utility::widen(name)));
}

sol::object create_global_object(sol::this_state &s, ::REManagedObject *obj) {
    // The engine count: an object with a queued last release is global, not local.
    bool is_currently_local = static_cast<std::int32_t>(obj->get_ref_count()) <= 0;

    // Mark object as global so that lua knows
    if (is_currently_local) {
        obj->add_ref();
    }

    auto obj_safe = sol::make_object(s, obj);

    if (is_currently_local) {
        obj->release();
    }

    return obj_safe;
}

sol::object create_userdata_impl(sol::this_state s, std::string type_name, std::string name, bool global) {
    auto& types = reframework::get_types();

    // NOT a type definition!!
    // this is a via::typeinfo::TypeInfo
    const auto t = types->get(type_name);

    if (t == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    auto resource_manager = ::sdk::ResourceManager::get();
    auto obj = resource_manager->create_userdata(t, utility::widen(name));

    if (!obj.has_value()) {
        return sol::make_object(s, sol::nil);
    }

    auto re_obj = (::REManagedObject*)obj.get();

    if (global) {
        return create_global_object(s, re_obj);
    } else {
        return sol::make_object(s, re_obj);
    }
}

sol::object create_userdata(sol::this_state s, std::string type_name, std::string name) {
    return create_userdata_impl(s, type_name, name, false);
}

sol::object create_userdata_global(sol::this_state s, std::string type_name, std::string name) {
    return create_userdata_impl(s, type_name, name, true);
}

sol::object create_instance_impl(sol::this_state s, const char* name, sol::object simplify_obj, bool global) {
    bool simplify = false;

    if (simplify_obj.is<bool>()) {
        simplify = simplify_obj.as<bool>();
    }

    if (name == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    auto type_definition = find_type_definition(name);

    if (type_definition == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    auto instance_full = type_definition->create_instance_full(simplify);

    if (global) {
        return create_global_object(s, instance_full);
    } else {
        return sol::make_object(s, instance_full);
    }
}

sol::object create_instance(sol::this_state s, const char* name, sol::object simplify_obj) {
    return create_instance_impl(s, name, simplify_obj, false);
}

sol::object create_instance_global(sol::this_state s, const char* name, sol::object simplify_obj) {
    return create_instance_impl(s, name, simplify_obj, true);
}

sol::object create_holder_impl(sol::this_state s, ::sdk::Resource* res, const char* tn, bool global) {
    if (tn == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    const auto t = sdk::find_type_definition(tn);

    if (t == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    auto holder = res->create_holder(t);

    if (holder == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    if (global) {
        return create_global_object(s, holder);
    } else {
        return sol::make_object(s, holder);
    }
}

sol::object create_holder(sol::this_state s, ::sdk::Resource* res, const char* tn) {
    return create_holder_impl(s, res, tn, false);
}

sol::object create_holder_global(sol::this_state s, ::sdk::Resource* res, const char* tn) {
    return create_holder_impl(s, res, tn, true);
}

bool copy_to_clipboard(sol::this_state s, const char* text) {
    if (text == nullptr) {
        return false;
    }

    if (OpenClipboard(nullptr)) {
        EmptyClipboard();

        size_t len = strlen(text) + 1;
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);

        if (hMem) {
            memcpy(GlobalLock(hMem), text, len);
            GlobalUnlock(hMem);

            SetClipboardData(CF_TEXT, hMem);
        } else {
            return false;
        }

        CloseClipboard();
    } else {
        return false;
    }

    return true;
}

void* get_real_obj(sol::object obj) {
    void* real_obj = nullptr;

    if (!obj.is<sol::nil_t>()) {
        if (obj.is<REManagedObject*>()) {
            real_obj = (void*)obj.as<REManagedObject*>();
        } else if (obj.is<::sdk::Resource*>()) {
            real_obj = (void*)obj.as<::sdk::Resource*>();
        } else if (obj.is<ValueType>()) {
            real_obj = (void*)obj.as<ValueType&>().address();
        } else if (obj.is<void*>()) {
            real_obj = obj.as<void*>();
        } else {
            real_obj = (void*)obj.as<uintptr_t>();
        }
    }

    return real_obj;
}

// `this` to pass to fn->invoke() for obj. Before TDB 74, invoke() passes `this` of a value-type instance method
// through unchanged (see ValueType::call), so a boxed value must be passed as its data, past the object header;
// since TDB 74 invoke() skips the header itself.
void* get_invoke_this(sol::object obj, ::sdk::REMethodDefinition* fn) {
    auto real_obj = get_real_obj(obj);

    // Runs for every method called from Lua, and only a value-type instance method before TDB 74 needs anything but
    // real_obj: check the cheap conditions first and leave the sol type check (a metatable lookup) for last.
    if (real_obj == nullptr || ::sdk::GameIdentity::get().tdb_ver() >= 74 || fn->is_static()) {
        return real_obj;
    }

    const auto declaring_type = fn->get_declaring_type();

    if (declaring_type == nullptr || !declaring_type->is_value_type() || !obj.is<REManagedObject*>()) {
        return real_obj;
    }

    return (uint8_t*)real_obj + REManagedObject::runtime_size();
}

sol::object parse_data(lua_State* l, void* data, ::sdk::RETypeDefinition* data_type, bool from_method) {
    if (data_type != nullptr) {
        if (!data_type->is_value_type()) {
            if (data == nullptr || *(void**)data == nullptr) {
                return sol::make_object(l, sol::nil);
            }
        }

        size_t full_name_hash{};

        // Slightly different logic for enums
        if (data_type->is_enum()) {
            auto underlying_type = data_type->get_underlying_type();

            if (underlying_type != nullptr) {
                full_name_hash = utility::hash(underlying_type->get_full_name());
            }
        } else {
            full_name_hash = utility::hash(data_type->get_full_name());
        }

        const auto vm_obj_type = data_type->get_vm_obj_type();

        switch (full_name_hash) {
        case "System.String"_fnv: {
            const auto managed_ret_val = *(::REManagedObject**)data;
            const auto managed_str = (SystemString*)((uintptr_t)managed_ret_val->get_field_ptr() - REManagedObject::runtime_size());
            const auto str = utility::narrow(managed_str->data);

            return sol::make_object(l, str);
        }
        case "System.Single"_fnv: {
            if (from_method) {
                // even though it's a single, it's actually a double because of the invoke wrapper conversion
                auto ret_val_f = *(double*)data;
                return sol::make_object(l, ret_val_f);
            } else {
                auto ret_val_f = *(float*)data;
                return sol::make_object(l, ret_val_f);
            }
        }
        case "System.Double"_fnv: {
            auto ret_val_d = *(double*)data;
            return sol::make_object(l, ret_val_d);
        }
        case "System.Boolean"_fnv: {
            auto ret_val_b = *(bool*)data;
            return sol::make_object(l, ret_val_b);
        }
        case "System.SByte"_fnv: {
            auto ret_val_i = *(int8_t*)data;
            return sol::make_object(l, ret_val_i);
        }
        case "System.Byte"_fnv: {
            auto ret_val_b = *(uint8_t*)data;
            return sol::make_object(l, ret_val_b);
        }
        case "System.Int16"_fnv: {
            auto ret_val_i = *(int16_t*)data;
            return sol::make_object(l, ret_val_i);
        }
        case "System.UInt16"_fnv: {
            auto ret_val_i = *(uint16_t*)data;
            return sol::make_object(l, ret_val_i);
        }
        case "System.UInt32"_fnv: {
            auto ret_val_u = *(uint32_t*)data;
            return sol::make_object(l, ret_val_u);
        }
        case "System.Int32"_fnv: {
            auto ret_val_u = *(int32_t*)data;
            return sol::make_object(l, ret_val_u);
        }
        case "System.Int64"_fnv: {
            auto ret_val_u = *(int64_t*)data;
            return sol::make_object(l, ret_val_u);
        }
        case "System.UInt64"_fnv: {
            //auto ret_val_u = *(uint64_t*)data;
            // so, sol is converting the unsigned version incorrectly into some 1.blah e+19 number
            // so just return it as signed since Lua only has signed integers
            auto ret_val_u = *(int64_t*)data;
            return sol::make_object(l, ret_val_u);
        }
        case "via.Float2"_fnv: [[fallthrough]];
        case "via.vec2"_fnv: {
            auto ret_val_v = *(Vector2f*)data;
            return sol::make_object<Vector2f>(l, ret_val_v);
        }
        case "via.Float3"_fnv: [[fallthrough]];
        case "via.vec3"_fnv: {
            auto ret_val_v = *(Vector3f*)data;
            return sol::make_object<Vector3f>(l, ret_val_v);
        }
        case "via.Float4"_fnv: [[fallthrough]];
        case "via.vec4"_fnv: {
            auto ret_val_v = *(Vector4f*)data;
            return sol::make_object<Vector4f>(l, ret_val_v);
        }
        case "via.mat4"_fnv: {
            auto ret_val_m = *(Matrix4x4f*)data;
            return sol::make_object<Matrix4x4f>(l, ret_val_m);
        }
        case "via.Quaternion"_fnv: {
            auto ret_val_q = *(glm::quat*)data;
            return sol::make_object<glm::quat>(l, ret_val_q);
        }
        case "via.GameObjectRef"_fnv: {
            static auto object_ref_type = ::sdk::find_type_definition("via.GameObjectRef");
            static auto get_target_func = object_ref_type->get_method("get_Target");
            auto obj = get_target_func->call_safe<::REManagedObject*>(sdk::get_thread_context(), data);

            if (obj == nullptr) {
                return sol::make_object(l, sol::nil);
            }

            return sol::make_object(l, obj);
        }
        default:
            if (vm_obj_type > via::clr::VMObjType::NULL_ && vm_obj_type < via::clr::VMObjType::ValType) {
                switch (vm_obj_type) {
                case via::clr::VMObjType::Array:
                    return sol::make_object(l, *(::sdk::SystemArray**)data);
                default: {
                    const auto td = (*(::REManagedObject**)data)->get_type_definition();

                    // another fallback incase the method returns an object which is an array
                    if (td != nullptr && td->get_vm_obj_type() == via::clr::VMObjType::Array) {
                        return sol::make_object(l, *(::sdk::SystemArray**)data);
                    }

                    return sol::make_object(l, *(::REManagedObject**)data);
                }
                }
            }
            break;
        }

        // so, we managed to get here, but we don't know what to do with the data
        // check if it's a valuetype first
        if (data_type->is_value_type()) {
            auto new_obj = sol::make_object(l, ValueType{ data_type });
            auto& bytes = new_obj.as<ValueType&>();
            
            memcpy(bytes.data.data(), data, data_type->get_size());

            return new_obj;
        }
    }

    // A null void* will get converted into an userdata with value 0. That's not very useful in Lua, so
    // let's return nil instead since that's a much more usable value.
    if (data == nullptr || *(void**)data == nullptr) {
        return sol::make_object(l, sol::nil);
    }

    return sol::make_object(l, *(void**)data);
}

void set_data(void* data, ::sdk::RETypeDefinition* data_type, sol::object& value) {
    if (data_type != nullptr) {
        size_t full_name_hash{};
        const auto vm_obj_type = data_type->get_vm_obj_type();

        if (data_type->is_enum()) {
            auto underlying_type = data_type->get_underlying_type();

            if (underlying_type != nullptr) {
                full_name_hash = utility::hash(underlying_type->get_full_name());
            }
        } else {
            full_name_hash = utility::hash(data_type->get_full_name());
        }

        // Commemorating the moment I finally added support for this
        // ....... 2 years after the Lua API was first created
        // This allows us to pass arbitrary ValueTypes and set fields with them
        // So we don't need to hardcode every possible ValueType, just the most important ones like Int32, Single, vec3, etc...
        if (data != nullptr && vm_obj_type == via::clr::VMObjType::ValType && value.is<ValueType*>()) {
            auto vt = value.as<ValueType*>();

            if (vt->type != data_type) {
                if (vt->type != nullptr && data_type != nullptr) {
                    throw sol::error(std::format("Attempted to set ValueType of type {} to field of type {}", vt->type->get_full_name(), data_type->get_full_name()));
                }

                throw sol::error("Attempted to set ValueType to field of unknown type");
            }

            // vt->data holds the boxed size (get_size: payload plus the object header), but an inline field holds
            // only the payload. Copying the whole buffer wrote get_size() - get_valuetype_size() bytes past the field.
            memcpy(data, vt->data.data(), std::min<size_t>(vt->data.size(), data_type->get_valuetype_size()));
            return;
        }

        switch (full_name_hash) {
        case "System.Single"_fnv:
            *(float*)data = value.as<float>();
            return;
        case "System.Double"_fnv:
            *(double*)data = value.as<double>();
            return;
        case "System.Boolean"_fnv:
            *(bool*)data = value.as<bool>();
            return;
        case "System.SByte"_fnv:
            *(int8_t*)data = value.as<int8_t>();
            return;
        case "System.Byte"_fnv:
            *(uint8_t*)data = value.as<uint8_t>();
            return;
        case "System.Int16"_fnv:
            *(int16_t*)data = value.as<int16_t>();
            return;
        case "System.UInt16"_fnv:
            *(uint16_t*)data = value.as<uint16_t>();
            return;
        case "System.Char"_fnv:
            *(uint16_t*)data = value.as<uint16_t>();
            return;
        case "System.UInt32"_fnv:
            *(uint32_t*)data = value.as<uint32_t>();
            return;
        case "System.Int32"_fnv:
            *(int32_t*)data = value.as<int32_t>();
            return;
        case "System.Int64"_fnv:
            *(int64_t*)data = value.as<int64_t>();
            return;
        case "System.UInt64"_fnv:
            *(int64_t*)data = value.as<int64_t>();
            return;
        case "via.Float2"_fnv: [[fallthrough]];
        case "via.vec2"_fnv:
            *(Vector2f*)data = value.as<Vector2f>();
            return;
        case "via.Float3"_fnv: [[fallthrough]];
        case "via.vec3"_fnv:
            *(Vector3f*)data = value.as<Vector3f>();
            return;
        case "via.Float4"_fnv: [[fallthrough]];
        case "via.vec4"_fnv:
            *(Vector4f*)data = value.as<Vector4f>();
            return;
        case "via.mat4"_fnv:
            *(Matrix4x4f*)data = value.as<Matrix4x4f>();
            return;
        case "via.Quaternion"_fnv:
            *(glm::quat*)data = value.as<glm::quat>();
            return;
        default:
            if (vm_obj_type > via::clr::VMObjType::NULL_ && vm_obj_type < via::clr::VMObjType::ValType) {
                REManagedObject* new_data;
                if (value.is<const char*>()) {
                    new_data = ::sdk::VM::create_managed_string(utility::widen(value.as<const char*>()));
                } else {
                    new_data = value.as<::REManagedObject*>();
                }

                REManagedObject** field = (REManagedObject**) data;
                if (field != nullptr && *field != new_data) {
                    if (new_data != nullptr) {
                        new_data->add_ref();
                    }

                    // Use a thread-safe atomic exchange. This is what Capcom does for all field assignments.
                    auto old = *field;
                    while (_InterlockedCompareExchangePointer((PVOID*)field, new_data, old) != old) {
                        old = *field;
                    }

                    if (old != nullptr) {
                        old->release();
                    }
                }

                return;
            }
        }
    }

    // A null void* will get converted into an userdata with value 0. That's not very useful in Lua, so
    // let's return nil instead since that's a much more usable value.
    if (data == nullptr || *(void**)data == nullptr) {
        return;
    }

    *(void**)data = value.as<void*>();
}

void set_native_field_from_field(sol::object obj, ::sdk::RETypeDefinition* ty, ::sdk::REField* field, sol::object value) {

    auto real_obj = get_real_obj(obj);
    bool managed_obj_passed = obj.is<REManagedObject*>();

    const auto field_type = field->get_type();

    if (!field->is_static() && !field_type->is_value_type() && real_obj == nullptr) {
        return;
    }

    auto data = field->get_data_raw(real_obj, ty->is_value_type() && !managed_obj_passed);

    if (data == nullptr) {
        return;
    }

    set_data(data, field_type, value);
}

void set_native_field(sol::this_state s, sol::object obj, ::sdk::RETypeDefinition* ty, const char* name, sol::object value) {
    const auto field = ty->get_field(name);
    if (field == nullptr) {
        auto l = s.lua_state();
        //throw sol::error("Attempted to set invalid REManagedObject field:" + std::string(name));
        luaL_traceback(l, l, ("Attempted to set invalid REManagedObject field:" + std::string(name)).c_str(), 1);
        std::string traceback_err = lua_tostring(l, -1);
        lua_pop(l, 1);
        ScriptRunner::get()->spew_error(traceback_err); // allows us to log the error without failing script execution altogether.
        return;
    }
    return set_native_field_from_field(obj, ty, field, value);
}

sol::object get_native_field_from_field(sol::object obj, ::sdk::RETypeDefinition* ty, ::sdk::REField* field) {
    auto l = obj.lua_state();

    auto real_obj = get_real_obj(obj);
    bool managed_obj_passed = obj.is<REManagedObject*>();

    if (field == nullptr || (!field->is_static() && real_obj == nullptr)) {
        return sol::make_object(l, sol::nil);
    }

    const auto field_type = field->get_type();
    auto data = field->get_data_raw(real_obj, ty->is_value_type() && !managed_obj_passed);

    if (data == nullptr) {
        return sol::make_object(l, sol::nil);
    }

    return parse_data(l, data, field_type, false);
}

sol::object get_native_field(sol::object obj, ::sdk::RETypeDefinition* ty, const char* name) {
    auto l = obj.lua_state();
    const auto field = ty->get_field(name);

    if (field == nullptr) {
        return sol::make_object(l, sol::nil);
    }

    return get_native_field_from_field(obj, ty, field);
}

namespace {
enum class NumericParam { OTHER, FLOATING, INTEGRAL };

NumericParam classify_numeric_param(::sdk::RETypeDefinition* t) {
    if (t != nullptr && t->is_enum()) {
        t = t->get_underlying_type();
    }

    if (t == nullptr) {
        return NumericParam::OTHER;
    }

    switch (utility::hash(t->get_full_name())) {
    case "System.Single"_fnv:
    case "System.Double"_fnv:
        return NumericParam::FLOATING;
    case "System.Boolean"_fnv:
    case "System.Char"_fnv:
    case "System.SByte"_fnv:
    case "System.Byte"_fnv:
    case "System.Int16"_fnv:
    case "System.UInt16"_fnv:
    case "System.Int32"_fnv:
    case "System.UInt32"_fnv:
    case "System.Int64"_fnv:
    case "System.UInt64"_fnv:
        return NumericParam::INTEGRAL;
    default:
        return NumericParam::OTHER;
    }
}

// How each parameter of a method takes a Lua number, computed once per method: build_args runs for every call from
// Lua, often per frame, and get_param_types/get_param_modifiers allocate. Methods live as long as the type database,
// so the pointer is a stable key. thread_local because scripts can run on several threads.
const std::vector<NumericParam>& numeric_param_kinds(::sdk::REMethodDefinition* fn) {
    thread_local std::unordered_map<::sdk::REMethodDefinition*, std::vector<NumericParam>> cache{};

    if (auto it = cache.find(fn); it != cache.end()) {
        return it->second;
    }

    const auto param_types = fn->get_param_types();
    const auto param_modifiers = fn->get_param_modifiers();
    std::vector<NumericParam> kinds(param_types.size(), NumericParam::OTHER);

    for (size_t i = 0; i < param_types.size(); ++i) {
        // An out/ref/pointer parameter takes an address: a Lua number passed there is a pointer, not a value of the
        // pointed-to type.
        if (i < param_modifiers.size() && param_modifiers[i] != ::sdk::REMethodDefinition::ParamModifier::None) {
            continue;
        }

        kinds[i] = classify_numeric_param(param_types[i]);
    }

    return cache.emplace(fn, std::move(kinds)).first->second;
}
}

std::vector<void*>& build_args(sol::variadic_args va, ::sdk::REMethodDefinition* fn) {
    auto l = va.lua_state();

    static std::vector<void*> args{};
    static std::vector<Vector4f> vec_storage{};

    args.clear();
    vec_storage.clear();

    // Per-parameter number encoding, looked up only if a number argument needs it.
    const std::vector<NumericParam>* kinds = nullptr;

    const auto numeric_param = [&](size_t index) {
        if (fn == nullptr) {
            return NumericParam::OTHER;
        }

        if (kinds == nullptr) {
            kinds = &numeric_param_kinds(fn);
        }

        return index < kinds->size() ? (*kinds)[index] : NumericParam::OTHER;
    };

    for (auto&& arg : va) {
        auto i = arg.stack_index();

        if (lua_isnil(l, i)) {
            args.push_back(nullptr);
            continue;
        }

        // sol2 doesn't seem to differentiate between Lua integers and numbers. So
        // we must do it ourselves.
        if (lua_isboolean(l, i)) {
            auto b = lua_toboolean(l, i);
            args.push_back((void*)(intptr_t)b);
        } else if (lua_type(l, i) == LUA_TSTRING) {
            auto s = lua_tostring(l, i);
            args.push_back(::sdk::VM::create_managed_string(utility::widen(s)));
        } else if (lua_type(l, i) == LUA_TNUMBER) {
            // Encode by the parameter's type when it is known, not by the Lua subtype: 5 passed to a
            // System.Single parameter must arrive as 5.0, and 4/2 (a float) passed to System.Int32 as 2.
            const auto kind = numeric_param(args.size());

            if (lua_isinteger(l, i) && kind != NumericParam::FLOATING) {
                args.push_back((void*)(intptr_t)lua_tointeger(l, i));
            } else if (kind == NumericParam::INTEGRAL) {
                args.push_back((void*)(intptr_t)(int64_t)lua_tonumber(l, i));
            } else {
                const double f = lua_tonumber(l, i);
                args.push_back(*(void**)&f);
            }
        } else if (arg.is<Vector2f>()) {
            auto& v = arg.as<Vector2f&>();
            args.push_back((void*)&vec_storage.emplace_back(v.x, v.y, 0.0f, 0.0f));
        } else if (arg.is<Vector3f>()) {
            auto& v = arg.as<Vector3f&>();
            args.push_back((void*)&vec_storage.emplace_back(v.x, v.y, v.z, 0.0f));
        } else if (arg.is<Vector4f>()) {
            auto& v = arg.as<Vector4f&>();
            args.push_back((void*)&v);
        } else if (arg.is<Matrix4x4f>()) {
            auto& v = arg.as<Matrix4x4f&>();
            args.push_back((void*)&v);
        } else if (arg.is<glm::quat>()) {
            auto& v = arg.as<glm::quat&>();
            args.push_back((void*)&v);
        } else if (arg.is<::REManagedObject*>()) {
            args.push_back(arg.as<::REManagedObject*>());
        } else if (arg.is<ValueType>()) {
            auto& b = arg.as<ValueType&>();
            args.push_back((void*)b.address());
        } else {
            args.push_back(arg.as<void*>());
        }
    }

    return args;
}

sol::object call_native_func_direct(sol::object obj, ::sdk::REMethodDefinition* fn, sol::variadic_args va) {
    auto l = va.lua_state();
    auto ret_ty = fn->get_return_type();

    if (ret_ty == nullptr) {
        return sol::make_object(l, sol::nil);
    }

    auto real_obj = get_invoke_this(obj, fn);
    auto ret_val = fn->invoke(real_obj, build_args(va, fn));

    if (ret_val.exception_thrown) {
        throw sol::error("Invoke threw an exception");
    }

    return parse_data(l, &ret_val, ret_ty, true);
}

sol::object call_native_func(sol::object obj, ::sdk::RETypeDefinition* ty, const char* name, sol::variadic_args va) {
    auto l = va.lua_state();
    
    // Convert return values to the correct Lua types.
    auto fn = ty->get_method(name);

    if (fn == nullptr) {
        return sol::make_object(l, sol::nil);
    }
    return call_native_func_direct(obj, fn, va);
}

auto call_object_func(sol::object obj, const char* name, sol::variadic_args va) {
    auto real_obj = get_real_obj(obj);

    if (real_obj == nullptr) {
        return sol::make_object(obj.lua_state(), sol::nil);
    }

    auto def = ((::REManagedObject*)real_obj)->get_type_definition();

    if (def == nullptr) {
        return sol::make_object(obj.lua_state(), sol::nil);
    }

    return call_native_func(obj, def, name, va);
}

sol::object get_primary_camera(sol::this_state s) {
    return sol::make_object(s, (::REManagedObject*)::sdk::get_primary_camera());
}

bool is_managed_object(sol::object obj) {
    auto real_obj = get_real_obj(obj);

    if (real_obj == nullptr) {
        return false;
    }

    return REManagedObject::is_managed_object(real_obj);
}

void hook(sol::this_state s, ::sdk::REMethodDefinition* fn, sol::protected_function pre_cb, sol::protected_function post_cb, sol::object ignore_jmp_object) {
    auto sol_state = sol::state_view{s};
    auto state = sol_state.registry()["state"].get<ScriptState*>();
    state->add_hook(fn, pre_cb, post_cb, ignore_jmp_object);
}

void hook_vtable(sol::this_state s, ::REManagedObject* obj, ::sdk::REMethodDefinition* fn, sol::protected_function pre_cb, sol::protected_function post_cb) {
    if (obj == nullptr) {
        throw sol::error("Object is null");
        return;
    }
    
    auto sol_state = sol::state_view{s};
    auto state = sol_state.registry()["state"].get<ScriptState*>();
    state->add_vtable(obj, fn, pre_cb, post_cb);
}
}

namespace api::re_managed_object {
sol::object index(sol::this_state s, sol::object lua_obj, sol::variadic_args args) {
    auto obj = lua_obj.as<REManagedObject*>();
    if (obj == nullptr) {
        throw sol::error("Attempted to index invalid REManagedObject");
    }
    auto index = args[0];

    auto type_def = obj->get_type_definition();
    std::string name;
    if (index.is<const char*>()) {
        name = index.as<const char*>();
        auto field = type_def->get_field(name.c_str());
        if (field != nullptr) {
            return api::sdk::get_native_field_from_field(lua_obj, type_def, field);
        }
        auto method = type_def->get_method(name.c_str());
        if (method != nullptr) {
            return sol::make_object(s, method);
        }
    }

    if (auto fn = type_def->get_method("get_Item"); fn != nullptr) {
        try {
            const auto params = fn->get_param_types();

            if (!params.empty()) {
                static auto system_object = sdk::find_type_definition("System.Object");
                static auto system_string = sdk::find_type_definition("System.String");

                const auto first_param = params[0];

                // If it's a System.Object it's always okay, our arg will get auto converted.
                if (first_param != nullptr && first_param != system_object) {
                    if (index.is<const char*>()) {
                        if (first_param != system_string) {
                            return sol::make_object(s, sol::nil);
                        }
                    }
                }
            }

            return ::api::sdk::call_native_func_direct(lua_obj, fn, args);
        } catch (...) {
            
        }
    }

    //throw sol::error("Attempted to index invalid REManagedObject field: " + name);
    return sol::make_object(s, sol::nil);
}

static int32_t checked_array_index(sol::stack_proxy index, ::sdk::SystemArray* arr) {
    if (index.get_type() != sol::type::number) {
        throw sol::error("Array index must be a number");
    }

    const auto d = index.as<double>();

    if (d != (double)(int64_t)d) {
        throw sol::error("Array index must be an integer");
    }

    if (d < 0.0 || d >= (double)arr->get_size()) {
        throw sol::error("Array index out of range: " + std::to_string((int64_t)d));
    }

    return (int32_t)d;
}

void new_index(sol::this_state s, sol::object lua_obj, sol::variadic_args args) {
    auto obj = lua_obj.as<REManagedObject*>();
    if (obj == nullptr) {
        throw sol::error("Attempted to new_index invalid REManagedObject");
    }

    auto index = args[0];
    auto assign = args[1];

    auto type_def = obj->get_type_definition();
    std::string name;
    if (index.is<const char*>()) {
        name = index.as<const char*>();
        auto field = type_def->get_field(name.c_str());

        if (field != nullptr) {
            return api::sdk::set_native_field_from_field(lua_obj, type_def, field, assign);
        }
    }

    if (auto fn = type_def->get_method("set_Item"); fn != nullptr) {
        const auto params = fn->get_param_types();

        if (!params.empty()) {
            static auto system_object = sdk::find_type_definition("System.Object");
            static auto system_string = sdk::find_type_definition("System.String");
            
            const auto first_param = params[0];

            // If it's a System.Object it's always okay, our arg will get auto converted.
            if (first_param != nullptr && first_param != system_object) {
                if (index.is<const char*>()) {
                    if (first_param != system_string) {
                        throw sol::error("Attempted to new_index invalid REManagedObject field: " + name);
                    }
                }
            }
        }

        ::api::sdk::call_native_func_direct(lua_obj, fn, args);
        return;
    }

    // System.Int32 -> object
    if (auto set_method = type_def->get_method("Set"); set_method != nullptr) {
        static auto system_int32 = sdk::find_type_definition("System.Int32");
        const auto params = set_method->get_param_types();

        if (params.size() == 2 && params[0] == system_int32) {
            if (type_def->is_array()) {
                checked_array_index(index, static_cast<::sdk::SystemArray*>(obj));

                // Reject type-confused raw writes (a string into a value element
                // would store the managed string's pointer bits). Dispatch on the
                // real Lua type: is<const char*>/is<double> are true for both.
                const auto elem_type = params[1];

                if (elem_type != nullptr) {
                    const auto lua_t = assign.get_type();
                    const auto is_string = lua_t == sol::type::string;
                    const auto is_number = lua_t == sol::type::number || lua_t == sol::type::boolean;

                    if (elem_type->is_value_type() && is_string) {
                        throw sol::error("Cannot assign a string to a " + elem_type->get_full_name() + " element");
                    }

                    if (!elem_type->is_value_type() && is_number) {
                        throw sol::error("Cannot assign a number to a " + elem_type->get_full_name() + " element");
                    }
                }
            }

            ::api::sdk::call_native_func_direct(lua_obj, set_method, args);
            return;
        }
    }

    throw sol::error("Attempted to new_index invalid REManagedObject field: " + name);
}

bool is_valid_offset(::REManagedObject* obj, int32_t offset) {
    if (obj == nullptr || !REManagedObject::is_managed_object(obj)) {
        return false;
    }

    const auto typedefinition = obj->get_type_definition();

    if (typedefinition == nullptr) {
        return false;
    }

    auto size = typedefinition->get_size();

    if (typedefinition->is_array()) {
        size = obj->get_size();
    }

    // trying to limit the impact of this function.
    if (offset < -0x1000 || (uint32_t)offset >= size) {
        return false;
    }

    return true;
}

template <typename T>
void write_memory(::REManagedObject* obj, int32_t offset, T value) {
    if (!is_valid_offset(obj, offset)) {
        return;
    }

    *(T*)((uintptr_t)obj + offset) = value;
}

template <typename T>
T read_memory(::REManagedObject* obj, int32_t offset) {
    if (!is_valid_offset(obj, offset)) {
        return 0;
    }

    return *(T*)((uintptr_t)obj + offset);
}
}

void bindings::scan_wrapper_references(lua_State* l) {
    detail::scan_wrapper_references(l);
}

void bindings::open_sdk(ScriptState* s) {
    auto& lua = s->lua();

    //lua["_sol_lua_push_objects"] = std::unordered_map<::REManagedObject*, sol::object>();
    // _sol_lua_push_ephemeral_counts is no longer written (it counted wrappers of local objects for their __gc, which
    // wrappers no longer have); it stays for scripts and tools that read it.
    lua.do_string(R"(
        _sol_lua_push_objects = setmetatable({}, { __mode = "v" })
        _sol_lua_push_ref_counts = {}
        _sol_lua_push_ephemeral_counts = {}
    )");

    // The push hot path reaches these through the registry instead of by global name (see detail::s_objects_key).
    {
        const auto l = lua.lua_state();

        lua_getglobal(l, "_sol_lua_push_objects");
        lua_rawsetp(l, LUA_REGISTRYINDEX, &detail::s_objects_key);
        lua_getglobal(l, "_sol_lua_push_ref_counts");
        lua_rawsetp(l, LUA_REGISTRYINDEX, &detail::s_ref_counts_key);

        // Tracked engine references (detail::WrapperLifetime); the userdata's __gc releases them when the state closes.
        const auto box = (detail::WrapperLifetimeBox*)lua_newuserdatauv(l, sizeof(detail::WrapperLifetimeBox), 0);
        box->lifetime = nullptr;
        lua_createtable(l, 0, 1);
        lua_pushcfunction(l, &detail::wrapper_lifetime_gc);
        lua_setfield(l, -2, "__gc");
        lua_setmetatable(l, -2);
        box->lifetime = new detail::WrapperLifetime{};
        lua_rawsetp(l, LUA_REGISTRYINDEX, &detail::s_lifetime_key);
    }

    auto sdk = lua.create_table();
    sdk["get_tdb_version"] = []() -> int { return sdk::RETypeDB::get()->get_version(); };
    sdk["game_namespace"] = game_namespace;
    sdk["get_thread_context"] = api::sdk::get_thread_context;
    sdk["get_native_singleton"] = api::sdk::get_native_singleton;
    sdk["get_managed_singleton"] = api::sdk::get_managed_singleton;
    sdk["create_managed_string"] = api::sdk::create_managed_string;
    sdk["create_managed_array"] = api::sdk::create_managed_array;
    sdk["create_delegate"] = api::sdk::create_delegate;
    sdk["create_sbyte"] = api::sdk::create_sbyte;
    sdk["create_byte"] = api::sdk::create_byte;
    sdk["create_int16"] = api::sdk::create_int16;
    sdk["create_uint16"] = api::sdk::create_uint16;
    sdk["create_int32"] = api::sdk::create_int32;
    sdk["create_uint32"] = api::sdk::create_uint32;
    sdk["create_int64"] = api::sdk::create_int64;
    sdk["create_uint64"] = api::sdk::create_uint64;
    sdk["create_single"] = api::sdk::create_single;
    sdk["create_double"] = api::sdk::create_double;
    sdk["create_resource"] = api::sdk::create_resource;
    sdk["create_userdata"] = api::sdk::create_userdata;
    sdk["create_userdata_global"] = api::sdk::create_userdata_global;
    sdk["create_instance"] = api::sdk::create_instance;
    sdk["create_instance_global"] = api::sdk::create_instance_global;
    sdk["find_type_definition"] = api::sdk::find_type_definition;
    sdk["typeof"] = api::sdk::typeof;
    sdk["call_native_func"] = api::sdk::call_native_func;
    sdk["call_object_func"] = api::sdk::call_object_func;
    sdk["get_native_field"] = api::sdk::get_native_field;
    sdk["set_native_field"] = api::sdk::set_native_field;
    sdk["get_primary_camera"] = api::sdk::get_primary_camera;
    sdk["copy_to_clipboard"] = api::sdk::copy_to_clipboard;
    sdk["hook"] = api::sdk::hook;
    sdk["hook_vtable"] = api::sdk::hook_vtable;
    sdk.new_enum("PreHookResult", "CALL_ORIGINAL", HookManager::PreHookResult::CALL_ORIGINAL, "SKIP_ORIGINAL", HookManager::PreHookResult::SKIP_ORIGINAL);
    sdk["is_managed_object"] = api::sdk::is_managed_object;
    sdk["to_managed_object"] = [](sol::this_state s, sol::object ptr) { 
        if (ptr.is<::REManagedObject*>()) {
            // no need to convert to managed object.
            return sol::make_object(s, ptr.as<::REManagedObject*>());
        } else if (ptr.is<uintptr_t>()) {
            return sol::make_object(s, (::REManagedObject*)ptr.as<uintptr_t>());
        } else if (ptr.is<void*>()) {
            return sol::make_object(s, (::REManagedObject*)ptr.as<void*>());
        } else {
            //throw sol::error("Object passed was not a managed object, uintptr_t, or void*.");
            return sol::make_object(s, sol::nil);
        }
    };

    sdk["to_valuetype"] = [](sol::this_state s, sol::object ptr, sol::object type) {
        if (ptr.is<api::sdk::ValueType>()) {
            return ptr;
        }

        ::sdk::RETypeDefinition* ty;
        if (type.is<::sdk::RETypeDefinition*>()) {
           ty = type.as<::sdk::RETypeDefinition*>();
        } else if (type.is<const char*>()) {
            ty = api::sdk::find_type_definition(type.as<const char*>());
        }
        if (ty == nullptr || !ty->is_value_type()) {
            return sol::make_object(s, sol::nil);
        }

        void* addr = nullptr;
        if (ptr.is<uintptr_t>()) {
            addr = (void*) ptr.as<uintptr_t>();
        } else if (ptr.is<void*>()) {
            addr = ptr.as<void*>();
        }
        
        if (addr == nullptr) {
            return sol::make_object(s, sol::nil);
        }
        return sol::make_object(s, api::sdk::ValueType{ty, addr});
    },

    sdk["deserialize"] = [](sol::this_state s, sol::object data_obj) -> sol::object {
        if (!data_obj.is<std::vector<uint8_t>>()) {
            throw sol::error("Data must be a vector of bytes");
        }

        auto data = data_obj.as<std::vector<uint8_t>>();
        auto result = REManagedObject::deserialize(data.data(), data.size(), false);

        // Explicitly create a lua table so we know for certain we are
        // pushing the REManagedObjects to the stack, adding a reference to them.
        auto final_result = sol::state_view{s}.create_table();

        for (auto obj : result) {
            final_result[final_result.size() + 1] = sol::make_object(s, obj);
        }

        return final_result;
    };
    sdk["to_resource"] = [](sol::this_state s, void* ptr) { return sol::make_object(s, (::sdk::Resource*)ptr); };
    sdk["to_double"] = [](void* ptr) { return *(double*)&ptr; };
    sdk["to_float"] = [](void* ptr) { return *(float*)&ptr; };
    sdk["to_int64"] = [](void* ptr) { return *(int64_t*)&ptr; };
    sdk["to_ptr"] = [](sol::object obj) {
        if (obj.is<int64_t>()) {
            const auto n = obj.as<int64_t>();
            return *(void**)&n;
        } else if (obj.is<::REManagedObject*>()) {
            return (void*)obj.as<::REManagedObject*>();
        } else if (obj.is<double>()) {
            const auto n = obj.as<double>();
            return *(void**)&n;
        } else if (obj.is<bool>()) {
            const auto n = (uintptr_t)obj.as<bool>();
            return *(void**)&n;
        } else {
            return obj.as<void*>();
        }
    };
    sdk["float_to_ptr"] = [](float f) {
        uintptr_t n = *(uintptr_t*)&f;
        return *(void**)&f;
    };
    lua["sdk"] = sdk;

    lua.new_usertype<::sdk::RETypeDefinition>("RETypeDefinition",
        sol::meta_function::equal_to, [](::sdk::RETypeDefinition* lhs, ::sdk::RETypeDefinition* rhs) { return lhs == rhs; },
        "get_methods", [](sdk::RETypeDefinition* def) -> std::vector<sdk::REMethodDefinition*> {
            std::vector<sdk::REMethodDefinition*> methods{};

            for (auto& method : def->get_methods()) {
                const auto func = method.get_function();

                if (method.get_function() == nullptr) {
                    continue;
                }

                const auto bytes = (uint8_t*)func;

                if (utility::is_stub_code(bytes)) {
                    continue;
                }

                methods.push_back(&method);
            }

            return methods;
        },
        "get_fields", [](sdk::RETypeDefinition* def) -> std::vector<sdk::REField*> {
            std::vector<sdk::REField*> fields{};

            for (auto field : def->get_fields()) {
                fields.push_back(field);
            }

            return fields;
        },
        "get_full_name", &sdk::RETypeDefinition::get_full_name,
        "get_name", &sdk::RETypeDefinition::get_name,
        "get_namespace", &sdk::RETypeDefinition::get_namespace,
        "get_method", &::sdk::RETypeDefinition::get_method,
        "get_field", &::sdk::RETypeDefinition::get_field,
        "get_runtime_type", &::sdk::RETypeDefinition::get_runtime_type,
        "get_parent_type", &::sdk::RETypeDefinition::get_parent_type,
        "get_fqn_hash", &::sdk::RETypeDefinition::get_fqn_hash,
        "get_crc_hash", &::sdk::RETypeDefinition::get_crc_hash,
        "get_size", &::sdk::RETypeDefinition::get_size,
        "get_valuetype_size", &::sdk::RETypeDefinition::get_valuetype_size,
        "get_generic_argument_types", &::sdk::RETypeDefinition::get_generic_argument_types,
        "get_generic_type_definition", &::sdk::RETypeDefinition::get_generic_type_definition,
        "is_value_type", &::sdk::RETypeDefinition::is_value_type,
        "is_enum", &::sdk::RETypeDefinition::is_enum,
        "is_array", &::sdk::RETypeDefinition::is_array,
        "is_by_ref", &::sdk::RETypeDefinition::is_by_ref,
        "is_pointer", &::sdk::RETypeDefinition::is_pointer,
        "is_primitive", &::sdk::RETypeDefinition::is_primitive,
        "is_generic_type", &::sdk::RETypeDefinition::is_generic_type,
        "is_generic_type_definition", &::sdk::RETypeDefinition::is_generic_type_definition,
        "is_a", [](sdk::RETypeDefinition* def, sol::object comp) -> bool {
            if (comp.is<sdk::RETypeDefinition*>()) {
                return def->is_a(comp.as<sdk::RETypeDefinition*>());
            } else if (comp.is<const char*>()) {
                return def->is_a(comp.as<const char*>());
            }

            return false;
        },
        "create_instance", &::sdk::RETypeDefinition::create_instance_full,
        "create_instance_gc_safe", [](sol::this_state s, ::sdk::RETypeDefinition* type_def_obj) {
            auto obj = type_def_obj->create_instance_full();
            return api::sdk::create_global_object(s, obj);
        },
        "get_types_inheriting_from_this", &::sdk::RETypeDefinition::get_types_inherting_from_this
    );

    auto method_call = [](sdk::REMethodDefinition* def, sol::object obj, sol::variadic_args va) {
        auto l = va.lua_state();

        auto real_obj = ::api::sdk::get_invoke_this(obj, def);
        auto ret_val = def->invoke(real_obj, ::api::sdk::build_args(va, def));

        if (ret_val.exception_thrown) {
            throw sol::error("Invoke threw an exception");
        }

        // Convert return values to the correct Lua types.
        auto ret_ty = def->get_return_type();

        return ::api::sdk::parse_data(l, &ret_val, ret_ty, true);
    };
    
    lua.new_usertype<sdk::REMethodDefinition>("REMethodDefinition",
        sol::meta_function::call, method_call,
        "get_name", &sdk::REMethodDefinition::get_name,
        "get_return_type", &sdk::REMethodDefinition::get_return_type,
        "get_function", &sdk::REMethodDefinition::get_function,
        "get_declaring_type", &sdk::REMethodDefinition::get_declaring_type,
        "get_num_params", &sdk::REMethodDefinition::get_num_params,
        "get_param_types", &sdk::REMethodDefinition::get_param_types,
        "get_param_names", &sdk::REMethodDefinition::get_param_names,
        "is_static", &sdk::REMethodDefinition::is_static,
        "call", method_call
    );
    
    lua.new_usertype<sdk::REField>("REField",
        "get_name", &sdk::REField::get_name,
        "get_type", &sdk::REField::get_type,
        "get_offset_from_base", &sdk::REField::get_offset_from_base,
        "get_offset_from_fieldptr", &sdk::REField::get_offset_from_fieldptr,
        "get_declaring_type", &sdk::REField::get_declaring_type,
        "get_flags", &sdk::REField::get_flags,
        "is_static", &sdk::REField::is_static,
        "is_literal", &sdk::REField::is_literal,
        "get_data", [s](sdk::REField* f, sol::object obj) -> sol::object {
            auto& l = s->lua();

            if (obj.is<::REManagedObject*>()) {
                auto managed_obj = obj.as<::REManagedObject*>();

                if (managed_obj == nullptr) {
                    return sol::make_object(l, sol::nil);
                }

                auto ty = managed_obj->get_type_definition();

                if (ty == nullptr) {
                    return sol::make_object(l, sol::nil);
                }

                return api::sdk::get_native_field_from_field(obj, ty, f);
            }

            auto ty = f->get_declaring_type();

            if (ty == nullptr) {
                return sol::make_object(l, sol::nil);
            }

            return api::sdk::get_native_field_from_field(obj, ty, f);
        }
    );

    lua.new_usertype<::REManagedObject>("REManagedObject",
        sol::meta_function::equal_to, [s](REManagedObject* lhs, REManagedObject* rhs) { return lhs == rhs; },
        sol::meta_function::index, &api::re_managed_object::index,
        sol::meta_function::new_index, &api::re_managed_object::new_index,
        "add_ref", &api::re_managed_object::add_ref,
        "add_ref_permanent", &api::re_managed_object::add_ref_permanent,
        "force_release", [](sol::this_state s, ::REManagedObject* obj) {
            api::re_managed_object::release(s, obj, true);
        },
        "release", [](sol::this_state s, ::REManagedObject* obj) {
            api::re_managed_object::release(s, obj, false);
        },
        "deserialize_native", [](sol::this_state s, ::REManagedObject* obj, sol::object data_obj, sol::object objects_obj) {
            if (!data_obj.is<std::vector<uint8_t>>()) {
                throw sol::error("Data must be a vector of bytes");
            }

            auto data = data_obj.as<std::vector<uint8_t>>();

            std::vector<::REManagedObject*> objects{};

            if (objects_obj.is<sol::table>()) {
                sol::table objects_table = objects_obj.as<sol::table>();

                for (auto i = 1; i <= objects_table.size(); i++) {
                    auto obj = objects_table.get<::REManagedObject*>(i);
                    objects.push_back(obj);
                }
            }

            return obj->deserialize_native(data.data(), data.size(), objects);
        },
        // Without a last release REFramework still owes the engine: 0 right after the last release() (see
        // REManagedObject::get_visible_ref_count for a game where the engine delays it once more).
        "get_reference_count", [] (::REManagedObject* obj) { return obj->get_visible_ref_count(); },
        "get_address", [](REManagedObject* obj) { return (uintptr_t)obj; },
        "get_type_definition", [](::REManagedObject* obj) { return obj->get_type_definition(); },
        "get_field", [s](REManagedObject* obj, const char* name) {
            if (obj == nullptr) {
                return sol::make_object(s->lua(), sol::nil);
            }

            return api::sdk::get_native_field(sol::make_object(s->lua(), obj), obj->get_type_definition(), name); 
        },
        "get_object_size", [](::REManagedObject* obj) { return obj->get_size(); },
        "set_field", [s](REManagedObject* obj, const char* name, sol::object value) {
            if (obj == nullptr) {
                return;
            }

            return api::sdk::set_native_field(sol::this_state{s->lua()}, sol::make_object(s->lua(), obj), obj->get_type_definition(), name, value); 
        },
        "call", [s](REManagedObject* obj, const char* name, sol::variadic_args args) {
            if (obj == nullptr) {
                return sol::make_object(s->lua(), sol::nil);
            }

            return api::sdk::call_object_func(sol::make_object(s->lua(), obj), name, args);
        },
        "write_byte", &api::re_managed_object::write_memory<uint8_t>,
        "write_short", &api::re_managed_object::write_memory<uint16_t>,
        "write_dword", &api::re_managed_object::write_memory<uint32_t>,
        "write_qword", &api::re_managed_object::write_memory<int64_t>,
        "write_float", &api::re_managed_object::write_memory<float>,
        "write_double", &api::re_managed_object::write_memory<double>,
        "read_byte", &api::re_managed_object::read_memory<uint8_t>,
        "read_short", &api::re_managed_object::read_memory<uint16_t>,
        "read_dword", &api::re_managed_object::read_memory<uint32_t>,
        "read_qword", &api::re_managed_object::read_memory<int64_t>,
        "read_float", &api::re_managed_object::read_memory<float>,
        "read_double", &api::re_managed_object::read_memory<double>
    );

    lua.new_usertype<REComponent>("REComponent",
        sol::base_classes, sol::bases<::REManagedObject>(),
        sol::meta_function::index, &api::re_managed_object::index,
        sol::meta_function::new_index, &api::re_managed_object::new_index
    );

    lua.new_usertype<RETransform>("RETransform",
        sol::meta_function::index, &api::re_managed_object::index,
        sol::meta_function::new_index, &api::re_managed_object::new_index,
        "calculate_base_transform", &utility::re_transform::calculate_base_transform,
        "calculate_tpose_pos_world", &utility::re_transform::calculate_tpose_pos_world,
        "apply_joints_tpose", [](RETransform* t, sol::object joints, uint32_t additional_parents) {
            if (t == nullptr) {
                return;
            }

            if (!joints.is<sol::table>()) {
                throw sol::error("RETransform:apply_joints_tpose expected a table of joints");
                return;
            }

            std::vector<::REJoint*> joints_vec{};
            auto joint_tbl = joints.as<sol::table>();

            // convert sol::table to std::vector
            for (auto i = 1; i <= joint_tbl.size(); i++) {
                auto joint = joint_tbl.get<::REManagedObject*>(i);
                joints_vec.push_back((::REJoint*)joint);
            }

            utility::re_transform::apply_joints_tpose(*t, joints_vec, additional_parents);
        },
        "set_position", &sdk::set_transform_position,
        "set_rotation", &sdk::set_transform_rotation,
        "get_position", &sdk::get_transform_position,
        "get_rotation", &sdk::get_transform_rotation,
        "hook_update", [](sol::this_state s, RETransform* transform, sol::protected_function fn) {
            if (transform == nullptr) {
                return;
            }
            auto sol_state = sol::state_view{s};
            auto state = sol_state.registry()["state"].get<ScriptState*>();
            state->add_update_transform(transform, fn);
        },
        sol::base_classes, sol::bases<::REComponent, ::REManagedObject>()
    );

    lua.new_usertype<sdk::SystemArray>("SystemArray",
        "get_size", &sdk::SystemArray::get_size,
        "get_element", &sdk::SystemArray::get_element,
        "get_elements", &sdk::SystemArray::get_elements,
        sol::meta_function::index, [](sol::this_state s, sdk::SystemArray* arr, sol::variadic_args args) {
            auto index = args[0];
            if (index.get_type() == sol::type::number) {
                const auto d = index.as<double>();
                if (d != (double)(int64_t)d || d < 0.0 || d >= (double)arr->get_size()) {
                    return sol::make_object(s.L, sol::nil);
                }
                return sol::make_object(s.L, arr->get_element((int32_t)d));
            }
            return api::re_managed_object::index(s, sol::make_object(s.L, arr), args);
        },
        sol::meta_function::new_index, [](sol::this_state s, sdk::SystemArray* arr, sol::variadic_args args) {
            auto index = args[0];
            auto value = args[1];
            if (index.get_type() == sol::type::number && value.is<REManagedObject*>()) {
                return arr->set_element(api::re_managed_object::checked_array_index(index, arr), value);
            }
            return api::re_managed_object::new_index(s, sol::make_object(s.L, arr), args);
        },
        sol::meta_function::pairs, [](sol::this_state s, sol::object arr) -> sol::variadic_results {
            auto next = [](sol::this_state s, sdk::SystemArray* arr, sol::object k) -> sol::variadic_results {
                uint32_t i = k.is<sol::nil_t>() ? 0 : k.as<uint32_t>()+1;
                sol::variadic_results results{};
                if (i >= arr->size()) {
                    results.push_back(sol::make_object(s, sol::nil));\
                    results.push_back(sol::make_object(s, sol::nil));
                    return results;
                }
                results.push_back(sol::make_object(s, i));
                results.push_back(sol::make_object(s, arr->get_element(i)));
                return results;
            };
            sol::variadic_results results{};
            results.push_back(sol::make_object(s, next));
            results.push_back(arr);
            results.push_back(sol::make_object(s, sol::nil));
            return results;
        },
        sol::base_classes, sol::bases<::REManagedObject>()
    );

    lua.new_usertype<sdk::Delegate>("Delegate",
        "add", &api::delegate::add_lua_delegate,
        "invoke", &sdk::Delegate::invoke,
        sol::meta_function::new_index, &api::delegate::new_index,
        sol::base_classes, sol::bases<::REManagedObject>()
    );
    
    lua.new_usertype<api::sdk::ValueType>("ValueType",
        sol::meta_function::construct, sol::constructors<api::sdk::ValueType(sdk::RETypeDefinition*)>(),
        sol::meta_function::index, &api::sdk::ValueType::index,
        sol::meta_function::new_index, &api::sdk::ValueType::new_index,
        "data", &api::sdk::ValueType::data,
        "type", &api::sdk::ValueType::type,
        "write_byte", &api::sdk::ValueType::write_memory<uint8_t>,
        "write_short", &api::sdk::ValueType::write_memory<uint16_t>,
        "write_dword", &api::sdk::ValueType::write_memory<uint32_t>,
        "write_qword", &api::sdk::ValueType::write_memory<int64_t>,
        "write_float", &api::sdk::ValueType::write_memory<float>,
        "write_double", &api::sdk::ValueType::write_memory<double>,
        "read_byte", &api::sdk::ValueType::read_memory<uint8_t>,
        "read_short", &api::sdk::ValueType::read_memory<uint16_t>,
        "read_dword", &api::sdk::ValueType::read_memory<uint32_t>,
        "read_qword", &api::sdk::ValueType::read_memory<int64_t>,
        "read_float", &api::sdk::ValueType::read_memory<float>,
        "read_double", &api::sdk::ValueType::read_memory<double>,
        "address", &api::sdk::ValueType::address,
        "get_address", &api::sdk::ValueType::address,
        "call", &api::sdk::ValueType::call,
        "get_field", &api::sdk::ValueType::get_field,
        "set_field", &api::sdk::ValueType::set_field,
        "get_type_definition", [](api::sdk::ValueType* b) { return b->type; }
    );

    lua.new_usertype<api::sdk::MemoryView>("MemoryView",
        "write_byte", &api::sdk::MemoryView::write_memory<uint8_t>,
        "write_short", &api::sdk::MemoryView::write_memory<uint16_t>,
        "write_dword", &api::sdk::MemoryView::write_memory<uint32_t>,
        "write_qword", &api::sdk::MemoryView::write_memory<int64_t>,
        "write_float", &api::sdk::MemoryView::write_memory<float>,
        "write_double", &api::sdk::MemoryView::write_memory<double>,
        "read_byte", &api::sdk::MemoryView::read_memory<uint8_t>,
        "read_short", &api::sdk::MemoryView::read_memory<uint16_t>,
        "read_dword", &api::sdk::MemoryView::read_memory<uint32_t>,
        "read_qword", &api::sdk::MemoryView::read_memory<int64_t>,
        "read_float", &api::sdk::MemoryView::read_memory<float>,
        "read_double", &api::sdk::MemoryView::read_memory<double>,
        "address", &api::sdk::MemoryView::address,
        "get_address", &api::sdk::MemoryView::address,
        "wipe", &api::sdk::MemoryView::wipe,
        "size", &api::sdk::MemoryView::size
    );

    lua.new_usertype<::sdk::Resource>("REResource",
        "add_ref", [](sol::this_state s, ::sdk::Resource* res) { 
            res->add_ref();

            return sol::make_object(s, res);
        },
        "release", [](::sdk::Resource* res) {
            res->release();
        },
        "create_holder", &::api::sdk::create_holder,
        "create_holder_global", &::api::sdk::create_holder_global,
        "get_address", [](::sdk::Resource* res) { return (uintptr_t)res; }
    );

#define DYNAMIC_ARRAY_TYPE(C, T, name) \
    lua.new_usertype< C < T >>( name , \
        "as_memoryview", [](C < T >& data) {\
            return api::sdk::MemoryView((uint8_t*)&data, sizeof(C < T >));\
        },\
        "empty", & C < T >::empty, \
        "emplace", & C < T >::emplace, \
        "push_back", [](C < T >& arr, T value) { arr.push_back(value); },\
        "pop_back", & C < T >::pop_back,\
        "size", & C < T >::size,\
        "get_size", & C < T >::size,\
        "erase", & C < T >::erase,\
        "clear", & C < T >::clear,\
        sol::meta_function::index, [](sol::this_state s, C < T >& arr, uint32_t i) -> sol::object {\
            if (i >= arr.size()) { \
                return sol::make_object(s, sol::nil); \
            } \
            return sol::make_object(s, arr[i]);\
        },\
        sol::meta_function::new_index, [](C < T >& arr, uint32_t i, T value) {\
            if (i >= arr.size()) { \
                return; \
            } \
            arr[i] = value;\
        },\
        sol::meta_function::pairs, [](sol::this_state s, sol::object arr) -> sol::variadic_results {\
            auto next = [](sol::this_state s, C < T >& arr, sol::object k) -> sol::variadic_results {\
                uint32_t i = k.is<sol::nil_t>() ? 0 : k.as<uint32_t>()+1;\
                sol::variadic_results results{};\
                if (i >= arr.size()) { \
                    results.push_back(sol::make_object(s, sol::nil));\
                    results.push_back(sol::make_object(s, sol::nil));\
                    return results;\
                } \
                results.push_back(sol::make_object(s, i));\
                results.push_back(sol::make_object(s, arr[i]));\
                return results;\
            };\
            sol::variadic_results results{};\
            results.push_back(sol::make_object(s, next));\
            results.push_back(arr);\
            results.push_back(sol::make_object(s, sol::nil));\
            return results;\
        },\
        sol::meta_function::length, &C< T >::size\
    )

#define DYNAMIC_ARRAY_TYPE_REF(C, T, name) \
    lua.new_usertype< C < T >>( name , \
        "as_memoryview", [](C < T >& data) {\
            return api::sdk::MemoryView((uint8_t*)&data, sizeof(C < T >));\
        },\
        "empty", & C < T >::empty, \
        "emplace", & C < T >::emplace, \
        "push_back", [](C < T >& arr, T* value) { arr.push_back(*value); },\
        "pop_back", & C < T >::pop_back,\
        "size", & C < T >::size,\
        "get_size", & C < T >::size,\
        "erase", & C < T >::erase,\
        "clear", & C < T >::clear,\
        sol::meta_function::index, [](sol::this_state s, C < T >& arr, uint32_t i) -> sol::object {\
            if (i >= arr.size()) { \
                return sol::make_object(s, sol::nil); \
            } \
            return sol::make_object(s, &arr[i]);\
        },\
        sol::meta_function::new_index, [](C < T >& arr, uint32_t i, T* value) {\
            if (i >= arr.size()) { \
                return; \
            } \
            arr[i] = *value;\
        },\
        sol::meta_function::length, &C< T >::size\
    )


#define DYNAMIC_ARRAY_TYPE_PTR(C, T, name) \
    lua.new_usertype< C < T >>( name , \
        "as_memoryview", [](C < T >& data) {\
            return api::sdk::MemoryView((uint8_t*)&data, sizeof(C < T >));\
        },\
        "empty", & C < T >::empty, \
        "emplace", & C < T >::emplace, \
        "push_back", [](C < T >& arr, T value) { arr.push_back(value); },\
        "pop_back", & C < T >::pop_back,\
        "size", & C < T >::size,\
        "get_size", & C < T >::size,\
        "erase", & C < T >::erase,\
        "clear", & C < T >::clear,\
        sol::meta_function::index, [](sol::this_state s, C < T >& arr, uint32_t i) -> sol::object {\
            if (i >= arr.size()) { \
                return sol::make_object(s, sol::nil); \
            } \
            return sol::make_object(s, arr[i]);\
        },\
        sol::meta_function::new_index, [](C < T >& arr, uint32_t i, T value) {\
            if (i >= arr.size()) { \
                return; \
            } \
            arr[i] = value;\
        },\
        sol::meta_function::length, &C< T >::size\
    )

#define DYNAMIC_ARRAY_NOCAP_TYPE(T, name) DYNAMIC_ARRAY_TYPE(sdk::NativeArrayNoCapacity, T, name)
#define DYNAMIC_ARRAY_NOCAP_TYPE_REF(T, name) DYNAMIC_ARRAY_TYPE_REF(sdk::NativeArrayNoCapacity, T, name)
#define DYNAMIC_ARRAY_CAP_TYPE_PTR(T, name) DYNAMIC_ARRAY_TYPE_PTR(sdk::NativeArray, T, name)

    DYNAMIC_ARRAY_NOCAP_TYPE(uint8_t, "DynamicArrayNoCapacityUInt8");
    DYNAMIC_ARRAY_NOCAP_TYPE(int32_t, "DynamicArrayNoCapacityInt32");
    DYNAMIC_ARRAY_NOCAP_TYPE(uint32_t, "DynamicArrayNoCapacityUInt32");

    DYNAMIC_ARRAY_NOCAP_TYPE_REF(sdk::NativeArrayNoCapacity<uint32_t>, "DynamicArrayNoCapacityUInt32Array");

    lua.new_usertype<::sdk::behaviortree::TreeNodeData>("BehaviorTreeNodeData",
        "as_memoryview", [](::sdk::behaviortree::TreeNodeData* data) {
            return api::sdk::MemoryView((uint8_t*)data, sdk::behaviortree::tree_node_data_stride());
        },
        "id", sol::property(&::sdk::behaviortree::TreeNodeData::get_id),
        "parent", sol::property(&::sdk::behaviortree::TreeNodeData::get_parent),
        "is_branch", sol::property(&::sdk::behaviortree::TreeNodeData::get_is_branch),
        "is_end", sol::property(&::sdk::behaviortree::TreeNodeData::get_is_end),
        "has_selector", sol::property(&::sdk::behaviortree::TreeNodeData::get_has_selector),
        //"selector_id", ...,
        "attr", sol::property(&::sdk::behaviortree::TreeNodeData::get_attr),
        "parent_2", sol::property(&::sdk::behaviortree::TreeNodeData::get_parent_2),
        "get_children", &::sdk::behaviortree::TreeNodeData::get_children,
        "get_actions", &::sdk::behaviortree::TreeNodeData::get_actions,
        "get_states", &::sdk::behaviortree::TreeNodeData::get_states,
        "get_states_2", &::sdk::behaviortree::TreeNodeData::get_states_2,
        "get_start_states", &::sdk::behaviortree::TreeNodeData::get_start_states,
        "get_start_transitions", &::sdk::behaviortree::TreeNodeData::get_start_transitions,
        "get_conditions", &::sdk::behaviortree::TreeNodeData::get_conditions,
        "get_transition_conditions", &::sdk::behaviortree::TreeNodeData::get_transition_conditions,
        "get_transition_events", &::sdk::behaviortree::TreeNodeData::get_transition_events,
        "get_transition_ids", &::sdk::behaviortree::TreeNodeData::get_transition_ids,
        "get_transition_attributes", &::sdk::behaviortree::TreeNodeData::get_transition_attributes,
        "get_tags", &::sdk::behaviortree::TreeNodeData::get_tags,
        "get_name", &::sdk::behaviortree::TreeNodeData::get_name,
        "set_name", &::sdk::behaviortree::TreeNodeData::set_name
    );

    lua.new_usertype<::sdk::behaviortree::TreeNode>("BehaviorTreeNode",
        "as_memoryview", [](::sdk::behaviortree::TreeNode* node) {
            return api::sdk::MemoryView((uint8_t*)node, sdk::behaviortree::tree_node_stride());
        },
        "id", sol::property(&::sdk::behaviortree::TreeNode::get_id),
        "get_id", &::sdk::behaviortree::TreeNode::get_id,
        "get_data", &::sdk::behaviortree::TreeNode::get_data,
        "get_owner", &::sdk::behaviortree::TreeNode::get_owner,
        "get_parent", &::sdk::behaviortree::TreeNode::get_parent,
        "get_name", &::sdk::behaviortree::TreeNode::get_name,
        "set_name", &::sdk::behaviortree::TreeNode::set_name,
        "get_full_name", &::sdk::behaviortree::TreeNode::get_full_name,
        "get_children", &::sdk::behaviortree::TreeNode::get_children,
        "get_actions", &::sdk::behaviortree::TreeNode::get_actions,
        "get_unloaded_actions", &::sdk::behaviortree::TreeNode::get_unloaded_actions,
        "get_conditions", &::sdk::behaviortree::TreeNode::get_conditions,
        "get_transition_conditions", &::sdk::behaviortree::TreeNode::get_transition_conditions,
        "get_transition_events", &::sdk::behaviortree::TreeNode::get_transition_events,
        "get_states", &::sdk::behaviortree::TreeNode::get_states,
        "get_start_states", &::sdk::behaviortree::TreeNode::get_start_states,
        "get_status1", &::sdk::behaviortree::TreeNode::get_status1,
        "get_status2", &::sdk::behaviortree::TreeNode::get_status2,
        "relocate", &::sdk::behaviortree::TreeNode::relocate,
        "get_selector", [](sol::this_state s, ::sdk::behaviortree::TreeNode* node) {
            return sol::make_object(s, (::REManagedObject*)node->get_selector());
        }
    );

    // TreeNode and TreeNodeData are opaque; provide minimal array bindings without copy semantics.
    lua.new_usertype<sdk::NativeArrayNoCapacity<::sdk::behaviortree::TreeNode>>("DynamicArrayNoCapacityTreeNode",
        "as_memoryview", [](sdk::NativeArrayNoCapacity<::sdk::behaviortree::TreeNode>& data) {
            return api::sdk::MemoryView((uint8_t*)&data, sizeof(sdk::NativeArrayNoCapacity<::sdk::behaviortree::TreeNode>));
        },
        "size", &sdk::NativeArrayNoCapacity<::sdk::behaviortree::TreeNode>::size,
        "get_size", &sdk::NativeArrayNoCapacity<::sdk::behaviortree::TreeNode>::size,
        "empty", &sdk::NativeArrayNoCapacity<::sdk::behaviortree::TreeNode>::empty,
        sol::meta_function::index, [](sol::this_state s, sdk::NativeArrayNoCapacity<::sdk::behaviortree::TreeNode>& arr, uint32_t i) -> sol::object {
            if (i >= arr.size()) {
                return sol::make_object(s, sol::nil);
            }
            return sol::make_object(s, sdk::behaviortree::tree_node_at(arr.elements, i));
        },
        sol::meta_function::length, &sdk::NativeArrayNoCapacity<::sdk::behaviortree::TreeNode>::size
    );
    lua.new_usertype<sdk::NativeArrayNoCapacity<::sdk::behaviortree::TreeNodeData>>("DynamicArrayNoCapacityTreeNodeData",
        "as_memoryview", [](sdk::NativeArrayNoCapacity<::sdk::behaviortree::TreeNodeData>& data) {
            return api::sdk::MemoryView((uint8_t*)&data, sizeof(sdk::NativeArrayNoCapacity<::sdk::behaviortree::TreeNodeData>));
        },
        "size", &sdk::NativeArrayNoCapacity<::sdk::behaviortree::TreeNodeData>::size,
        "get_size", &sdk::NativeArrayNoCapacity<::sdk::behaviortree::TreeNodeData>::size,
        "empty", &sdk::NativeArrayNoCapacity<::sdk::behaviortree::TreeNodeData>::empty,
        sol::meta_function::index, [](sol::this_state s, sdk::NativeArrayNoCapacity<::sdk::behaviortree::TreeNodeData>& arr, uint32_t i) -> sol::object {
            if (i >= arr.size()) {
                return sol::make_object(s, sol::nil);
            }
            return sol::make_object(s, sdk::behaviortree::tree_node_data_at(arr.elements, i));
        },
        sol::meta_function::length, &sdk::NativeArrayNoCapacity<::sdk::behaviortree::TreeNodeData>::size
    );
    DYNAMIC_ARRAY_CAP_TYPE_PTR(::REManagedObject*, "DynamicArrayManagedObject");

    lua.new_usertype<::sdk::behaviortree::TreeObjectData>("BehaviorTreeObjectData",
        "as_memoryview", [](::sdk::behaviortree::TreeObjectData* data) {
            return api::sdk::MemoryView((uint8_t*)data, 0x300);
        },
        "get_nodes", &::sdk::behaviortree::TreeObjectData::get_nodes,
        "get_static_actions", &::sdk::behaviortree::TreeObjectData::get_static_actions,
        "get_static_conditions", &::sdk::behaviortree::TreeObjectData::get_static_conditions,
        "get_static_transitions", &::sdk::behaviortree::TreeObjectData::get_static_transitions,
        "get_expression_tree_conditions", &::sdk::behaviortree::TreeObjectData::get_expression_tree_conditions,
        "get_action_methods", &::sdk::behaviortree::TreeObjectData::get_action_methods,
        "get_static_action_methods", &::sdk::behaviortree::TreeObjectData::get_static_action_methods
    );

    lua.new_usertype<::sdk::behaviortree::TreeObject>("BehaviorTreeObject",
        "as_memoryview", [](::sdk::behaviortree::TreeObject* obj) {
            return api::sdk::MemoryView((uint8_t*)obj, 0xD8);
        },
        "get_data", &::sdk::behaviortree::TreeObject::get_data,
        "get_node_by_id", &::sdk::behaviortree::TreeObject::get_node_by_id,
        "get_node_by_name", [](::sdk::behaviortree::TreeObject* obj, const char* name) {
            return obj->get_node_by_name(name);
        },
        "get_actions", &::sdk::behaviortree::TreeObject::get_action_array,
        "get_nodes", &::sdk::behaviortree::TreeObject::get_node_array,
        "get_conditions", &::sdk::behaviortree::TreeObject::get_condition_array,
        "get_transitions", &::sdk::behaviortree::TreeObject::get_transition_array,
        "get_selectors", &::sdk::behaviortree::TreeObject::get_selector_array,
        "get_node", &::sdk::behaviortree::TreeObject::get_node,
        "get_node_count", &::sdk::behaviortree::TreeObject::get_node_count,
        "get_action", &::sdk::behaviortree::TreeObject::get_action,
        "get_unloaded_action", &::sdk::behaviortree::TreeObject::get_unloaded_action,
        "get_transition", &::sdk::behaviortree::TreeObject::get_transition,
        "get_condition", &::sdk::behaviortree::TreeObject::get_condition,
        "get_action_count", &::sdk::behaviortree::TreeObject::get_action_count,
        "get_condition_count", &::sdk::behaviortree::TreeObject::get_condition_count,
        "get_transition_count", &::sdk::behaviortree::TreeObject::get_transition_count,
        "get_unloaded_action_count", &::sdk::behaviortree::TreeObject::get_unloaded_action_count,
        "get_static_action_count", &::sdk::behaviortree::TreeObject::get_static_action_count,
        "get_static_condition_count", &::sdk::behaviortree::TreeObject::get_static_condition_count,
        "get_static_transition_count", &::sdk::behaviortree::TreeObject::get_static_transition_count,
        "relocate", &::sdk::behaviortree::TreeObject::relocate,
        "get_uservariable_hub", &::sdk::behaviortree::TreeObject::get_uservariable_hub
    );

    lua.new_usertype<api::sdk::BehaviorTreeCoreHandle>("BehaviorTreeCoreHandle",
        sol::meta_function::index, &api::re_managed_object::index,
        sol::meta_function::new_index, &api::re_managed_object::new_index,
        sol::base_classes, sol::bases<::REManagedObject>(),
        "get_tree_object", [](api::sdk::BehaviorTreeCoreHandle* handle) {
            return ((sdk::behaviortree::CoreHandle*)handle)->get_tree_object();
        },
        "relocate", [](api::sdk::BehaviorTreeCoreHandle* handle, uintptr_t old_start, uintptr_t old_end, sdk::NativeArrayNoCapacity<sdk::behaviortree::TreeNode>& new_nodes) {
            ((sdk::behaviortree::CoreHandle*)handle)->relocate(old_start, old_end, new_nodes);
        },
        "relocate_datas", [](api::sdk::BehaviorTreeCoreHandle* handle, uintptr_t old_start, uintptr_t old_end, sdk::NativeArrayNoCapacity<sdk::behaviortree::TreeNodeData>& new_nodes) {
            ((sdk::behaviortree::CoreHandle*)handle)->relocate_datas(old_start, old_end, new_nodes);
        }
    );

    lua.new_usertype<api::sdk::BehaviorTree>("BehaviorTree",
        sol::meta_function::index, &api::re_managed_object::index,
        sol::meta_function::new_index, &api::re_managed_object::new_index,
        sol::base_classes, sol::bases<::REManagedObject>(),
        "get_tree", [](api::sdk::BehaviorTree* tree, uint32_t index) {
            return ((sdk::behaviortree::BehaviorTree*)tree)->get_tree<api::sdk::BehaviorTreeCoreHandle>(index);
        },
        "get_tree_count", [](api::sdk::BehaviorTree* tree) {
            return ((sdk::behaviortree::BehaviorTree*)tree)->get_tree_count();
        },
        "get_trees", [](api::sdk::BehaviorTree* tree) {
            return ((sdk::behaviortree::BehaviorTree*)tree)->get_trees<api::sdk::BehaviorTreeCoreHandle>();
        }
    );
}
