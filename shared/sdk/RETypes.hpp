#pragma once

#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "ReClass.hpp"

std::string& game_namespace(std::string_view base_name);

class REType;

namespace sdk {
struct RETypeDB;
}

// A list of types in the RE engine
class RETypes {
public:
    RETypes();
    virtual ~RETypes() {};

    const auto& get_raw_types() const {
        return m_raw_types;
    }

    const auto& get_types_set() const {
        return m_types;
    }

    const auto& get_types() const {
        return m_type_list;
    }

    sdk::RETypeDB* get_type_db() const;

    // Equivalent
    REType* get(std::string_view name);
    REType* operator[](std::string_view name);

    template <typename T>
    T* get(std::string_view name) {
        return (T*)get(name);
    }

    // Lock a mutex and then refresh the map.
    void safe_refresh();

private:
    void fill_types_from_tdb();
    void refresh_map();

    TypeList* m_raw_types{ nullptr };
#if TDB_VER >= 69
    RETypeImpl** m_raw_type_impls{ nullptr };
#endif

    // Class name to object like "app.foo.bar" -> 0xDEADBEEF
    std::unordered_map<std::string, REType*> m_type_map;

    // Raw list of objects (for if the type hasn't been fully initialized, we need to refresh the map)
    std::unordered_set<REType*> m_types;
    std::vector<REType*> m_type_list;
    // List of objects we've already logged
    std::unordered_set<REType*> m_acknowledged_types;

    std::shared_mutex m_map_mutex{};
};

namespace reframework {
std::unique_ptr<RETypes>& get_types();
}
