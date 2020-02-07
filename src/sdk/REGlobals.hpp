#pragma once

#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "ReClass.hpp"

// A list of globals in the RE engine (singletons?)
class REGlobals {
public:
    REGlobals();
    virtual ~REGlobals() {};

    const auto& get_objects_set() const {
        return m_objects;
    }

    const auto& get_objects() const {
        return m_object_list;
    }

    // Equivalent
    REManagedObject* get(std::string_view name);
    REManagedObject* operator[](std::string_view name);

    template <typename T>
    T* get(std::string_view name) {
        return (T*)get(name);
    }

    // Lock a mutex and then refresh the map.
    void safe_refresh();

private:
    void refresh_map();

    // Class name to object like "app.foo.bar" -> 0xDEADBEEF
    std::unordered_map<std::string, REManagedObject**> m_object_map;

    // Raw list of objects (for if the type hasn't been fully initialized, we need to refresh the map)
    std::unordered_set<REManagedObject**> m_objects;
    std::vector<REManagedObject**> m_object_list;
    // List of objects we've already logged
    std::unordered_set<REManagedObject**> m_acknowledged_objects;

    std::mutex m_map_mutex{};
};
