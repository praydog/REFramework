#pragma once

#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "ReClass.hpp"

// A list of types in the RE engine
class RETypes {
public:
    RETypes();
    virtual ~RETypes() {};

    const auto& getRawTypes() const {
        return m_rawTypes;
    }

    const auto& getTypesSet() const {
        return m_types;
    }

    const auto& getTypes() const {
        return m_typeList;
    }

    // Equivalent
    REType* get(std::string_view name);
    REType* operator[](std::string_view name);

    template <typename T>
    T* get(std::string_view name) {
        return (T*)get(name);
    }

    // Lock a mutex and then refresh the map.
    void safeRefresh();

private:
    void refreshMap();

    TypeList* m_rawTypes{ nullptr };

    // Class name to object like "app.foo.bar" -> 0xDEADBEEF
    std::unordered_map<std::string, REType*> m_typeMap;

    // Raw list of objects (for if the type hasn't been fully initialized, we need to refresh the map)
    std::unordered_set<REType*> m_types;
    std::vector<REType*> m_typeList;
    // List of objects we've already logged
    std::unordered_set<REType*> m_acknowledgedTypes;

    std::mutex m_mapMutex{};
};
