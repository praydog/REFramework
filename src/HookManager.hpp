#pragma once

#include <cstdint>
#include <functional>
#include <vector>
#include <memory>
#include <mutex>

#include <asmjit/asmjit.h>

#include "utility/FunctionHook.hpp"
#include "sdk/REVTableHook.hpp"
#include "sdk/RETypeDB.hpp"

class REManagedObject;

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

    struct HookedFn;

    struct HookedVTable {
        HookManager& hookman;
        HookId next_hook_id{};
        std::unique_ptr<sdk::REVTableHook> vtable_hook{};
        std::unordered_map<sdk::REMethodDefinition*, std::unique_ptr<HookedFn>> hooked_fns{};

        std::recursive_mutex mux{};
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
        std::recursive_mutex mux{};

        bool is_virtual{false};
        HookedVTable* vtable{nullptr};

        HookedFn(HookManager& hm);
        ~HookedFn();

        PreHookResult on_pre_hook();
        void on_post_hook();

        __declspec(noinline) static void lock_static(HookedFn* fn) { 
            fn->mux.lock();

            if (fn->is_virtual) {
                fn->vtable->mux.lock();
            }
        }
        __declspec(noinline) static void unlock_static(HookedFn* fn) {
            if (fn->is_virtual) {
                fn->vtable->mux.unlock();
            }

            fn->mux.unlock(); 
        }
        __declspec(noinline) static PreHookResult on_pre_hook_static(HookedFn* fn) { return fn->on_pre_hook(); }
        __declspec(noinline) static void on_post_hook_static(HookedFn* fn) { fn->on_post_hook(); }
    };

    HookId add(sdk::REMethodDefinition* fn, PreHookFn pre_fn, PostHookFn post_fn, bool ignore_jmp = false);
    HookId add_vtable(::REManagedObject* obj, sdk::REMethodDefinition* fn, PreHookFn pre_fn, PostHookFn post_fn);

    struct EitherOr {
        ::REManagedObject* obj{nullptr};
        sdk::REMethodDefinition* fn{nullptr};
        bool ignore_jmp{false};
    };
    HookId add_either_or(const EitherOr& either_or, PreHookFn pre_fn, PostHookFn post_fn) {
        if (either_or.obj == nullptr) {
            return add(either_or.fn, pre_fn, post_fn, either_or.ignore_jmp);
        } else {
            return add_vtable(either_or.obj, either_or.fn, pre_fn, post_fn);
        }
    }
    void remove(sdk::REMethodDefinition* fn, HookId id);

private:
    void create_jitted_facilitator(
        std::unique_ptr<HookedFn>& hooked_fn, 
        sdk::REMethodDefinition* fn,
        std::function<uintptr_t ()> hook_initialization,
        std::function<void ()> hook_create);

    asmjit::JitRuntime m_jit{};
    std::mutex m_jit_mux{};
    std::unordered_map<sdk::REMethodDefinition*, std::unique_ptr<HookedFn>> m_hooked_fns{};
    std::unordered_map<::REManagedObject*, std::unique_ptr<HookedVTable>> m_hooked_vtables{};
};

inline HookManager g_hookman{};