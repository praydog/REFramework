#include <spdlog/spdlog.h>

#include "utility/Scan.hpp"
#include "utility/Module.hpp"

#include "REFramework.hpp"
#include "REContext.hpp"
#include "RETypeDB.hpp"
#include "RETypes.hpp"

std::string game_namespace(std::string_view base_name)
{
#ifdef MHRISE
    return std::string{ "snow." } + base_name.data();
#elif defined(RE8) || defined(RE7) || defined(DMC5)
    return std::string{ "app." } + base_name.data();
#elif RE3
    return std::string{ "offline." } + base_name.data();
#else
    return std::string{ "app.ropeway." } + base_name.data();
#endif
}

RETypes::RETypes() {
    spdlog::info("RETypes initialization");

    // RE2, RE3, RE8, DMC5
    auto pat = "48 8d 0d ? ? ? ? e8 ? ? ? ? 48 8d 05 ? ? ? ? 48 89 03";
    const auto mod = g_framework->get_module().as<HMODULE>();

    auto types_offset = 3;
    auto ref = utility::scan(mod, pat);

    bool re7_version = false;

    if (!ref) {
        // Scan for RE7 version
        // mov edx, 8F7E7AEh (TypeInfoNone hash)
        pat = "BA AE E7 F7 08";

        const auto typeinfo_none_ref = utility::scan(mod, pat);

        if (!typeinfo_none_ref) {
            spdlog::error("Failed to find TypeInfoNone");
            return;
        }

        const auto add_type_ref = utility::scan(*typeinfo_none_ref, 0x100, "48 8B CB E8 ? ? ? ?");

        if (!add_type_ref) {
            spdlog::error("Failed to find add_type_ref");
            return;
        }

        const auto add_type_fn = utility::calculate_absolute(*add_type_ref + 4);

        if (!add_type_fn) {
            spdlog::error("Failed to calculate add_type_fn");
            return;
        }

        ref = utility::scan(add_type_fn, 0x200, "4C 8B 05 ? ? ? ?");

        if (!ref) {
            spdlog::error("Bad RETypes ref");
            return;
        }

        types_offset = 3;
        re7_version = true;
    }

    spdlog::info("Initial ref: {:x}", (uintptr_t)*ref);
    
    m_raw_types = (TypeList*)(utility::calculate_absolute(*ref + types_offset));
    spdlog::info("Initial TypeList: {:x}", (uintptr_t)m_raw_types);

    const auto start = g_framework->get_module();
    const auto module_size = utility::get_module_size(mod);
    const auto module_end = g_framework->get_module() + *module_size;

    if (!re7_version) {
        bool found_something = false;

        // keeps track of how many references there are to each potential type list
        std::unordered_map<uintptr_t, uint32_t> references{};

        // Scan multiple times to find all references to TypeList
        // If more than 20 references are found for a single address, it's the right one
        for (auto i = utility::scan(start, module_end - start, pat); i.has_value(); i = utility::scan(*i + 1, module_end - (*i + 1), pat)) {
            auto potential_types_ptr = utility::calculate_absolute(*i + 3);

            // Log the potential type if it's not already in the map
            if (!references.contains(potential_types_ptr)) {
                spdlog::info("Potential ref: {:x}", (uintptr_t)*i);
                spdlog::info("Potential TypeList: {:x}", (uintptr_t)potential_types_ptr);
            }

            references[potential_types_ptr]++;

            // this is for sure the right one
            if (references[potential_types_ptr] > 20) {
                ref = *i;
                m_raw_types = (TypeList*)potential_types_ptr;
                found_something = true;
                break;
            }
        }

        if (!found_something) {
            spdlog::error("Couldn't find TypeList");
            return;
        }

        spdlog::info("Settled on ref: {:x}", *ref);
        spdlog::info("Settled on TypeList: {:x}", (uintptr_t)m_raw_types);
    }

    auto& typeList = *m_raw_types;

    // I don't know why but it can extend past the size.
    for (auto i = 0; i < typeList.numAllocated; ++i) {
        auto t = (*typeList.data)[i];

        if (t == nullptr || IsBadReadPtr(t, sizeof(REType)) || ((uintptr_t)t & (sizeof(void*) - 1)) != 0) {
            continue;
        }
        
        if (t->name == nullptr) {
            continue;
        }

        auto name = std::string{ t->name };

        if (name.empty()) {
            continue;
        }

        spdlog::info("{:s}", name);

        m_type_map[name] = t;
        m_types.insert(t);
        m_type_list.push_back(t);
    }

    spdlog::info("Finished RETypes initialization");
}

sdk::RETypeDB* RETypes::get_type_db() const {
    auto c = sdk::VM::get();

    if (c == nullptr) {
        return nullptr;
    }

    return c->get_type_db();
}

REType* RETypes::get(std::string_view name) {
    auto getObj = [&]() -> REType* {
        std::shared_lock _{ m_map_mutex };

        if (auto it = m_type_map.find(name.data()); it != m_type_map.end()) {
            return it->second;
        }

        return nullptr;
    };

    auto obj = getObj();

    // try to refresh the map if the object doesnt exist.
    // assume the user knows this object exists.
    if (obj == nullptr) {
        refresh_map();
    }

    // try again after refreshing the map
    return obj == nullptr ? getObj() : obj;
}

REType* RETypes::operator[](std::string_view name) {
    return get(name);
}

void RETypes::safe_refresh() {
    std::unique_lock _{ m_map_mutex };
    refresh_map();
}

void RETypes::refresh_map() {
    auto& typeList = *m_raw_types;

    // I don't know why but it can extend past the size.
    for (auto i = 0; i < typeList.numAllocated; ++i) {
        auto t = (*typeList.data)[i];

        if (t == nullptr || IsBadReadPtr(t, sizeof(REType)) || ((uintptr_t)t & (sizeof(void*) - 1)) != 0) {
            continue;
        }

        if (t->name == nullptr) {
            continue;
        }

        auto name = std::string{ t->name };

        if (name.empty()) {
            continue;
        }

        spdlog::info("{:s}", name);

        m_type_map[name] = t;

        if (m_types.count(t) == 0) {
            m_types.insert(t);
            m_type_list.push_back(t);
        }
    }
}
