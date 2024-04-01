#pragma once

#include <cstdint>
#include <functional>
#include <vector>
#include <memory>
#include <mutex>
#include <stack>

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
    using PreHookFn = std::function<PreHookResult(std::vector<uintptr_t>& args, std::vector<sdk::RETypeDefinition*>& arg_tys, uintptr_t ret_addr)>;
    using PostHookFn = std::function<void(uintptr_t& ret_val, sdk::RETypeDefinition* ret_ty, uintptr_t ret_addr)>;
    using HookId = size_t;

    struct HookCallback {
        HookId id{};
        PreHookFn pre_fn{};
        PostHookFn post_fn{};
    };

    struct HookedFn;

    struct HookedVTable {
        HookManager& hookman;
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
        //std::vector<uintptr_t> args{};
        std::vector<sdk::RETypeDefinition*> arg_tys{};
        //uintptr_t ret_addr_pre{};
        //uintptr_t ret_addr{};
        //uintptr_t ret_val{};
        sdk::REMethodDefinition* fn_def{};
        sdk::RETypeDefinition* ret_ty{};
        std::recursive_mutex mux{};
        std::shared_mutex access_mux{};

        bool is_virtual{false};
        HookedVTable* vtable{nullptr};

        // Per-thread storage for hooked function.
        struct HookStorage {
            size_t* args{};
            uintptr_t This{};
            uintptr_t ret_addr_pre{}; // VOLATILE.
            //uintptr_t ret_addr_post{}; // VOLATILE.
            uintptr_t ret_val{};
            
            std::stack<uintptr_t> ptr_stack{}; // full storage for pointer-sized values. Supports recursion.
            std::vector<size_t> args_impl{};

            uint32_t pre_depth{0};
            uint32_t overall_depth{0};
            uint32_t post_depth{0};
            bool pre_warned_recursion{false}; // for logging recursion.
            bool overall_warned_recursion{false}; // for logging recursion.
            bool post_warned_recursion{false}; // for logging recursion.
        };

        // Thread->storage
        std::unordered_map<size_t, std::unique_ptr<HookStorage>> thread_storage{};
        std::shared_mutex storage_mux{};

        HookedFn(HookManager& hm);
        ~HookedFn();

        PreHookResult on_pre_hook();
        void on_post_hook();

        __declspec(noinline) static void push_ptr(HookStorage* storage, uintptr_t reg) {
            storage->ptr_stack.push(reg);
        }

        __declspec(noinline) static uintptr_t pop_ptr(HookStorage* storage) {
            auto rbx = storage->ptr_stack.top();
            storage->ptr_stack.pop();
            return rbx;
        }

        __declspec(noinline) static HookStorage* get_storage(HookedFn* fn) {
            auto tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
            {
                std::shared_lock _{fn->storage_mux};

                if (auto it = fn->thread_storage.find(tid); it != fn->thread_storage.end()) {
                    return it->second.get();
                }
            }

            std::unique_lock _{fn->storage_mux};
            auto& ts = fn->thread_storage[tid];
            ts = std::make_unique<HookStorage>();
            ts->args_impl.resize(size_t(2) + 2 + fn->fn_def->get_num_params());
            ts->args = ts->args_impl.data();

            return ts.get();
        }

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

    HookId m_next_hook_id{1};
};

inline HookManager g_hookman{};