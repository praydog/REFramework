#include <hde64.h>

#include "HookManager.hpp"

namespace detail {
void* get_actual_function(void* possible_fn) {
    auto actual_fn = possible_fn;
    auto ip = (uintptr_t)possible_fn;

    // Disassemble the first few instructions to see if there is a jmp to an actual function.
    for (auto i = 0; i < 10; ++i) {
        hde64s hde{};
        auto len = hde64_disasm((void*)ip, &hde);
        ip += len;

        if (hde.opcode == 0xE9) { // jmp.
            actual_fn = (void*)(ip + hde.imm.imm32);
            break;
        }
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
    auto any_skipped = false;

    for (const auto& cb : cbs) {
        if (cb.pre_fn(this) == PreHookResult::SKIP_ORIGINAL) {
            any_skipped = true;
        }
    } 

    return any_skipped ? PreHookResult::SKIP_ORIGINAL : PreHookResult::CALL_ORIGINAL;
}

void HookManager::HookedFn::on_post_hook() {
    for (const auto& cb : cbs) {
        cb.post_fn(this);
    }
}

HookManager::HookId HookManager::add(sdk::REMethodDefinition* fn, HookManager::PreHookFn pre_fn, HookManager::PostHookFn post_fn, bool ignore_jmp) {
    if (auto search = m_hooked_fns.find(fn); search != m_hooked_fns.end()) {
        auto& hook = search->second;
        std::scoped_lock _{hook->mux};
        auto hook_id = hook->next_hook_id++;
        hook->cbs.emplace_back(hook_id, std::move(pre_fn), std::move(post_fn));
        return hook_id;
    }

    auto hook = std::make_unique<HookedFn>(*this);
    auto hook_id = hook->next_hook_id++;

    hook->target_fn = ignore_jmp ? fn->get_function() : detail::get_actual_function(fn->get_function());
    hook->cbs.emplace_back(hook_id, std::move(pre_fn), std::move(post_fn));
    hook->arg_tys = fn->get_param_types();
    hook->ret_ty = fn->get_return_type();

    auto& args = hook->args;
    auto& arg_tys = hook->arg_tys;
    auto& fn_hook = hook->fn_hook;

    using namespace asmjit;
    using namespace asmjit::x86;

    std::scoped_lock _{m_jit_mux};
    CodeHolder code{};
    code.init(m_jit.environment());

    Assembler a{&code};

    // Make sure we have room to store the arguments.
    args.resize(2 + fn->get_num_params());

    // Generate the facilitator function that will store the arguments, call on_hook, 
    // restore the arguments, and call the original function.
    auto hook_label = a.newLabel();
    auto args_label = a.newLabel();
    auto this_label = a.newLabel();
    auto on_pre_hook_label = a.newLabel();
    auto on_post_hook_label = a.newLabel();
    auto ret_addr_label = a.newLabel();
    auto ret_val_label = a.newLabel();
    auto orig_label = a.newLabel();
    auto lock_label = a.newLabel();
    auto unlock_label = a.newLabel();

    // Save state.
    a.push(rcx);
    a.push(rdx);
    a.push(r8);
    a.push(r9);

    // Lock context.
    a.mov(rcx, ptr(hook_label));
    a.sub(rsp, 40);
    a.call(ptr(lock_label));
    a.add(rsp, 40);

    // Restore state.
    a.pop(r9);
    a.pop(r8);
    a.pop(rdx);
    a.pop(rcx);

    // Store args.
    // TODO: Handle all the arguments the function takes.
    a.mov(rax, ptr(args_label));
    a.mov(ptr(rax), rcx); // current thread context.

    auto args_start_offset = 8;

    if (!fn->is_static()) {
        args_start_offset = 16;
        a.mov(ptr(rax, 8), rdx); // this ptr... probably.
    }

    for (auto i = 0u; i < fn->get_num_params(); ++i) {
        auto arg_ty = arg_tys[i];
        auto args_offset = args_start_offset + (i * 8);
        auto is_float = false;

        if (arg_ty->get_full_name() == "System.Single") {
            is_float = true;
        }

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
            // TODO: handle stack args.
            break;
        }
    }

    // Call on_pre_hook.
    a.mov(rcx, ptr(hook_label));
    a.sub(rsp, 40);
    a.call(ptr(on_pre_hook_label));
    a.add(rsp, 40);

    // Save the return value so we can see if we need to call the original later.
    a.mov(r11, rax);

    // Restore args.
    a.mov(rax, ptr(args_label));
    a.mov(rcx, ptr(rax)); // current thread context.

    if (!fn->is_static()) {
        a.mov(rdx, ptr(rax, 8)); // this ptr... probably.
    }

    for (auto i = 0u; i < fn->get_num_params(); ++i) {
        auto arg_ty = arg_tys[i];
        auto args_offset = args_start_offset + (i * 8);
        auto is_float = false;

        if (arg_ty->get_full_name() == "System.Single") {
            is_float = true;
        }

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
            // TODO: handle stack args.
            break;
        }
    }

    // Call original function.
    auto ret_label = a.newLabel();
    auto skip_label = a.newLabel();

    // Save return address.
    a.mov(r10, ptr(rsp));
    a.mov(rax, ptr(ret_addr_label));
    a.mov(ptr(rax), r10);

    // Overwrite return address.
    a.lea(rax, ptr(ret_label));
    a.mov(ptr(rsp), rax);

    // Determine if we need to skip the original function or not.
    a.cmp(r11, (int)PreHookResult::CALL_ORIGINAL);
    a.jnz(skip_label);

    // Jmp to original function.
    a.jmp(ptr(orig_label));

    a.bind(skip_label);
    a.add(rsp, 8); // pop ret address.

    a.bind(ret_label);

    // Save return value.
    a.mov(rcx, ptr(ret_val_label));

    auto is_ret_ty_float = hook->ret_ty->get_full_name() == "System.Single";

    if (is_ret_ty_float) {
        a.movq(ptr(rcx), xmm0);
    } else {
        a.mov(ptr(rcx), rax);
    }

    // Call on_post_hook.
    a.mov(rcx, ptr(hook_label));
    a.call(ptr(on_post_hook_label));

    // Restore return value.
    a.mov(rcx, ptr(ret_val_label));

    if (is_ret_ty_float) {
        a.movq(xmm0, ptr(rcx));
    } else {
        a.mov(rax, ptr(rcx));
    }

    // Store state.
    a.push(rax);
    a.mov(r10, ptr(ret_addr_label));
    a.mov(r10, ptr(r10));
    a.push(r10);

    // Unlock context.
    a.mov(rcx, ptr(hook_label));
    a.sub(rsp, 32);
    a.call(ptr(unlock_label));
    a.add(rsp, 32);

    // Restore state.
    a.pop(r10);
    a.pop(rax);

    // Return.
    a.jmp(r10);

    a.bind(hook_label);
    a.dq((uint64_t)hook.get());
    a.bind(args_label);
    a.dq((uint64_t)args.data());
    a.bind(this_label);
    a.dq((uint64_t)this);
    a.bind(on_pre_hook_label);
    a.dq((uint64_t)&HookedFn::on_pre_hook_static);
    a.bind(on_post_hook_label);
    a.dq((uint64_t)&HookedFn::on_post_hook_static);
    a.bind(lock_label);
    a.dq((uint64_t)&HookedFn::lock_static);
    a.bind(unlock_label);
    a.dq((uint64_t)&HookedFn::unlock_static);
    a.bind(ret_addr_label);
    a.dq((uint64_t)&hook->ret_addr);
    a.bind(ret_val_label);
    a.dq((uint64_t)&hook->ret_val);
    a.bind(orig_label);
    // Can't do the following because the hook hasn't been created yet.
    //a.dq(fn_hook->get_original());
    a.dq(0);

    m_jit.add(&hook->facilitator_fn, &code);

    // Hook the function to our facilitator.
    fn_hook = std::make_unique<FunctionHook>(hook->target_fn, (void*)hook->facilitator_fn);

    // Set the facilitators original function pointer.
    *(uintptr_t*)(hook->facilitator_fn + code.labelOffsetFromBase(orig_label)) = fn_hook->get_original();

    fn_hook->create();
    m_hooked_fns.emplace(fn, std::move(hook));

    return hook_id;
}

void HookManager::remove(sdk::REMethodDefinition* fn, HookId id) {
    if (auto search = m_hooked_fns.find(fn); search != m_hooked_fns.end()) {
        auto& hook = search->second;
        auto& cbs = hook->cbs;
        std::scoped_lock _{hook->mux};
        cbs.erase(std::remove_if(cbs.begin(), cbs.end(), [id](const HookCallback& cb) { return cb.id == id; }));
    }
}
