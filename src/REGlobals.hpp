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

    const auto& getObjectsSet() const {
        return m_objects;
    }

    const auto& getObjects() const {
        return m_objectList;
    }

    // Equivalent
    REManagedObject* get(std::string_view name);
    REManagedObject* operator[](std::string_view name);

    // Lock a mutex and then refresh the map.
    void safeRefresh();

private:
    void refreshMap();

    // Class name to object like "app.foo.bar" -> 0xDEADBEEF
    std::unordered_map<std::string, REManagedObject**> m_objectMap;

    // Raw list of objects (for if the type hasn't been fully initialized, we need to refresh the map)
    std::unordered_set<REManagedObject**> m_objects;
    std::vector<REManagedObject**> m_objectList;
    // List of objects we've already logged
    std::unordered_set<REManagedObject**> m_acknowledgedObjects;

    std::mutex m_mapMutex{};
};
