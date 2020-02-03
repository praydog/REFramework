#include <spdlog/spdlog.h>

#include "utility/Scan.hpp"
#include "utility/Module.hpp"

#include "REFramework.hpp"
#include "RETypes.hpp"

RETypes::RETypes() {
    spdlog::info("RETypes initialization");

    auto mod = g_framework->getModule().as<HMODULE>();
    auto ref = utility::scan(mod, "48 8d 0d ? ? ? ? e8 ? ? ? ? 48 8d 05 ? ? ? ? 48 89 03");

    spdlog::info("Ref: {:x}", (uintptr_t)*ref);

    m_rawTypes = (TypeList*)(utility::calculateAbsolute(*ref + 3));
    spdlog::info("TypeList: {:x}", (uintptr_t)m_rawTypes);

    auto& typeList = *m_rawTypes;

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

        m_typeMap[name] = t;
        m_types.insert(t);
        m_typeList.push_back(t);
    }

    spdlog::info("Finished RETypes initialization");
}

REType* RETypes::get(std::string_view name) {
    std::lock_guard _{ m_mapMutex };

    auto getObj = [&]() -> REType* {
        if (auto it = m_typeMap.find(name.data()); it != m_typeMap.end()) {
            return it->second;
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

REType* RETypes::operator[](std::string_view name) {
    return get(name);
}

void RETypes::safeRefresh() {
    std::lock_guard _{ m_mapMutex };
    refreshMap();
}

void RETypes::refreshMap() {
    auto& typeList = *m_rawTypes;

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

        m_typeMap[name] = t;

        if (m_types.count(t) == 0) {
            m_types.insert(t);
            m_typeList.push_back(t);
        }
    }
}
