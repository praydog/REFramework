#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>

#include <Windows.h>

#include <sol/sol.hpp>
#include <xbyak/xbyak.h>

#include "sdk/RETypeDB.hpp"
#include "utility/FunctionHook.hpp"

#include "Mod.hpp"

#include "../include/API.hpp"

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
    enum class PreHookResult : int {
        CALL_ORIGINAL,
        SKIP_ORIGINAL,
    };

    struct HookedFn {
        void* target_fn{};
        std::vector<sol::protected_function> script_pre_fns{};
        std::vector<sol::protected_function> script_post_fns{};
        sol::table script_args{};

        std::unique_ptr<FunctionHook> fn_hook{};
        Xbyak::CodeGenerator facilitator_gen{};
        std::vector<uintptr_t> args{};
        std::vector<sdk::RETypeDefinition*> arg_tys{};
        uintptr_t ret_addr{};
        uintptr_t ret_val{};
        sdk::RETypeDefinition* ret_ty{};
    };

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

    // Returns true when the original function should be called.
    PreHookResult on_pre_hook(HookedFn* fn);
    void on_post_hook(HookedFn* fn);

    __declspec(noinline) static PreHookResult on_pre_hook_static(ScriptState* s, HookedFn* fn) {
        return s->on_pre_hook(fn);
    }

    __declspec(noinline)static void on_post_hook_static(ScriptState* s, HookedFn* fn) {
        s->on_post_hook(fn);
    }

    auto& hooked_fns() {
        return m_hooked_fns;
    }

    __declspec(noinline) static void lock_static(ScriptState* s) { s->m_execution_mutex.lock(); }
    __declspec(noinline) static void unlock_static(ScriptState* s) { s->m_execution_mutex.unlock(); }

    auto& lua() { return m_lua; }

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

    Xbyak::CodeGenerator m_code{};
    //std::vector<std::unique_ptr<HookedFn>> m_hooked_fns{};

    std::unordered_map<sdk::REMethodDefinition*, std::unique_ptr<HookedFn>> m_hooked_fns{};
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

private:
    std::unique_ptr<ScriptState> m_state{};
    std::recursive_mutex m_access_mutex{};

    // Resets the ScriptState and runs autorun scripts again.
    void reset_scripts();
};

