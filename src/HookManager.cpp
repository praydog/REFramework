#include <ranges>

#include <hde64.h>
#include <spdlog/spdlog.h>

#include "HookManager.hpp"

namespace detail {
void* get_actual_function(void* possible_fn) {
    if (possible_fn == nullptr) {
        return nullptr;
    }

    auto actual_fn = possible_fn;
    auto ip = (uintptr_t)possible_fn;

    // Disassemble the first few instructions to see if there is a jmp to an actual function.
    for (auto i = 0; i < 10; ++i) {
        hde64s hde{};
        auto len = hde64_disasm((void*)ip, &hde);
        ip += len;

        if (hde.opcode == 0xCC) { // int3, stop disassembling.
            break;
        }

        if (hde.opcode == 0xC3) { // ret, stop disassembling.
            break;
        }
        
        if (hde.opcode == 0xC2) { // ret, stop disassembling.
            break;
        }

        if (hde.opcode == 0xE9) { // jmp.
            actual_fn = (void*)(ip + hde.imm.imm32);
            break;
        }
    }

    if (possible_fn != actual_fn) {
        spdlog::info("[HookManager] Using actual function @ {:p} for wrapper function @ {:p}", actual_fn, possible_fn);
    }

    return actual_fn;
}
}

HookManager::HookedFn::HookedFn(HookManager& hm) : hookman{hm} {
}

HookManager::HookedFn::~HookedFn() {
    fn_hook.reset();

    if (facilitator_fn) {
        std::scoped_lock _{hookman.m_jit_mux};
        hookman.m_jit.release(facilitator_fn);
    }
}

HookManager::PreHookResult HookManager::HookedFn::on_pre_hook() {
    //std::shared_lock _{this->access_mux};

    auto any_skipped = false;

    auto storage = get_storage(this);

    if (storage->pre_depth == 0) {
        // afaik, shared locks are not reentrant, so only lock it
        // if we're not already in a pre-hook.
        this->access_mux.lock_shared();
    } else if (!storage->pre_warned_recursion) {
        const auto tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
        const auto declaring_type = fn_def->get_declaring_type();
        const auto decltype_name = declaring_type != nullptr ? declaring_type->get_full_name() : "unknownclass";
        spdlog::warn("[HookManager] (Pre) Recursive hook detected for '{}.{}' (thread ID: {:x})", decltype_name, fn_def->get_name(), tid);
        storage->pre_warned_recursion = true;
    }

    if (storage->overall_depth > 0 && !storage->overall_warned_recursion) {
        const auto tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
        const auto declaring_type = fn_def->get_declaring_type();
        const auto decltype_name = declaring_type != nullptr ? declaring_type->get_full_name() : "unknownclass";
        spdlog::warn("[HookManager] (Overall) '{}.{}' appears to be calling itself in some way (thread ID: {:x})", decltype_name, fn_def->get_name(), tid);
        storage->overall_warned_recursion = true;
    }

    ++storage->pre_depth;
    const auto ret_addr_pre = storage->ret_addr_pre;

    for (const auto& cb : cbs) {
        if (cb.pre_fn) {
            if (cb.pre_fn(storage->args_impl, arg_tys, ret_addr_pre) == PreHookResult::SKIP_ORIGINAL) {
                any_skipped = true;
            }
        }
    }

    ++storage->overall_depth;
    --storage->pre_depth;

    if (storage->pre_depth == 0) {
        this->access_mux.unlock_shared();
    }

    return any_skipped ? PreHookResult::SKIP_ORIGINAL : PreHookResult::CALL_ORIGINAL;
}

void HookManager::HookedFn::on_post_hook() {
    //std::shared_lock _{this->access_mux};

    auto storage = get_storage(this);

    if (storage->post_depth == 0) {
        // afaik, shared locks are not reentrant, so only lock it
        // if we're not already in a post-hook.
        this->access_mux.lock_shared();
    } else if (!storage->post_warned_recursion) {
        const auto tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
        const auto declaring_type = fn_def->get_declaring_type();
        const auto decltype_name = declaring_type != nullptr ? declaring_type->get_full_name() : "unknownclass";
        spdlog::warn("[HookManager] (Post) Recursive hook detected for '{}.{}' (thread ID: {:x})", decltype_name, fn_def->get_name(), tid);
        storage->post_warned_recursion = true;
    }

    ++storage->post_depth;
    --storage->overall_depth;

    auto& ret_val = storage->ret_val;
    //auto& ret_addr = storage->ret_addr_post;

    // Iterate in reverse because it helps with the hook storage we use in Lua
    // It should help with any other system that wants to use a stack-based storage system.
    for (const auto& cb : cbs | std::views::reverse) {
        if (cb.post_fn) {
            // Valid return address in recursion scenario is no longer supported with this API.
            // We just pass ret_addr_pre for now, even though it's not accurate.
            // Hooks will not have much use for the return address anyway.
            cb.post_fn(ret_val, ret_ty, storage->ret_addr_pre); 
        }
    }

    --storage->post_depth;

    if (storage->post_depth == 0) {
        this->access_mux.unlock_shared();
    }
}

void HookManager::create_jitted_facilitator(std::unique_ptr<HookManager::HookedFn>& hook, sdk::REMethodDefinition* fn, std::function<uintptr_t ()> hook_initialization, std::function<void ()> hook_create) {
    auto& args = hook->get_storage(hook.get())->args_impl;
    auto& arg_tys = hook->arg_tys;
    auto& fn_hook = hook->fn_hook;

    using namespace asmjit;
    using namespace asmjit::x86;

    std::scoped_lock _{m_jit_mux};
    CodeHolder code{};
    code.init(m_jit.environment());

    Assembler a{&code};

    // Make sure we have room to store the arguments.
    // + 2 for the thread context + this pointer.
    // Another + 2 for hidden arguments that we may not know about.
    constexpr auto HIDDEN_ARGUMENT_COUNT = 2;
    //args.resize(size_t(2) + HIDDEN_ARGUMENT_COUNT + fn->get_num_params());

    // Generate the facilitator function that will store the arguments, call on_hook, 
    // restore the arguments, and call the original function.
    auto hook_label = a.newLabel();
    auto on_pre_hook_label = a.newLabel();
    auto on_post_hook_label = a.newLabel();
    auto orig_label = a.newLabel();
    auto get_storage_label = a.newLabel();
    auto push_ptr_label = a.newLabel();
    auto pop_ptr_label = a.newLabel();
    auto lock_label = a.newLabel();
    auto unlock_label = a.newLabel();


    constexpr size_t STACK_STORAGE_AMOUNT = 80;

    // Save state and any volatile registers corresponding to arguments.
    a.mov(rax, ptr(rsp)); // return address.

    a.push(r12); // Temporary cross-call storage for storage ptr.
    a.push(r13); // Temporary storage for rbx, cross-call storage.
    a.push(r14); // Temporary storage for return address.

    a.mov(r13, rbx); // store rbx in r13.
    a.mov(r14, rax); // store return address in r14.

    a.push(rbx);

    a.push(rcx);
    a.push(rdx);
    a.push(r8);
    a.push(r9);

    // Store XMM arguments.
    auto store_xmm_args = [&]() {
        a.sub(rsp, 16 * 4);

        a.movdqu(ptr(rsp), xmm0);
        a.movdqu(ptr(rsp, 16), xmm1);
        a.movdqu(ptr(rsp, 32), xmm2);
        a.movdqu(ptr(rsp, 48), xmm3);
    };

    auto pop_xmm_args = [&]() {
        a.movdqu(xmm0, ptr(rsp));
        a.movdqu(xmm1, ptr(rsp, 16));
        a.movdqu(xmm2, ptr(rsp, 32));
        a.movdqu(xmm3, ptr(rsp, 48));

        a.add(rsp, 16 * 4);
    };

    store_xmm_args();

    // Fix stack.
    a.mov(rbx, rsp);
    a.sub(rsp, STACK_STORAGE_AMOUNT);
    a.and_(rsp, -16);

    //a.call(ptr(lock_label));
    a.mov(rcx, ptr(hook_label));
    a.call(ptr(get_storage_label));

    a.mov(r12, rax); // storage ptr.

    // Save return address (pre-hook).
    a.mov(ptr(r12, offsetof(HookedFn::HookStorage, ret_addr_pre)), r14);

    // Push return address onto stack.
    a.mov(rcx, r12); // storage ptr.
    a.mov (rdx, r14); // return address.
    a.call(ptr(push_ptr_label));

    // Use this moment to push RBX to our pseudo-stack.
    // because the pre-hook may call this function recursively, clobbering RBX.
    a.mov(rcx, r12); // storage ptr.
    a.mov(rdx, r13); // original rbx.
    a.call(ptr(push_ptr_label));

    // restore stack
    a.mov(rsp, rbx);

    pop_xmm_args();

    // Restore state.
    a.pop(r9);
    a.pop(r8);
    a.pop(rdx);
    a.pop(rcx);

    // restore rbx
    a.pop(rbx);

    // Fix temporary storage registers.
    a.pop(r14);
    a.pop(r13);

    a.mov(rax, r12); // storage ptr.
    a.pop(r12);

    a.mov(r10, rax); // save storage ptr for later.
    a.mov(rax, ptr(rax)); // args ptr now.

    constexpr auto hook_args_offset = offsetof(HookedFn::HookStorage, args);
    static_assert(hook_args_offset == 0, "HookedFn::HookStorage::args offset is not 0");

    // Store args.
    // TODO: Handle all the arguments the function takes.
    //a.mov(rax, ptr(args_label));
    a.mov(ptr(rax), rcx); // current thread context.

    auto args_start_offset = 8;

    if (!fn->is_static()) {
        args_start_offset = 16;
        a.mov(ptr(rax, 8), rdx); // this ptr... probably.
    }

    auto save_arg = [&a](uint32_t args_offset, bool is_float) {
        switch (args_offset) {
        case 8: // rdx/xmm1
            if (is_float) {
                a.movq(ptr(rax, args_offset), xmm1);
            } else {
                a.mov(ptr(rax, args_offset), rdx);
            }
            break;

        case 16: // r8/xmm2
            if (is_float) {
                a.movq(ptr(rax, args_offset), xmm2);
            } else {
                a.mov(ptr(rax, args_offset), r8);
            }
            break;

        case 24: // r9/xmm3
            if (is_float) {
                a.movq(ptr(rax, args_offset), xmm3);
            } else {
                a.mov(ptr(rax, args_offset), r9);
            }
            break;

        default:
            // stack args
            if (args_offset >= 32) {
                a.mov(r11, ptr(rsp, sizeof(void*) + (args_offset)));
                a.mov(ptr(rax, args_offset), r11);
            }

            break;
        }
    };
    
    const auto num_params = fn->get_num_params();

    // +2 for buffer params to fix possible corruption.
    for (auto i = 0u; i < num_params + HIDDEN_ARGUMENT_COUNT; ++i) {
        auto is_float = false;

        if (i < num_params) {
            auto arg_ty = arg_tys[i];

            if (arg_ty->get_full_name() == "System.Single") {
                is_float = true;
            }
        }

        const auto args_offset = args_start_offset + (i * 8);

        save_arg(args_offset, is_float);
    }


    a.mov(rax, rsp);
    a.mov(rax, ptr(rax)); // return address.

    a.push(r12); // push storage
    a.mov(r12, r10); // storage ptr.

    a.mov(rbx, rsp);
    a.sub(rsp, STACK_STORAGE_AMOUNT);
    a.and_(rsp, -16);

    // Call on_pre_hook.
    a.mov(rcx, ptr(hook_label));
    a.call(ptr(on_pre_hook_label));

    // Save the return value so we can see if we need to call the original later.
    a.mov(r11, rax);
    
    // restore rsp
    a.mov(rsp, rbx);
    a.mov(r10, r12); // storage ptr.

    a.pop(r12); // restore storage

    // Restore args.
    a.mov(rax, ptr(r10)); // set up args ptr from storage.
    a.mov(rcx, ptr(rax)); // current thread context.

    if (!fn->is_static()) {
        a.mov(rdx, ptr(rax, 8)); // this ptr... probably.
    }

    auto restore_arg = [&a](uint32_t args_offset, bool is_float) {
        switch (args_offset) {
        case 8: // rdx/xmm1
            if (is_float) {
                a.movq(xmm1, ptr(rax, args_offset));
            } else {
                a.mov(rdx, ptr(rax, args_offset));
            }
            break;

        case 16: // r8/xmm2
            if (is_float) {
                a.movq(xmm2, ptr(rax, args_offset));
            } else {
                a.mov(r8, ptr(rax, args_offset));
            }
            break;

        case 24: // r9/xmm3
            if (is_float) {
                a.movq(xmm3, ptr(rax, args_offset));
            } else {
                a.mov(r9, ptr(rax, args_offset));
            }
            break;

        default:
            if (args_offset >= 32) {
                a.mov(rax, ptr(r10));
                a.mov(rax, ptr(rax, args_offset));
                a.mov(ptr(rsp, sizeof(void*) + (args_offset)), rax);
                //a.mov(rax, ptr(r10)); // deref storage.
            }

            // TODO: handle stack args.
            break;
        }
    };

    // +2 for buffer params to fix possible corruption.
    for (auto i = 0u; i < num_params + HIDDEN_ARGUMENT_COUNT; ++i) {
        auto is_float = false;

        if (i < num_params) {
            auto arg_ty = arg_tys[i];

            if (arg_ty->get_full_name() == "System.Single") {
                is_float = true;
            }
        }

        auto args_offset = args_start_offset + (i * 8);
        
        restore_arg(args_offset, is_float);
    }

    // Call original function.
    auto ret_label = a.newLabel();
    auto skip_label = a.newLabel();

    // Save return address.
    //a.mov(rax, ptr(rsp));
    //a.mov(rax, ptr(ret_addr_label));
    //constexpr auto ret_addr_offset = offsetof(HookedFn::HookStorage, ret_addr);
    //a.mov(ptr(r10, ret_addr_offset), rax);
    //a.mov(r10, rax); // save storage ptr for later.

    // Overwrite return address.
    a.lea(rax, ptr(ret_label));
    a.mov(ptr(rsp), rax);

    // Store off our HookStorage in RBX.
    // RBX is safe if the called function respects the ABI.
    a.mov(rbx, r10);

    // Determine if we need to skip the original function or not.
    a.cmp(r11, (int)PreHookResult::CALL_ORIGINAL);
    a.jnz(skip_label);

    // Jmp to original function.
    a.jmp(ptr(orig_label));

    a.bind(skip_label);
    a.add(rsp, 8); // pop ret address.

    a.bind(ret_label);

    // Set hook storage back to R10.
    a.mov(r10, rbx);

    constexpr auto ret_val_offset = offsetof(HookedFn::HookStorage, ret_val);
    //a.lea(rcx, ptr(r10, ret_val_offset));

    auto is_ret_ty_float = hook->ret_ty->get_full_name() == "System.Single";

    if (is_ret_ty_float) {
        a.movq(ptr(r10, ret_val_offset), xmm0);
    } else {
        a.mov(ptr(r10, ret_val_offset), rax);
    }

    // Call on_post_hook.
    a.push(r12); // R12 being used as a cross-call register for storage of the storage ptr.
    a.push(r13);
    a.mov(r12, r10);

    a.mov(rbx, rsp);
    a.sub(rsp, STACK_STORAGE_AMOUNT);
    a.and_(rsp, -16);

    a.mov(rcx, ptr(hook_label));
    a.call(ptr(on_post_hook_label));

    // Now use this moment to pop RBX from our pseudo-stack.
    a.mov(rcx, r12); // storage ptr.
    a.call(ptr(pop_ptr_label));

    a.mov(r13, rax); // temp storage for original rbx.

    // Now pop return address off the stack
    a.mov(rcx, r12);
    a.call(ptr(pop_ptr_label));
    a.mov(r11, rax); // store return address in volatile register.
    
    a.mov(rsp, rbx); // Restore stack ptr.
    a.mov(rbx, r13); // restore original RBX, the return value from pop_rbx which we stored in r13.

    a.mov(r10, r12); // storage ptr.
    a.pop(r13);
    a.pop(r12);

    if (is_ret_ty_float) {
        a.movq(xmm0, ptr(r10, ret_val_offset));
    } else {
        a.mov(rax, ptr(r10, ret_val_offset));
    }

    //a.mov(r10, ptr(r10, ret_addr_offset)); // ret addr

    // Return.
    a.jmp(r11);

    a.bind(hook_label);
    a.dq((uint64_t)hook.get());
    a.bind(on_pre_hook_label);
    a.dq((uint64_t)&HookedFn::on_pre_hook_static);
    a.bind(on_post_hook_label);
    a.dq((uint64_t)&HookedFn::on_post_hook_static);
    a.bind(get_storage_label);
    a.dq((uint64_t)&HookedFn::get_storage);
    a.bind(push_ptr_label);
    a.dq((uint64_t)&HookedFn::push_ptr);
    a.bind(pop_ptr_label);
    a.dq((uint64_t)&HookedFn::pop_ptr);
    a.bind(lock_label);
    a.dq((uint64_t)&HookedFn::lock_static);
    a.bind(unlock_label);
    a.dq((uint64_t)&HookedFn::unlock_static);
    a.bind(orig_label);
    // Can't do the following because the hook hasn't been created yet.
    //a.dq(fn_hook->get_original());
    a.dq(0);

    m_jit.add(&hook->facilitator_fn, &code);

    *(uintptr_t*)(hook->facilitator_fn + code.labelOffsetFromBase(orig_label)) = hook_initialization();
}

HookManager::HookId HookManager::add(sdk::REMethodDefinition* fn, HookManager::PreHookFn pre_fn, HookManager::PostHookFn post_fn, bool ignore_jmp) {
    if (fn == nullptr) {
        //throw std::exception{"[HookManager] Cannot add nullptr function"};
        spdlog::error("[HookManager] Cannot add nullptr function");
        return HookId{};
    }
    
    auto target_fn = ignore_jmp ? fn->get_function() : detail::get_actual_function(fn->get_function());

    if (target_fn == nullptr) {
        spdlog::error("[HookManager] Cannot add method that resolves to nullptr");
        return HookId{};
    }

    spdlog::info("[HookManager] Adding hook for '{}' @ {:p}...", fn->get_name(), target_fn);

    if (auto search = m_hooked_fns.find(fn); search != m_hooked_fns.end()) {
        spdlog::info("[HookManager] Reusing existing hook...");

        auto& hook = search->second;
        std::scoped_lock _{hook->mux};
        std::unique_lock __{hook->access_mux};
        auto hook_id = m_next_hook_id++;

        spdlog::info("[HookManager] Hook assigned ID {}", hook_id);

        hook->cbs.emplace_back(hook_id, std::move(pre_fn), std::move(post_fn));

        spdlog::info("[HookManager] Hook {} added for '{}' @ {:p}", hook_id, fn->get_name(), target_fn);

        return hook_id;
    }

    spdlog::info("[HookManager] Creating a new hook...");

    auto hook = std::make_unique<HookedFn>(*this);
    hook->fn_def = fn;
    hook->next_hook_id = m_next_hook_id++;

    auto hook_id = m_next_hook_id++;

    spdlog::info("[HookManager] Hook assigned ID {}", hook_id);

    hook->target_fn = target_fn;
    hook->cbs.emplace_back(hook_id, std::move(pre_fn), std::move(post_fn));
    hook->arg_tys = fn->get_param_types();
    hook->ret_ty = fn->get_return_type();
    

    //auto& args = hook->args_impl;
    //auto& arg_tys = hook->arg_tys;
    auto& fn_hook = hook->fn_hook;

    // Create the facilitator! this really important!
    create_jitted_facilitator(hook, fn,
        [&](){
            fn_hook = std::make_unique<FunctionHook>(hook->target_fn, (void*)hook->facilitator_fn);
            if (!fn_hook->create()) {
                spdlog::error("[HookManager] Failed to hook function for '{}'", fn->get_name());
                return uintptr_t{0};
            }
            return fn_hook->get_original();
        },
        [&]() {
        }
    );

    m_hooked_fns.emplace(fn, std::move(hook));

    spdlog::info("[HookManager] Hook {} added for '{}' @ {:p}", hook_id, fn->get_name(), target_fn);

    return hook_id;
}

HookManager::HookId HookManager::add_vtable(::REManagedObject* obj, sdk::REMethodDefinition* fn, PreHookFn pre_fn, PostHookFn post_fn) {
#if TDB_VER == 49
    throw std::runtime_error("VTable hooks are not supported in TDB 49");
#endif
    
    if (obj == nullptr) {
        return HookId{};
    }

    if (fn == nullptr) {
        //throw std::exception{"[HookManager] Cannot add nullptr function"};
        spdlog::error("[HookManager] Cannot add nullptr function");
        return HookId{};
    }
    
    spdlog::info("[HookManager] Adding hook for '{}' @ {:p} (vtable index {})...", fn->get_name(), fn->get_function(), fn->get_virtual_index());

    if (fn->get_virtual_index() == -1) {
        spdlog::error("[HookManager] Cannot add non-virtual function with add_vtable, use add instead.");
        return HookId{};
    }

    auto search = m_hooked_vtables.find(obj);

    if (search != m_hooked_vtables.end()) {
        spdlog::info("[HookManager] Reusing existing VT hook...");
    } else {
        spdlog::info("[HookManager] Creating a new VT hook...");

        auto hook = std::make_unique<HookManager::HookedVTable>(*this);
        std::scoped_lock _{hook->mux};

        hook->vtable_hook = std::make_unique<sdk::REVTableHook>(obj);

        if (!hook->vtable_hook->is_hooked()) {
            spdlog::error("[HookManager] Failed to hook vtable for {:x}", (uintptr_t)obj);
            hook = nullptr;
            return 0;
        }

        m_hooked_vtables[obj] = std::move(hook);
        search = m_hooked_vtables.find(obj);

        spdlog::info("[HookManager] VT hook created for {:x}", (uintptr_t)obj);
    }

    auto& hook = search->second;

    std::scoped_lock _{hook->mux};

    // Now we need to find or create an existing function hook for the individual vtable method inside the vtable
    if (auto it = hook->hooked_fns.find(fn); it != hook->hooked_fns.end()) {
        spdlog::info("[HookManager] Reusing existing VT hook...");

        auto& hook_fn = it->second;

        std::unique_lock _{hook_fn->access_mux};

        auto hook_id = m_next_hook_id++;
        hook_fn->cbs.emplace_back(hook_id, std::move(pre_fn), std::move(post_fn));

        spdlog::info("[HookManager] VT Hook {} added for '{}' @ {:p}", hook_id, fn->get_name(), fn->get_function());

        return hook_id;
    }

    spdlog::info("[HookManager] Creating a new VT hook...");

    hook->hooked_fns[fn] = std::make_unique<HookedFn>(*this);

    auto& hook_fn = hook->hooked_fns[fn];
    hook_fn->fn_def = fn;
    hook_fn->next_hook_id = m_next_hook_id++;
    auto hook_id = m_next_hook_id++;

    spdlog::info("[HookManager] VT Hook assigned ID {}", hook_id);

    hook_fn->target_fn = fn->get_function();
    hook_fn->cbs.emplace_back(hook_id, std::move(pre_fn), std::move(post_fn));
    hook_fn->arg_tys = fn->get_param_types();
    hook_fn->ret_ty = fn->get_return_type();
    

    //auto& args = hook_fn->args;
    //auto& arg_tys = hook_fn->arg_tys;

    // Create the facilitator! this really important!
    create_jitted_facilitator(hook_fn, fn,
        [&]() -> uintptr_t {
            if (!hook->vtable_hook->hook_method(fn->get_virtual_index(), (void*)hook_fn->facilitator_fn)) {
                spdlog::error("[HookManager] Failed to hook vtable method for {:x}", (uintptr_t)obj);
                hook_fn = nullptr;
                return 0;
            }

            return hook->vtable_hook->get_original<uintptr_t>(fn->get_virtual_index());
        },
        [&]() -> void {
            // dont need to do anything.
        }
    );

    spdlog::info("[HookManager] VT Hook {} added for '{}' @ {:p}", hook_id, fn->get_name(), fn->get_function());

    return hook_id;
}

void HookManager::remove(sdk::REMethodDefinition* fn, HookId id) {
    if (auto search = m_hooked_fns.find(fn); search != m_hooked_fns.end()) {
        spdlog::info("[HookManager] Removing hook ID {} from '{}'", id, fn->get_name());

        auto& hook = search->second;
        auto& cbs = hook->cbs;
        std::scoped_lock _{hook->mux};
        std::unique_lock __{hook->access_mux};
        cbs.erase(std::remove_if(cbs.begin(), cbs.end(), [id](const HookCallback& cb) { return cb.id == id; }));
    } else {
        std::vector<::REManagedObject*> queued_vtable_deletions{};

        // Search through the vtable hooks.
        for (auto& it : m_hooked_vtables) {
            auto& hook = it.second;

            if (auto search = hook->hooked_fns.find(fn); search != hook->hooked_fns.end()) {
                spdlog::info("[HookManager] Removing VT method hook ID {} from '{}'", id, fn->get_name());

                auto& hook_fn = search->second;
                auto& cbs = hook_fn->cbs;
                std::scoped_lock _{hook->mux};
                cbs.erase(std::remove_if(cbs.begin(), cbs.end(), [id](const HookCallback& cb) { return cb.id == id; }), cbs.end());

                if (cbs.empty()) {
                    queued_vtable_deletions.push_back(it.first);
                }
            }
        }

        // Delete the vtable hooks.
        for (auto& obj : queued_vtable_deletions) {
            spdlog::info("[HookManager] Removing VT hook for {:x}", (uintptr_t)obj);
            m_hooked_vtables.erase(obj);
        }
    }
}
