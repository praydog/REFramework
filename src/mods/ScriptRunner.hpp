#pragma once

#include <deque>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>

#include <Windows.h>

#include <sol/sol.hpp>
#include <asmjit/asmjit.h>

#include "sdk/RETypeDB.hpp"
#include "utility/FunctionHook.hpp"

#include "Mod.hpp"

#include "reframework/API.hpp"

#include "HookManager.hpp"

class REManagedObject;
class RETransform;

namespace detail {
template<typename T>
concept ManagedObjectBased = std::is_base_of_v<::REManagedObject, T>;
}

template<detail::ManagedObjectBased T>
int sol_lua_push(sol::types<T*>, lua_State* l, T* obj);

class ScriptState {
public:
    ScriptState();
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

private:
    sol::state m_lua{};

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

    void on_frame() override;
    void on_draw_ui() override;

    void on_config_save(utility::Config& cfg) override;

    void on_pre_application_entry(void* entry, const char* name, size_t hash) override;
    void on_application_entry(void* entry, const char* name, size_t hash) override;
    void on_pre_update_transform(RETransform* transform) override;
    void on_update_transform(RETransform* transform) override;
    bool on_pre_gui_draw_element(REComponent* gui_element, void* primitive_context) override;
    void on_gui_draw_element(REComponent* gui_element, void* primitive_context) override;

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
    std::unique_ptr<ScriptState> m_state{};
    std::recursive_mutex m_access_mutex{};

    // A list of Lua files that have been explicitly loaded either through the user manually loading the script, or
    // because the script was in the autorun directory.
    std::vector<std::string> m_loaded_scripts{}; 

    // Resets the ScriptState and runs autorun scripts again.
    void reset_scripts();
};

