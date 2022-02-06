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

    using PreHookFn = std::function<PreHookResult(std::vector<uintptr_t>)>;
    using PostHookFn = std::function<uintptr_t(uintptr_t)>;

    struct HookedFn {
        HookManager& hookman;
        void* target_fn{};
        std::vector<PreHookFn> pre_fns{};
        std::vector<PostHookFn> post_fns{};
        std::unique_ptr<FunctionHook> fn_hook{};
        uintptr_t facilitator_fn{};
        std::vector<uintptr_t> args{};
        std::vector<sdk::RETypeDefinition*> arg_tys{};
        uintptr_t ret_addr{};
        uintptr_t ret_val{};
        sdk::RETypeDefinition* ret_ty{};
        std::mutex mux{};

        ~HookedFn();

        PreHookResult on_pre_hook();
        void on_post_hook();

        __declspec(noinline) static void lock_static(HookedFn* fn) { fn->mux.lock(); }
        __declspec(noinline) static void unlock_static(HookedFn* fn) { fn->mux.unlock(); }
        __declspec(noinline) static PreHookResult on_pre_hook_static(HookedFn* fn) { return fn->on_pre_hook(); }
        __declspec(noinline) static void on_post_hook_static(HookedFn* fn) { fn->on_post_hook(); }
    };

    void add(sdk::REMethodDefinition* fn, PreHookFn pre_fn, PostHookFn post_fn, bool ignore_jmp = false);

private:
    asmjit::JitRuntime m_jit{};
    std::mutex m_jit_mux{};
    std::unordered_map<sdk::REMethodDefinition*, std::unique_ptr<HookedFn>> m_hooked_fns{};
};
