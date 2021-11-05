#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>

#include <Windows.h>

#include <sol/sol.hpp>
#include <xbyak/xbyak.h>

#include "sdk/RETypeDefinition.hpp"
#include "utility/FunctionHook.hpp"

#include "Mod.hpp"

class RETransform;

class ScriptState {
public:
    struct HookedFn {
        void* target_fn{};
        sol::function script_pre_fn{};
        sol::function script_post_fn{};
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

    void run_script(const std::string& p);

    void on_pre_application_entry(const char* name);
    void on_application_entry(const char* name);
    void on_pre_update_transform(RETransform* transform);
    void on_update_transform(RETransform* transform);

    void on_pre_hook(HookedFn* fn);
    void on_post_hook(HookedFn* fn);

    static void on_pre_hook_static(ScriptState* s, HookedFn* fn) {
        s->on_pre_hook(fn);
    }

    static void on_post_hook_static(ScriptState* s, HookedFn* fn) {
        s->on_post_hook(fn);
    }

    auto& hooked_fns() {
        return m_hooked_fns;
    }

private:
    sol::state m_lua{};

    std::recursive_mutex m_execution_mutex{};

    std::unordered_multimap<std::string, sol::function> m_pre_application_entry_fns{};
    std::unordered_multimap<std::string, sol::function> m_application_entry_fns{};
    std::unordered_multimap<RETransform*, sol::function> m_pre_update_transform_fns{};
    std::unordered_multimap<RETransform*, sol::function> m_update_transform_fns{};

    Xbyak::CodeGenerator m_code{};
    std::vector<std::unique_ptr<HookedFn>> m_hooked_fns{};
};

class ScriptRunner : public Mod {
public:
    std::string_view get_name() const override { return "ScriptRunner"; }

    void on_draw_ui() override;
    void on_pre_application_entry(void* entry, const char* name, size_t hash) override;
    void on_application_entry(void* entry, const char* name, size_t hash) override;
    void on_pre_update_transform(RETransform* transform) override;
    void on_update_transform(RETransform* transform) override;

private:
    std::unique_ptr<ScriptState> m_state{new ScriptState{}};
    std::recursive_mutex m_access_mutex{};
};

