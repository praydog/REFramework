#include <spdlog/spdlog.h>

#include "utility/Scan.hpp"
#include "utility/Module.hpp"

#include "REFramework.hpp"
#include "REGlobals.hpp"

REGlobals::REGlobals() {
    spdlog::info("REGlobals initialization");

    auto mod = g_framework->getModule().as<HMODULE>();
    auto start = (uintptr_t)mod;
    auto end = (uintptr_t)start + *utility::getModuleSize(mod);

    // generic pattern used for all these globals
    auto pat = std::string{ "48 8D 0D ? ? ? ? 48 B8 00 00 00 00 00 00 00 80" };

    // find all the globals
    for (auto i = utility::scan(start, end - start, pat); i.has_value(); i = utility::scan(*i + 1, end - (*i + 1), pat)) {
        auto ptr = utility::calculateAbsolute(*i + 3);

        // Make sure the pointer is aligned on an 8-byte boundary.
        if (ptr == 0 || ((uintptr_t)ptr & (sizeof(void*) - 1)) != 0) {
            continue;
        }

        if (IsBadReadPtr((void*)ptr, sizeof(void*))) {
            continue;
        }

        auto objPtr = (REManagedObject**)ptr;

        if (m_objects.find(objPtr) != m_objects.end()) {
            continue;
        }
        
        m_objects.insert(objPtr);
        m_objectList.push_back(objPtr);
    }

    spdlog::info("Finished REGlobals initialization");
}

REManagedObject* REGlobals::get(std::string_view name) {
    std::lock_guard _{ m_mapMutex };

    auto getObj = [&]() -> REManagedObject* {
        if (auto it = m_objectMap.find(name.data()); it != m_objectMap.end()) {
            return *it->second;
        }

        return nullptr;
    };

    auto obj = getObj();

    // try to refresh the map if the object doesnt exist.
    // assume the user knows this object exists.
    if (obj == nullptr) {
        refreshMap();
    }

    // try again after refreshing the map
    return obj == nullptr ? getObj() : obj;
}

REManagedObject* REGlobals::operator[](std::string_view name) {
    return get(name);
}

void REGlobals::safeRefresh() {
    std::lock_guard _{ m_mapMutex };
    refreshMap();
}

void REGlobals::refreshMap() {
    for (auto objPtr : m_objects) {
        auto obj = *objPtr;

        // Make sure the pointer is aligned on an 8-byte boundary.
        if (obj == nullptr || ((uintptr_t)obj & (sizeof(void*) - 1)) != 0) {
            continue;
        }

        if (IsBadReadPtr(obj, sizeof(REManagedObject))) {
            continue;
        }

        auto t = utility::REManagedObject::safeGetType(obj);

        if (t == nullptr || t->name == nullptr) {
            continue;
        }

        if (m_acknowledgedObjects.find(objPtr) == m_acknowledgedObjects.end()) {
#ifdef DEVELOPER
            spdlog::info("{:x}->{:x} ({:s})", (uintptr_t)objPtr, (uintptr_t)*objPtr, t->name);
#endif
            m_acknowledgedObjects.insert(objPtr);
        }

        m_objectMap[t->name] = objPtr;
    }
}
