#pragma once

#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <memory>

#include "ReClass.hpp"

// A list of globals in the RE engine (singletons?)
class REGlobals {
public:
    REGlobals();
    virtual ~REGlobals() {};

    const auto& get_objects_set() const {
        return m_objects;
    }

    std::unordered_set<REManagedObject*> get_objects();

    REType* get_native(std::string_view name);
    std::vector<::REType*>& get_native_singleton_types();

    // Equivalent
    REManagedObject* get(std::string_view name);
    REManagedObject* operator[](std::string_view name);

    template <typename T>
    T* get(std::string_view name) {
        return (T*)get(name);
    }

    // Lock a mutex and then refresh the map.
    void safe_refresh();
    void safe_refresh_native();

private:
    void refresh_natives();
    void refresh_map();

    // Class name to object like "app.foo.bar" -> 0xDEADBEEF
    std::unordered_map<std::string, REManagedObject**> m_object_map;

    // Raw list of objects (for if the type hasn't been fully initialized, we need to refresh the map)
    std::unordered_set<REManagedObject**> m_objects;
    std::unordered_set<REManagedObject**> m_object_list;
    std::unordered_map<std::string, std::function<REManagedObject* ()>> m_getters;

    // List of objects we've already logged
    std::unordered_set<REManagedObject**> m_acknowledged_objects;

    std::vector<::REType*> m_native_singleton_types{};
    std::unordered_map<std::string, ::REType*> m_native_singleton_map{};

    std::mutex m_map_mutex{};
};

namespace reframework {
std::unique_ptr<REGlobals>& get_globals();
}