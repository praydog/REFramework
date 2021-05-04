#include <spdlog/spdlog.h>

#include "utility/Scan.hpp"
#include "utility/Module.hpp"

#include "REFramework.hpp"
#include "REGlobals.hpp"

REGlobals::REGlobals() {
    spdlog::info("REGlobals initialization");

    auto mod = g_framework->get_module().as<HMODULE>();
    auto start = (uintptr_t)mod;
    auto end = (uintptr_t)start + *utility::get_module_size(mod);

    // generic pattern used for all these globals
    auto pat = std::string{ "48 8D ? ? ? ? ? 48 B8 00 00 00 00 00 00 00 80" };

    // find all the globals
    for (auto i = utility::scan(start, end - start, pat); i.has_value(); i = utility::scan(*i + 1, end - (*i + 1), pat)) {
        auto ptr = utility::calculate_absolute(*i + 3);

        // Make sure the pointer is aligned on an 8-byte boundary.
        if (ptr == 0 || ((uintptr_t)ptr & (sizeof(void*) - 1)) != 0) {
            continue;
        }

        if (IsBadReadPtr((void*)ptr, sizeof(void*))) {
            continue;
        }

        auto obj_ptr = (REManagedObject**)ptr;

        if (m_objects.find(obj_ptr) != m_objects.end()) {
            continue;
        }
        
        m_objects.insert(obj_ptr);
        m_object_list.push_back(obj_ptr);
    }

    spdlog::info("Finished REGlobals initialization");
}

REManagedObject* REGlobals::get(std::string_view name) {
    std::lock_guard _{ m_map_mutex };

    auto get_obj = [&]() -> REManagedObject* {
        if (auto it = m_object_map.find(name.data()); it != m_object_map.end()) {
            return *it->second;
        }

        return nullptr;
    };

    auto obj = get_obj();

    // try to refresh the map if the object doesnt exist.
    // assume the user knows this object exists.
    if (obj == nullptr) {
        refresh_map();
    }

    // try again after refreshing the map
    return obj == nullptr ? get_obj() : obj;
}

REManagedObject* REGlobals::operator[](std::string_view name) {
    return get(name);
}

void REGlobals::safe_refresh() {
    std::lock_guard _{ m_map_mutex };
    refresh_map();
}

void REGlobals::refresh_map() {
    for (auto obj_ptr : m_objects) {
        auto obj = *obj_ptr;

        // Make sure the pointer is aligned on an 8-byte boundary.
        if (obj == nullptr || ((uintptr_t)obj & (sizeof(void*) - 1)) != 0) {
            continue;
        }

        if (IsBadReadPtr(obj, sizeof(REManagedObject))) {
            continue;
        }

        auto t = utility::re_managed_object::safe_get_type(obj);

        if (t == nullptr || t->name == nullptr) {
            continue;
        }

        if (m_acknowledged_objects.find(obj_ptr) == m_acknowledged_objects.end()) {
#ifdef DEVELOPER
            spdlog::info("{:x}->{:x} ({:s})", (uintptr_t)obj_ptr, (uintptr_t)*obj_ptr, t->name);
#endif
            m_acknowledged_objects.insert(obj_ptr);
        }

        m_object_map[t->name] = obj_ptr;
    }
}
