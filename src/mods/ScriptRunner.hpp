#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include <Windows.h>

#include <sol/sol.hpp>
#include <xbyak/xbyak.h>

#include "utility/FunctionHook.hpp"

#include "Mod.hpp"

class ScriptState {
public:
    struct HookedFn {
        void* target_fn{};
        sol::function script_fn{};

        std::unique_ptr<FunctionHook> fn_hook{};
        Xbyak::CodeGenerator facilitator_gen{};
        std::vector<uintptr_t> args{};
    };

    ScriptState();

    void run_script(const std::string& p);

    void on_pre_application_entry(const char* name);
    void on_application_entry(const char* name);

    void on_hook(HookedFn* fn);

    static void on_hook_static(ScriptState* s, HookedFn* fn) {
        s->on_hook(fn);
    }

    auto& hooked_fns() {
        return m_hooked_fns;
    }

private:
    sol::state m_lua{};
    std::unordered_multimap<std::string, sol::function> m_pre_application_entry_fns{};
    std::unordered_multimap<std::string, sol::function> m_application_entry_fns{};

    Xbyak::CodeGenerator m_code{};
    std::vector<std::unique_ptr<HookedFn>> m_hooked_fns{};
};

class ScriptRunner : public Mod {
public:
    std::string_view get_name() const override { return "ScriptRunner"; }

    void on_draw_ui() override;
    void on_pre_application_entry(void* entry, const char* name, size_t hash) override;
    void on_application_entry(void* entry, const char* name, size_t hash) override;

private:
    std::unique_ptr<ScriptState> m_state{new ScriptState{}};
};

