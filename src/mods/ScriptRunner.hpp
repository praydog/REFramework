#pragma once

#include <deque>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <deque>
#include <shared_mutex>

#include <Windows.h>

#include <sol/sol.hpp>
#include <asmjit/asmjit.h>

#include "sdk/RETypeDB.hpp"
#include "utility/FunctionHook.hpp"

#include "Mod.hpp"

#include "reframework/API.hpp"

#include "HookManager.hpp"

namespace regenny {
namespace via {
namespace clr {
struct ManagedObject;
}
}    
}

namespace sdk {
namespace behaviortree {
class TreeNode;
class TreeNodeData;
class TreeObject;
class TreeObjectData;
}
}

namespace sdk {
template<typename T>
struct NativeArray;

template<typename T>
struct NativeArrayNoCapacity;
}

namespace sdk {
class REMethodDefinition;
class RETypeDefinition;
class REField;

struct Delegate;
struct DelegateInvocation;
}

class REManagedObject;
class RETransform;

namespace detail {
template<typename T>
concept ManagedObjectBased = std::is_base_of_v<::REManagedObject, T> || std::is_base_of_v<regenny::via::clr::ManagedObject, T>;

template<typename T>
concept CachedUserType = std::is_base_of_v<sdk::RETypeDefinition, T> 
                         || std::is_base_of_v<sdk::REMethodDefinition, T>
                         || std::is_base_of_v<sdk::REField, T>
                         || std::is_base_of_v<::sdk::behaviortree::TreeNode, T>
                         || std::is_base_of_v<::sdk::behaviortree::TreeNodeData, T>
                         || std::is_base_of_v<::sdk::behaviortree::TreeObject, T>
                         || std::is_base_of_v<::sdk::behaviortree::TreeObjectData, T>
                         || std::is_base_of_v<::sdk::NativeArrayNoCapacity<uint8_t>, T>
                         || std::is_base_of_v<::sdk::NativeArrayNoCapacity<uint16_t>, T>
                         || std::is_base_of_v<::sdk::NativeArrayNoCapacity<uint32_t>, T>
                         || std::is_base_of_v<::sdk::NativeArrayNoCapacity<uint64_t>, T>
                         || std::is_base_of_v<::sdk::NativeArrayNoCapacity<int8_t>, T>
                         || std::is_base_of_v<::sdk::NativeArrayNoCapacity<int16_t>, T>
                         || std::is_base_of_v<::sdk::NativeArrayNoCapacity<int32_t>, T>
                         || std::is_base_of_v<::sdk::NativeArrayNoCapacity<int64_t>, T>
                         || std::is_base_of_v<::sdk::NativeArray<uint8_t>, T>
                            || std::is_base_of_v<::sdk::NativeArray<uint16_t>, T>
                            || std::is_base_of_v<::sdk::NativeArray<uint32_t>, T>
                            || std::is_base_of_v<::sdk::NativeArray<uint64_t>, T>
                            || std::is_base_of_v<::sdk::NativeArray<int8_t>, T>
                            || std::is_base_of_v<::sdk::NativeArray<int16_t>, T>
                            || std::is_base_of_v<::sdk::NativeArray<int32_t>, T>
                            || std::is_base_of_v<::sdk::NativeArray<int64_t>, T>
                            || std::is_base_of_v<::sdk::NativeArray<::REManagedObject*>, T>
                            || std::is_base_of_v<::sdk::NativeArrayNoCapacity<::REManagedObject*>, T>
                            || std::is_base_of_v<::sdk::NativeArrayNoCapacity<::sdk::behaviortree::TreeNode>, T>
                            || std::is_base_of_v<::sdk::NativeArrayNoCapacity<::sdk::behaviortree::TreeNodeData>, T>;
}

template<detail::ManagedObjectBased T>
int sol_lua_push(sol::types<T*>, lua_State* l, T* obj);

template<detail::CachedUserType T>
int sol_lua_push(sol::types<T*>, lua_State* l, T* obj);

class ScriptState : public std::enable_shared_from_this<ScriptState> {
public:
    enum class GarbageCollectionHandler : uint32_t {
        REFRAMEWORK_MANAGED = 0,
        LUA_MANAGED = 1,
        LAST
    };

    enum class GarbageCollectionType : uint32_t {
        STEP = 0,
        FULL = 1,
        LAST
    };

    enum class GarbageCollectionMode : uint32_t {
        GENERATIONAL = 0,
        INCREMENTAL = 1,
        LAST
    };

    struct GarbageCollectionData {
        GarbageCollectionHandler gc_handler{GarbageCollectionHandler::REFRAMEWORK_MANAGED};
        GarbageCollectionType gc_type{GarbageCollectionType::FULL};
        GarbageCollectionMode gc_mode{GarbageCollectionMode::GENERATIONAL};
        std::chrono::microseconds gc_budget{1000};

        uint32_t gc_minor_multiplier{1};
        uint32_t gc_major_multiplier{100};
    };

    ScriptState(const GarbageCollectionData& gc_data,bool is_main_state);
    ~ScriptState();

    void run_script(const std::string& p);
    sol::protected_function_result handle_protected_result(sol::protected_function_result result); // because protected_functions don't throw

    void on_frame();
    void on_draw_ui();
    void on_pre_application_entry(size_t hash);
    void on_application_entry(size_t hash);
    bool on_pre_gui_draw_element(REComponent* gui_element, void* primitive_context);
    void on_gui_draw_element(REComponent* gui_element, void* primitive_context);
    void on_script_reset();
    void on_config_save();
    bool is_main_state() { return m_is_main_state; }
    auto& lua() { return m_lua; }
    void lock() { m_execution_mutex.lock(); }
    void unlock() { m_execution_mutex.unlock(); }
    auto scoped_lock() { return std::scoped_lock{m_execution_mutex}; }

    // add_hook enqueues the hook definition to be installed the next time install_hooks is called.
    void add_hook(sdk::REMethodDefinition* fn, sol::protected_function pre_cb, sol::protected_function post_cb, sol::object ignore_jmp_obj);
    void add_vtable(::REManagedObject* obj, sdk::REMethodDefinition* fn, sol::protected_function pre_cb, sol::protected_function post_cb);

    // install_hooks goes through the queue of added hooks and actually creates them. The queue is emptied as a result.
    void install_hooks();

    void gc_data_changed(GarbageCollectionData data);

    void push_hook_storage(size_t thread_hash) {
        auto it = m_hook_storage.find(thread_hash);
        if (it == m_hook_storage.end()) {
            it = m_hook_storage.emplace(thread_hash, std::list<TablePool::TableGuard>{}).first;
        }

        it->second.push_back(m_table_pool.acquire(m_lua));
        m_current_hook_storage = it->second.back();
    }

    void pop_hook_storage(size_t thread_hash) {
        auto it = m_hook_storage.find(thread_hash);
        if (it != m_hook_storage.end()) {
            if (!it->second.empty()) {
                it->second.pop_back();
            }
        }

        m_current_hook_storage = sol::nil;
    }

    sol::reference get_hook_storage() {
        return m_current_hook_storage;
    }
    
    class TablePool {
    public:
        class TableGuard {
        public:
            TableGuard(TablePool& pool, sol::table& table) 
                : m_pool{pool},
                m_table{table} 
            {
            
            }

            TableGuard(TableGuard&& other) noexcept
                : m_pool{other.m_pool},
                m_table{other.m_table}
            {
                other.m_table = sol::nil;
                other.m_moved = true;
            }
            
            TableGuard& operator=(TableGuard&& other) noexcept {
                m_pool = other.m_pool;
                m_table = other.m_table;
                other.m_table = sol::nil;
                other.m_moved = true;
                return *this;
            }

            TableGuard(const TableGuard&) = delete;
            TableGuard& operator=(const TableGuard&) = delete;

            ~TableGuard() {
                if (m_moved) {
                    return;
                }

                m_pool.m_tables.push_back(m_table);
            }
            
            operator sol::table&() { return m_table; }
            sol::table get() { return m_table; }

        private:
            TablePool& m_pool;
            sol::table m_table;
            bool m_moved{false};
        };

        TableGuard acquire(sol::state_view lua) {
            if (m_tables.empty()) {
                m_tables.push_back(lua.create_table());
            }

            auto table = m_tables.front();
            m_tables.pop_front();

            // Clear for re-use
            table.clear();

            return TableGuard{*this, table};
        }

    private:
        std::list<sol::table> m_tables{};
    };

    TablePool& get_table_pool() {
        return m_table_pool;
    }

    void add_delegate_callback(sdk::DelegateInvocation& invo, sol::protected_function callback);

private:
    sol::reference get_hook_storage_internal(size_t thread_hash) {
        //return m_current_hook_storage;

        auto it = m_hook_storage.find(thread_hash);
        if (it != m_hook_storage.end()) {
            if (!it->second.empty()) {
                return it->second.back();
            }
        }

        return sol::nil;
    }

    sol::state m_lua{};
    TablePool m_table_pool{};

    GarbageCollectionData m_gc_data{};
    bool m_is_main_state;
    std::recursive_mutex m_execution_mutex{};

    // FNV-1A
    std::unordered_multimap<size_t, sol::protected_function> m_pre_application_entry_fns{};
    std::unordered_multimap<size_t, sol::protected_function> m_application_entry_fns{};

    std::vector<sol::protected_function> m_pre_gui_draw_element_fns{};
    std::vector<sol::protected_function> m_gui_draw_element_fns{};
    std::vector<sol::protected_function> m_on_draw_ui_fns{};
    std::vector<sol::protected_function> m_on_frame_fns{};
    std::vector<sol::protected_function> m_on_script_reset_fns{};
    std::vector<sol::protected_function> m_on_config_save_fns{};

    struct HookDef {
        ::REManagedObject* obj{nullptr};
        sdk::REMethodDefinition* fn;
        sol::protected_function pre_cb;
        sol::protected_function post_cb;
        sol::object ignore_jmp_obj;
    };

    std::deque<HookDef> m_hooks_to_add{};
    std::unordered_map<sdk::REMethodDefinition*, std::vector<HookManager::HookId>> m_hooks{};

    // Using std::list rather than deque because the elements need to remain valid even if the list is resized.
    // Using sol::reference instead of sol::table to keep a guaranteed reference to the table.
    std::unordered_map<size_t, std::list<TablePool::TableGuard>> m_hook_storage{};
    sol::reference m_current_hook_storage{};

    struct DelegateStorage {
        std::weak_ptr<ScriptState> owner{}; // Weak pointer to the ScriptState that owns this delegate storage because the ScriptState may be deleted. 
        std::vector<sol::protected_function> callbacks{};

    };

    static inline std::unordered_map<REManagedObject*, std::unique_ptr<DelegateStorage>> s_delegates{};
    static inline std::recursive_mutex s_delegates_mutex{};

    static void delegate_callback(sdk::VMContext* ctx, REManagedObject* obj);
};

class ScriptRunner : public Mod {
public:
    static std::shared_ptr<ScriptRunner>& get();

    std::string_view get_name() const override { return "ScriptRunner"; }
    std::optional<std::string> on_initialize() override;
    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    void hook_battle_rule();
    void set_last_battle_type(uint8_t t) {
        m_last_battle_type = t;
    }
    auto get_last_battle_type() {
        return m_last_battle_type;
    }
    void set_last_online_match_state() {
        m_last_online_match_state = true;
    }
    bool is_online_match() {
        return m_last_online_match_state;
    }
    void on_frame() override;
    void on_draw_ui() override;
    void on_pre_application_entry(void* entry, const char* name, size_t hash) override;
    void on_application_entry(void* entry, const char* name, size_t hash) override;
    bool on_pre_gui_draw_element(REComponent* gui_element, void* primitive_context) override;
    void on_gui_draw_element(REComponent* gui_element, void* primitive_context) override;

    void spew_error(const std::string& p);

    const auto& get_state() {
        return m_main_state;
    }
    //not sure how to approach this, should there be error checking here?
    const auto& get_state(int index) { 
        return m_states[index];
    }

    void lock() {
        m_access_mutex.lock();
        for (auto& state : m_states) {
            state->lock();
        }

        ++m_lock_depth;
    }

    void unlock() {
        for (auto& state : m_states) {
            state->unlock();
        }
        m_access_mutex.unlock();

        if (m_lock_depth > 0) {
            --m_lock_depth;
        }
    }

    lua_State* create_state() {
        std::scoped_lock _{m_access_mutex};
        m_states.emplace_back(std::make_shared<ScriptState>(make_gc_data(), false));

        for (uint32_t i = 0; i < m_lock_depth; ++i) {
            m_states.back()->lock();
        }

        return m_states.back()->lua().lua_state();
    }

    void delete_state(lua_State* lua_state) {
        std::scoped_lock _{m_access_mutex};
        m_states_to_delete.push_back(lua_state);
    }

private:
    ScriptState::GarbageCollectionData make_gc_data() const {
        ScriptState::GarbageCollectionData data{};

        data.gc_handler = (decltype(ScriptState::GarbageCollectionData::gc_handler))m_gc_handler->value();
        data.gc_type = (decltype(ScriptState::GarbageCollectionData::gc_type))m_gc_type->value();
        data.gc_mode = (decltype(ScriptState::GarbageCollectionData::gc_mode))m_gc_mode->value();
        data.gc_budget = std::chrono::microseconds{(uint32_t)m_gc_budget->value()};
        data.gc_minor_multiplier = (uint32_t)m_gc_minor_multiplier->value();
        data.gc_major_multiplier = (uint32_t)m_gc_major_multiplier->value();

        return data;
    }
    std::shared_ptr<ScriptState> m_main_state{};
    std::vector<std::shared_ptr<ScriptState>> m_states{};
    std::recursive_mutex m_access_mutex{};
    std::atomic<uint32_t> m_lock_depth{0};

    // A list of Lua files that have been explicitly loaded either through the user manually loading the script, or
    // because the script was in the autorun directory.
    std::vector<std::string> m_loaded_scripts{};
    std::vector<std::string> m_known_scripts{};
    std::unordered_map<std::string, bool> m_loaded_scripts_map{};
    std::vector<lua_State*> m_states_to_delete{};
    std::string m_last_script_error{};
    std::shared_mutex m_script_error_mutex{};
    std::chrono::system_clock::time_point m_last_script_error_time{};

    std::chrono::system_clock::time_point m_scene_check_time{};
    bool m_checked_scene_once{false};
    bool m_scene_okay{false};
    bool m_console_spawned{false};
    bool m_needs_first_reset{true};
    bool m_last_online_match_state{false};
    bool m_attempted_hook_battle_rule{false};
    std::optional<uint8_t> m_last_battle_type{};
    const ModToggle::Ptr m_log_to_disk{ ModToggle::create(generate_name("LogToDisk"), false) };

    const ModCombo::Ptr m_gc_handler { 
        ModCombo::create(generate_name("GarbageCollectionHandlerV2"),
        {
            "Managed by REFramework",
            "Managed by Lua"
        }, (int)ScriptState::GarbageCollectionHandler::REFRAMEWORK_MANAGED)
    };

    const ModCombo::Ptr m_gc_type {
        ModCombo::create(generate_name("GarbageCollectionTypeV2"),
        {
            "Step",
            "Full",
        }, (int)ScriptState::GarbageCollectionType::STEP)
    };

    const ModCombo::Ptr m_gc_mode {
        ModCombo::create(generate_name("GarbageCollectionMode"),
        {
            "Generational",
            "Incremental (Mark & Sweep)",
        }, (int)ScriptState::GarbageCollectionMode::GENERATIONAL)
    };

    // Garbage collection budget in microseconds.
    const ModSlider::Ptr m_gc_budget {
        ModSlider::create(generate_name("GarbageCollectionBudget"), 0.0f, 2000.0f, 1000.0f)
    };

    const ModSlider::Ptr m_gc_minor_multiplier {
        ModSlider::create(generate_name("GarbageCollectionMinorMultiplier"), 1.0f, 200.0f, 1.0f)
    };

    const ModSlider::Ptr m_gc_major_multiplier {
        ModSlider::create(generate_name("GarbageCollectionMajorMultiplier"), 1.0f, 1000.0f, 100.0f)
    };

    ValueList m_options{
        *m_log_to_disk,
        *m_gc_handler,
        *m_gc_type,
        *m_gc_mode,
        *m_gc_budget,
        *m_gc_minor_multiplier,
        *m_gc_major_multiplier
    };

    // Resets the ScriptState and runs autorun scripts again.
    void reset_scripts();
};

