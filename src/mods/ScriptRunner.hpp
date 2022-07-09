#pragma once

#include <deque>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
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

class REManagedObject;
class RETransform;

namespace detail {
template<typename T>
concept ManagedObjectBased = std::is_base_of_v<::REManagedObject, T> || std::is_base_of_v<regenny::via::clr::ManagedObject, T>;

template<typename T>
concept CachedUserType = std::is_base_of_v<::sdk::behaviortree::TreeNode, T>
                         || std::is_base_of_v<::sdk::behaviortree::TreeNodeData, T>
                         || std::is_base_of_v<::sdk::behaviortree::TreeObject, T>
                         || std::is_base_of_v<::sdk::behaviortree::TreeObjectData, T>;
}

template<detail::ManagedObjectBased T>
int sol_lua_push(sol::types<T*>, lua_State* l, T* obj);

template<detail::CachedUserType T>
int sol_lua_push(sol::types<T*>, lua_State* l, T* obj);

class ScriptState {
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

    ScriptState(const GarbageCollectionData& gc_data);
    ~ScriptState();

    void run_script(const std::string& p);
    sol::protected_function_result handle_protected_result(sol::protected_function_result result); // because protected_functions don't throw

    void on_frame();
    void on_draw_ui();
    void on_pre_application_entry(size_t hash);
    void on_application_entry(size_t hash);
    void on_pre_update_transform(RETransform* transform);
    void on_update_transform(RETransform* transform);
    bool on_pre_gui_draw_element(REComponent* gui_element, void* primitive_context);
    void on_gui_draw_element(REComponent* gui_element, void* primitive_context);
    void on_script_reset();
    void on_config_save();

    auto& lua() { return m_lua; }
    void lock() { m_execution_mutex.lock(); }
    void unlock() { m_execution_mutex.unlock(); }
    auto scoped_lock() { return std::scoped_lock{m_execution_mutex}; }

    // add_hook enqueues the hook definition to be installed the next time install_hooks is called.
    void add_hook(sdk::REMethodDefinition* fn, sol::protected_function pre_cb, sol::protected_function post_cb, sol::object ignore_jmp_obj);

    // install_hooks goes through the queue of added hooks and actually creates them. The queue is emptied as a result.
    void install_hooks();

    void gc_data_changed(GarbageCollectionData data);

private:
    sol::state m_lua{};

    GarbageCollectionData m_gc_data{};

    std::recursive_mutex m_execution_mutex{};

    // FNV-1A
    std::unordered_multimap<size_t, sol::protected_function> m_pre_application_entry_fns{};
    std::unordered_multimap<size_t, sol::protected_function> m_application_entry_fns{};
    std::unordered_multimap<RETransform*, sol::protected_function> m_pre_update_transform_fns{};
    std::unordered_multimap<RETransform*, sol::protected_function> m_update_transform_fns{};

    std::vector<sol::protected_function> m_pre_gui_draw_element_fns{};
    std::vector<sol::protected_function> m_gui_draw_element_fns{};
    std::vector<sol::protected_function> m_on_draw_ui_fns{};
    std::vector<sol::protected_function> m_on_frame_fns{};
    std::vector<sol::protected_function> m_on_script_reset_fns{};
    std::vector<sol::protected_function> m_on_config_save_fns{};

    struct HookDef {
        sdk::REMethodDefinition* fn;
        sol::protected_function pre_cb;
        sol::protected_function post_cb;
        sol::object ignore_jmp_obj;
    };

    std::deque<HookDef> m_hooks_to_add{};
    std::unordered_map<sdk::REMethodDefinition*, std::vector<HookManager::HookId>> m_hooks{};
};

class ScriptRunner : public Mod {
public:
    static std::shared_ptr<ScriptRunner>& get();

    std::string_view get_name() const override { return "ScriptRunner"; }
    std::optional<std::string> on_initialize() override;
    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    void on_frame() override;
    void on_draw_ui() override;

    void on_pre_application_entry(void* entry, const char* name, size_t hash) override;
    void on_application_entry(void* entry, const char* name, size_t hash) override;
    void on_pre_update_transform(RETransform* transform) override;
    void on_update_transform(RETransform* transform) override;
    bool on_pre_gui_draw_element(REComponent* gui_element, void* primitive_context) override;
    void on_gui_draw_element(REComponent* gui_element, void* primitive_context) override;

    void spew_error(const std::string& p);

    const auto& get_state() {
        return m_state;
    }

    void lock() {
        m_access_mutex.lock();

        if (m_state) {
            m_state->lock();
        }
    }

    void unlock() {
        if (m_state) {
            m_state->unlock();
        }

        m_access_mutex.unlock();
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

    std::unique_ptr<ScriptState> m_state{};
    std::recursive_mutex m_access_mutex{};

    // A list of Lua files that have been explicitly loaded either through the user manually loading the script, or
    // because the script was in the autorun directory.
    std::vector<std::string> m_loaded_scripts{}; 

    std::string m_last_script_error{};
    std::shared_mutex m_script_error_mutex{};
    std::chrono::system_clock::time_point m_last_script_error_time{};

    bool m_console_spawned{false};
    const ModToggle::Ptr m_log_to_disk{ ModToggle::create(generate_name("LogToDisk"), false) };

    const ModCombo::Ptr m_gc_handler { 
        ModCombo::create(generate_name("GarbageCollectionHandler"),
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
        *m_gc_budget,
        *m_gc_minor_multiplier,
        *m_gc_major_multiplier
    };

    // Resets the ScriptState and runs autorun scripts again.
    void reset_scripts();
};

