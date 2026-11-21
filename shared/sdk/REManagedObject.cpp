#include <atomic>
#include <cstdint>
#include <mutex>
#include <vector>

#include <spdlog/spdlog.h>

#include "utility/Scan.hpp"
#include "utility/Module.hpp"

#include "ReClass.hpp"

#include "REManagedObject.hpp"
#include "GameIdentity.hpp"

#include "RETypeDefDispatch.hpp"
using namespace utility::re_type_accessor;

size_t REManagedObject::runtime_size() {
    // RE7 TDB49 has REManagedObject at 0x20 (enlarged REObject base with vtable).
    // All other supported games: 0x10. Modern RE7 (TDB70) is also 0x10.
    return 0x10;  // Stable across all supported games. Update if a future game changes it.
}

// Used all over the place. It may be one of the most referenced functions in a disassembler.
// According to my IDA script, it's the 4th most referenced function though sometimes it's 5th or 6th.
// Release usually has more references than add_ref.
// A dead giveaway is the comparison to byte ptr RCX + 0xE near the start of the function.
// Cheat sheet for functions to find these function(s) in:
// via.Transform::get_Children
static void (*add_ref_func)(::REManagedObject*) = nullptr;
static void (*release_func)(::REManagedObject*) = nullptr;

namespace {
// Last references REFramework gives up (REManagedObject::release), released to the engine two frames later.
//
// A small new object lives in one of the engine's per-thread pages. add_ref() on it makes a page the thread started
// since its last local cleanup global, with the object's reference as its only one. If that reference is released
// before the thread's next local cleanup, the engine destroys the object and frees the page at once, while the thread
// still allocates from it and lists it and other local objects live in it: the game crashes later in unrelated code.
// Game code never does that: inside the engine's update scopes a last release is queued until after every scope's
// cleanup. REFramework's callers (Lua, plugins, C#) release outside those scopes, so REFramework queues the last
// release itself and carries it out at the second frame boundary after it (drain_deferred_releases), when the
// thread's cleanup has run.
//
// Invariant: an object is queued at most once. A release that sees count 1 for an object already queued has no
// reference left to give up (the queued release is its last one) and is dropped; one that sees count > 1 always has
// a reference besides the queued one and goes to the engine at once. So the engine count never goes below what the
// queue still owes, and the queued release happens exactly once, as a normal release if add_ref() raised the count
// meanwhile.
//
// What callers see: get_visible_ref_count() is the engine count minus the queued reference, so a count reads 0 right
// after its last release (as it did before this queue in games where the engine destroys the object at once), and a
// release at visible count 0 is the over-release that is dropped. Internal code that decides whether an object is
// local or global reads the engine count (get_ref_count): a queued object is still a counted, global object, and an
// add_ref() on it is an ordinary increment.
//
// Limits (none of them a regression: each case behaves as before this queue):
// - The count check in release() is not atomic with the engine call. If another owner (a second REFramework thread,
//   the drain, or a game thread) drops the count from 2 to 1 between the check and the call, that call is the last
//   release and reaches the engine at once. REFramework can't do the decrement itself: the engine's non-last release
//   also does per-type bookkeeping.
// - Two frames help only threads whose engine context runs its local cleanup in between. An object made global and
//   released on a thread that never runs engine updates is freed at the drain while that thread may still point at
//   the page.
// - Last releases the engine makes itself inside calls REFramework starts outside a scope (method invokes, an
//   array's set method replacing an element) don't pass through here.
// - If the drain stops running (no BeginRendering while frames still end), nothing queued is released. That is
//   logged once, after STALL_FRAMES frame ends (EndRendering, note_frame_end) without a drain while something is
//   queued. Frames, not time: the first BeginRendering can run for tens of seconds (plugin initialization, e.g. the
//   C# runtime compiling its assemblies, and the first script frame run inside it), and no frame ends meanwhile.
//
// After the drain hands a release to the engine, the engine may delay it once more: it does so while any thread is
// inside one of its update scopes (RE9 at BeginRendering, for one), keeping the object at count 1 in its own list until
// it flushes that list. From then on get_visible_ref_count() reads the engine's count, 1, as it did before this queue
// in that game. It can't tell that delay from a reference another owner holds without reading the engine's own list,
// whose layout differs per game, so it doesn't try.
class DeferredReleases {
public:
    // Frame ends without a drain, with something queued, before the stall is logged (about 5 s at 60 fps).
    static constexpr uint32_t STALL_FRAMES = 300;

    // Called from any thread, with the caller's last reference (count 1).
    void push(::REManagedObject* obj) {
        std::scoped_lock _{m_mtx};

        if (contains(obj)) {
            if (!m_warned_over_release) {
                m_warned_over_release = true;
                spdlog::warn("[REManagedObject] release() of {:x} at visible reference count 0 (its last release is already queued); ignored (logged once)", (uintptr_t)obj);
            }

            return;
        }

        m_buckets[m_epoch & 1].push_back(obj);
        insert(obj);
        publish();

        if (!m_warned_size && m_set_count >= 100000) {
            m_warned_size = true;
            spdlog::warn("[REManagedObject] {} last releases queued at once (logged once)", m_set_count);
        }
    }

    // Game thread, at every EndRendering: counts frame ends without a drain while something is queued. One relaxed
    // atomic load when nothing is queued.
    void note_frame_end() {
        if (m_queued.load(std::memory_order_relaxed) == 0) {
            return;
        }

        if (m_frames_without_drain.fetch_add(1, std::memory_order_relaxed) + 1 == STALL_FRAMES) {
            std::scoped_lock _{m_mtx};

            if (!m_warned_stall) {
                m_warned_stall = true;
                spdlog::warn("[REManagedObject] {} frames ended without a drain while {} last releases are queued: the drain at BeginRendering isn't running, so they stay unreleased (logged once)",
                    STALL_FRAMES, m_set_count);
            }
        }
    }

    // obj's engine count minus the last release the queue still owes for it (queued, or being released by the drain
    // right now). The count is read under m_mtx, so it is never one from before a drain release that has finished.
    // No lock while nothing is queued.
    int32_t visible_count(const ::REManagedObject* obj, const volatile int32_t* count) {
        if (m_queued.load(std::memory_order_acquire) == 0) {
            return *count;
        }

        std::scoped_lock _{m_mtx};
        const auto c = *count;

        // A local (negative) or zero count can't include a queued reference.
        if (c > 0 && (obj == m_in_flight || contains(const_cast<::REManagedObject*>(obj)))) {
            return c - 1;
        }

        return c;
    }

    // Game thread, once a frame. Releases what was queued before the previous drain.
    template <typename Fn> void drain(Fn&& release) {
        std::scoped_lock drain_lock{m_drain_mtx};

        {
            std::scoped_lock _{m_mtx};
            m_draining.swap(m_buckets[(m_epoch + 1) & 1]);
            ++m_epoch;
            m_frames_without_drain.store(0, std::memory_order_relaxed);
        }

        // One at a time, outside m_mtx (an engine release can run destructors that call back into REFramework). Each
        // object leaves the set just before its release, so a legitimate owner's release from then on (count 1 = its
        // own reference) is queued, not dropped. Until the release has returned, visible_count() still subtracts it,
        // so the visible count never reads 1 for a reference nobody holds; for an owner that still holds one it can
        // read one low for that moment (never high).
        for (auto obj : m_draining) {
            {
                std::scoped_lock _{m_mtx};
                erase(obj);
                m_in_flight = obj;
                publish();
            }

            release(obj);

            {
                std::scoped_lock _{m_mtx};
                m_in_flight = nullptr;
                publish();
            }
        }

        m_draining.clear();
    }

private:
    // Under m_mtx: how many objects visible_count() may find, so its lock-free check sees 0 only when nothing is owed.
    void publish() {
        m_queued.store(m_set_count + (m_in_flight != nullptr ? 1 : 0), std::memory_order_release);
    }

    // Open-addressing set of every queued object (linear probing, nullptr = empty slot), so the at-most-once check
    // costs O(1) and allocates only when the set grows past its peak.
    size_t home(::REManagedObject* obj) const {
        return (size_t)((((uintptr_t)obj >> 4) * 0x9E3779B97F4A7C15ull) >> 32) & (m_set.size() - 1);
    }

    bool contains(::REManagedObject* obj) const {
        if (m_set.empty()) {
            return false;
        }

        for (auto i = home(obj); m_set[i] != nullptr; i = (i + 1) & (m_set.size() - 1)) {
            if (m_set[i] == obj) {
                return true;
            }
        }

        return false;
    }

    void insert(::REManagedObject* obj) {
        if ((m_set_count + 1) * 2 > m_set.size()) {
            std::vector<::REManagedObject*> old{};
            old.swap(m_set);
            m_set.assign(old.empty() ? 64 : old.size() * 2, nullptr);
            m_set_count = 0;

            for (auto o : old) {
                if (o != nullptr) {
                    insert(o);
                }
            }
        }

        auto i = home(obj);
        while (m_set[i] != nullptr) {
            i = (i + 1) & (m_set.size() - 1);
        }

        m_set[i] = obj;
        ++m_set_count;
    }

    // Backward-shift deletion: no tombstones, so lookups stay short however many objects passed through.
    void erase(::REManagedObject* obj) {
        if (m_set.empty()) {
            return;
        }

        const auto mask = m_set.size() - 1;
        auto i = home(obj);

        while (m_set[i] != obj) {
            if (m_set[i] == nullptr) {
                return;
            }

            i = (i + 1) & mask;
        }

        m_set[i] = nullptr;
        --m_set_count;

        for (auto j = (i + 1) & mask; m_set[j] != nullptr; j = (j + 1) & mask) {
            const auto h = home(m_set[j]);
            // m_set[j] stays only if its home lies cyclically in (i, j].
            const bool stays = i <= j ? (i < h && h <= j) : (i < h || h <= j);

            if (!stays) {
                m_set[i] = m_set[j];
                m_set[j] = nullptr;
                i = j;
            }
        }
    }

    std::mutex m_mtx{};
    std::mutex m_drain_mtx{};
    std::vector<::REManagedObject*> m_buckets[2]{}; // by the parity of the epoch they were queued in
    std::vector<::REManagedObject*> m_draining{};   // only while m_drain_mtx is held
    std::vector<::REManagedObject*> m_set{};
    size_t m_set_count{0};
    ::REManagedObject* m_in_flight{nullptr}; // the object the drain is releasing right now
    std::atomic<size_t> m_queued{0}; // see publish(); read without the lock
    uint32_t m_epoch{0};
    std::atomic<uint32_t> m_frames_without_drain{0}; // frame ends since the last drain while something was queued
    bool m_warned_over_release{false};
    bool m_warned_size{false};
    bool m_warned_stall{false};
};

// Never destroyed: at process exit the queue is dropped without releasing anything (the VM may already be gone).
DeferredReleases& deferred_releases() {
    static auto* const s_deferred = new DeferredReleases{};
    return *s_deferred;
}
}

void REManagedObject::resolve_add_ref() {
    if (add_ref_func != nullptr) {
        return;
    }

    constexpr std::array<std::string_view, 4> possible_patterns{
        "40 ? 48 83 EC ? 8B 41 ? 48 8B ? 85 C0 0F ? ? ? ? ? 0F ? ? 0E", // RE2+
        "40 ? 48 83 EC ? 8B 41 ? 48 8B ? 85 C0 0F ? ? ? ? ? 80 ? 0E 00", // TDB73+/DD2+
        "48 89 ? ? ? 57 48 83 EC ? 0F ? ? 0E", // RE7 TDB49
        "41 57 41 56 41 55 41 54 56 57 55 53 48 83 EC ? 48 89 CE 8B 41 08 85 C0", // MHWILDS+ (or unoptimized compiler builds?)
    };

    spdlog::info("[REManagedObject] Finding add_ref function...");

    for (auto pattern : possible_patterns) {
        auto address = utility::scan(utility::get_executable(), pattern.data());

        if (address) {
            add_ref_func = (decltype(add_ref_func))*address;
            break;
        }
    }

    spdlog::info("[REManagedObject] Found add_ref function at {:x}", (uintptr_t)add_ref_func);
}

void REManagedObject::resolve_release() {
    if (release_func != nullptr) {
        return;
    }

    // We also need to resolve add_ref
    // because we need to make sure we don't resolve release to the same function.
    resolve_add_ref();

    constexpr std::array<std::string_view, 5> possible_patterns{
        "40 53 48 83 EC ? 8B 41 08 48 8B D9 85 C0 0F", // RE2+
        "40 53 48 83 EC ? 8B 41 08 48 8B D9 48 83 C1 08 85 C0 78", // RE7
        "41 57 41 56 41 55 41 54 56 57 55 53 48 83 EC ? 48 8B 05 ? ? ? ? 48 31 E0 48 89 ? ? ? 8B 41 08 85 C0", // MHWILDS TU4+
        "41 57 41 56 41 55 41 54 56 57 55 53 48 83 EC ? 48 8B 05 ? ? ? ? 48 31 E0 48 89 44 24 30 8B 41 08", // MHWILDS+ (or unoptimized compiler builds?)
        "41 57 41 56 *[3] 41 54 56 57 *[3] 53 48 83 EC ? 48 8B 05 ? ? ? ? 48 31 E0 48 89 44 24 30 8B 41 08", // Seen in Onimusha Wots demo
    };

    spdlog::info("[REManagedObject] Finding release function...");

    for (auto pattern : possible_patterns) {
        auto address = utility::scan(utility::get_executable(), pattern.data());

        if (address && *address != (uintptr_t)add_ref_func) {
            release_func = (decltype(release_func))*address;
            break;
        } else if (address) {
            spdlog::info("[REManagedObject] Found add_ref function while looking for release function, trying again...");

            const auto start = *address + 1;
            const auto module_size = *utility::get_module_size(utility::get_executable());
            const auto end = (uintptr_t)utility::get_executable() + module_size;
            const auto size = end - start;
            address = utility::scan(start, size - 0x1000, pattern.data());

            if (address && *address != (uintptr_t)add_ref_func) {
                release_func = (decltype(release_func))*address;
                break;
            }
        }
    }

    spdlog::info("[REManagedObject] Found release function at {:x}", (uintptr_t)release_func);
}

void REManagedObject::add_ref() {
    resolve_add_ref();

#if TDB_VER <= 49
    if ((int32_t)get_ref_count() >= 0) {
        _InterlockedIncrement((volatile long*)ref_count_ptr());
    } else {
        add_ref_func(this);
    }
#else
    add_ref_func(this);
#endif
}

void REManagedObject::release() {
    resolve_release();

    if (release_func == nullptr) {
        // The pattern scan failed: nothing can be released, so nothing is queued either.
        static std::atomic<bool> s_logged{false};

        if (!s_logged.exchange(true)) {
            spdlog::error("[REManagedObject] release() called but the release function was not found; references are kept");
        }

        return;
    }

    // Not the last reference (or a local object, whose release the engine ignores): straight to the engine, with no
    // lock and no allocation. The last one is queued (see DeferredReleases).
    if (*(volatile int32_t*)ref_count_ptr() != 1) {
        release_func(this);
        return;
    }

    deferred_releases().push(this);
}

int32_t REManagedObject::get_visible_ref_count() const {
    return deferred_releases().visible_count(this, (const volatile int32_t*)&m_ref_count);
}

void REManagedObject::drain_deferred_releases() {
    if (release_func == nullptr) {
        return; // release() queues only once the release function is found, so nothing is queued
    }

    deferred_releases().drain([](::REManagedObject* obj) { release_func(obj); });
}

void REManagedObject::note_frame_end() {
    deferred_releases().note_frame_end();
}

std::vector<::REManagedObject*> REManagedObject::deserialize(const uint8_t* data, size_t size, bool add_references) {
    static void (*deserialize_func)(void* placeholder, const sdk::NativeArray<::REManagedObject*>&, const uint8_t*, size_t) = []() -> decltype(deserialize_func) {
        spdlog::info("[REManagedObject] Finding deserialize function...");
        decltype(deserialize_func) result{nullptr};
        uintptr_t first{};

        constexpr std::array<std::string_view, 2> possible_patterns{
            "41 81 ? 52 53 5A 00", // Confirmed RE8+
            "41 81 7D 00 52 53 5A 00" // RE2 (TDB66) -> DMC5
        };

        for (auto pattern : possible_patterns) {
            auto address = utility::scan(utility::get_executable(), pattern.data());

            if (address) {
                first = *address;
                break;
            }
        }

        if (first != 0) {
            spdlog::info("[REManagedObject] Found first step at {:x}", first);

            auto second = utility::scan_reverse(first, 0x100, "48 8B C4");

            if (!second) {
                spdlog::error("[REManagedObject] Failed to find second step");
                return nullptr;
            }

            result = (decltype(result))*second;
        } else {
            spdlog::error("[REManagedObject] Failed to find first step");
            return nullptr;
        }

        spdlog::info("[REManagedObject] Found deserialize function at {:x}", (uintptr_t)result);

        return result;
    }();

    // Array gets resized in the function.
    sdk::NativeArray<::REManagedObject*> arr;
    deserialize_func(nullptr, arr, data, size);

    std::vector<::REManagedObject*> result{};
    
    for (auto object : arr) {
        if (object != nullptr) {
            if (add_references) {
                object->add_ref();
            }

            result.push_back(object);
        }
    }

    return result;
}

void REManagedObject::deserialize_native(const uint8_t* data, size_t size, const std::vector<::REManagedObject*>& objects) {
    if (!is_managed_object(this)) {
        return;
    }

    const auto tdef = get_type_definition();

    if (tdef == nullptr) {
        return;
    }

    const auto t = tdef->get_type();

    if (t == nullptr || get_fields(t) == nullptr || get_fields(t)->get_deserializer() == nullptr) {
        return;
    }

    struct DeserializeStream {
        uint8_t* head{nullptr};
        uint8_t* cur{nullptr};
        uint8_t* tail{nullptr};
        uintptr_t stackptr{0};
        uint8_t* stack[32]{0};
    }; static_assert(sizeof(DeserializeStream) == 0x120, "DeserializeStream is not the correct size");

    const auto deserializer = (void (*)(::REManagedObject*, DeserializeStream*, sdk::NativeArray<REManagedObject*>* objects))get_fields(t)->get_deserializer();

    std::array<uint8_t, 1024 * 8> stack_buffer{};

    DeserializeStream stream{
        .head = (uint8_t*)data,
        .cur = (uint8_t*)data,
        .tail = (uint8_t*)data + size,
        .stackptr = (uintptr_t)stack_buffer.data() // No need to set the "stack" variable, it seems to get filled by the stackptr
    };

    sdk::NativeArray<::REManagedObject*> objects_array;

    if (!objects.empty()) {
        for (auto object : objects) {
            objects_array.push_back(object);
        }
    }

    deserializer(this, &stream, &objects_array);
}

bool REManagedObject::is_managed_object(void* address) {
    if (address == nullptr) {
        return false;
    }

    if (IsBadReadPtr(address, sizeof(void*))) {
        return false;
    }

    auto object = (::REManagedObject*)address;

    if (object->info == nullptr || IsBadReadPtr(object->info, sizeof(void*))) {
        return false;
    }

    auto class_info = object->info->get_class_info();

    if (class_info == nullptr || IsBadReadPtr(class_info, sizeof(void*))) {
        return false;
    }

    const auto& gi = sdk::GameIdentity::get();
    if (gi.tdb_ver() >= 71) {
        const auto td = (sdk::RETypeDefinition*)class_info;

        if ((uintptr_t)TDEF_FIELD(td, managed_vt) != (uintptr_t)object->info) {
            // This allows for cases when a vtable hook is being used to replace this pointer.
            if (IsBadReadPtr(TDEF_FIELD(td, managed_vt), sizeof(void*)) || *(sdk::RETypeDefinition**)TDEF_FIELD(td, managed_vt) != td) {
                return false;
            }
        }

        if (TDEF_FIELD(td, type) == nullptr) {
            return false;
        }

        if (IsBadReadPtr(TDEF_FIELD(td, type), REType::runtime_size()) || TDEF_FIELD(td, type)->get_type_name() == nullptr) {
            return false;
        }

        if (IsBadReadPtr(TDEF_FIELD(td, type)->get_type_name(), sizeof(void*))) {
            return false;
        }
    } else {
        auto td = (sdk::RETypeDefinition*)class_info;
        auto ci_parentInfo = td->get_managed_vt();
        auto ci_type = td->get_type();
        if (ci_parentInfo != object->info) {
            // This allows for cases when a vtable hook is being used to replace this pointer.
            if (IsBadReadPtr(ci_parentInfo, sizeof(void*)) || ci_parentInfo->get_class_info() != class_info) {
                return false;
            }
        }

        if (ci_type == nullptr) {
            return false;
        }

        if (IsBadReadPtr(ci_type, REType::runtime_size()) || ci_type->get_type_name() == nullptr) {
            return false;
        }

        if (IsBadReadPtr(ci_type->get_type_name(), sizeof(void*))) {
            return false;
        }
    }

    return true;
}

REType* REManagedObject::get_type() const {
    auto info_ptr = info;

    if (info_ptr == nullptr) {
        return nullptr;
    }

    const auto& gi = sdk::GameIdentity::get();
    if (gi.tdb_ver() >= 71) {
        const auto td = get_type_definition();

        if (td == nullptr) {
            return nullptr;
        }

        return TDEF_FIELD(td, type);
    } else {
        auto class_info = info_ptr->get_class_info();

        if (class_info == nullptr) {
            return nullptr;
        }

        return ((sdk::RETypeDefinition*)class_info)->get_type();
    }
}

sdk::RETypeDefinition* REManagedObject::get_type_definition() const {
    auto info_ptr = info;

    if (info_ptr == nullptr) {
        return nullptr;
    }

    return (sdk::RETypeDefinition*)info_ptr->get_class_info();
}

REType* REManagedObject::safe_get_type() const {
    return is_managed_object(const_cast<REManagedObject*>(this)) ? get_type() : nullptr;
}

std::string REManagedObject::get_type_name() const {
    auto t = get_type();

    if (t == nullptr) {
        return "";
    }

    return t->get_type_name();
}

bool REManagedObject::is_a(std::string_view name) const {
    for (auto t = get_type(); t != nullptr && t->get_type_name() != nullptr; t = get_super(t)) {
        if (name == t->get_type_name()) {
            return true;
        }
    }

    return false;
}

bool REManagedObject::is_a(REType* cmp) const {
    for (auto t = get_type(); t != nullptr && t->get_type_name() != nullptr; t = get_super(t)) {
        if (cmp == t) {
            return true;
        }
    }

    return false;
}

via::clr::VMObjType REManagedObject::get_vm_type() const {
    auto info_ptr = info;

    if (info_ptr == nullptr || info_ptr->get_class_info() == nullptr) {
        return via::clr::VMObjType::NULL_;
    }

    return get_type_definition()->get_vm_obj_type();
}

uint32_t REManagedObject::get_size() const {
    auto info_ptr = info;

    if (info_ptr == nullptr || info_ptr->get_class_info() == nullptr) {
        return 0;
    }

    const auto td = get_type_definition();
    auto size = td->get_size();

    switch (get_vm_type()) {
    case via::clr::VMObjType::Array:
    {
        auto container = (::REArrayBase*)this;

        // array of ptrs by default
        auto element_size = utility::re_array::get_element_size(container);

        if (container->num1 <= 1) {
            size = element_size * container->numElements + sizeof(::REArrayBase);
        }
        else {
            size = element_size * container->numElements + sizeof(::REArrayBase) + 4 * container->num1;
        }

        break;
    }
    case via::clr::VMObjType::String:
        size = sizeof(uint16_t) * (Address(const_cast<REManagedObject*>(this)).get(REManagedObject::runtime_size()).to<uint32_t>() + 1) + 0x14;
        break;
    case via::clr::VMObjType::Delegate:
        size = 0x10 * (Address(const_cast<REManagedObject*>(this)).get(REManagedObject::runtime_size()).to<uint32_t>() - 1) + 0x28;
        break;
    case via::clr::VMObjType::Object:
    default:
        break;
    }
    
    return size;
}

REVariableList* REManagedObject::get_variables(::REType* t) {
    return utility::re_type::get_variables(t);
}

REVariableList* REManagedObject::get_variables() const {
    return get_variables(get_type());
}

VariableDescriptor* REManagedObject::get_field_desc(std::string_view field) const {
    return utility::re_type::get_field_desc(get_type(), field);
}

FunctionDescriptor* REManagedObject::get_method_desc(std::string_view name) const {
    return utility::re_type::get_method_desc(get_type(), name);
}
