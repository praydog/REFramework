#pragma once

#include <cstdint>
#include <functional>
#include <vector>
#include <memory>
#include <mutex>

#include <asmjit/asmjit.h>

#include "utility/FunctionHook.hpp"
#include "sdk/RETypeDB.hpp"

class HookManager {
public:
    enum class PreHookResult : int {
        CALL_ORIGINAL,
        SKIP_ORIGINAL,
    };

    struct HookedFn;
    using PreHookFn = std::function<PreHookResult(std::vector<uintptr_t>& args, std::vector<sdk::RETypeDefinition*>& arg_tys)>;
    using PostHookFn = std::function<void(uintptr_t& ret_val, sdk::RETypeDefinition* ret_ty)>;
    using HookId = size_t;

    struct HookCallback {
        HookId id{};
        PreHookFn pre_fn{};
        PostHookFn post_fn{};
    };

    struct HookedFn {
        HookManager& hookman;
        void* target_fn{};
        std::vector<HookCallback> cbs{}; 
        HookId next_hook_id{};
        std::unique_ptr<FunctionHook> fn_hook{};
        uintptr_t facilitator_fn{};
        std::vector<uintptr_t> args{};
        std::vector<sdk::RETypeDefinition*> arg_tys{};
        uintptr_t ret_addr{};
        uintptr_t ret_val{};
        sdk::RETypeDefinition* ret_ty{};
        std::mutex mux{};

        HookedFn(HookManager& hm);
        ~HookedFn();

        PreHookResult on_pre_hook();
        void on_post_hook();

        __declspec(noinline) static void lock_static(HookedFn* fn) { fn->mux.lock(); }
        __declspec(noinline) static void unlock_static(HookedFn* fn) { fn->mux.unlock(); }
        __declspec(noinline) static PreHookResult on_pre_hook_static(HookedFn* fn) { return fn->on_pre_hook(); }
        __declspec(noinline) static void on_post_hook_static(HookedFn* fn) { fn->on_post_hook(); }
    };

    HookId add(sdk::REMethodDefinition* fn, PreHookFn pre_fn, PostHookFn post_fn, bool ignore_jmp = false);
    void remove(sdk::REMethodDefinition* fn, HookId id);

private:
    asmjit::JitRuntime m_jit{};
    std::mutex m_jit_mux{};
    std::unordered_map<sdk::REMethodDefinition*, std::unique_ptr<HookedFn>> m_hooked_fns{};
};

inline HookManager g_hookman{};