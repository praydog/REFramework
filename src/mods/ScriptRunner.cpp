#include <cstdint>

#include <imgui.h>

#include "../sdk/REContext.hpp"
#include "../sdk/REManagedObject.hpp"
#include "../sdk/RETypeDB.hpp"
#include "../sdk/SceneManager.hpp"

#include "utility/String.hpp"

#include "ScriptRunner.hpp"

namespace api::re {
void msg(const char* text) {
    MessageBox(g_framework->get_window(), text, "ScriptRunner Message", MB_ICONINFORMATION | MB_OK);
}
}

namespace api::sdk {
void* get_thread_context() {
    return (void*)::sdk::get_thread_context();
}

auto find_type_definition(const char* name) {
    return ::sdk::RETypeDB::get()->find_type(name);
}

void* get_native_singleton(const char* name) {
    return (void*)::sdk::get_native_singleton<void>(name);
}

auto get_managed_singleton(const char* name) {
    return g_framework->get_globals()->get(name);
}

void* create_managed_string(const char* text) {
    return ::sdk::VM::create_managed_string(utility::widen(text));
}

sol::object call_native_func(sol::object obj, ::sdk::RETypeDefinition* ty, const char* name, sol::variadic_args va) {
    static std::vector<void*> args{};
    auto l = va.lua_state();

    args.clear();

    void* real_obj = nullptr;

    if (!obj.is<sol::nil_t>()) {
        if (obj.is<REManagedObject*>()) {
            real_obj = (void*)obj.as<REManagedObject*>();
        } else if (obj.is<void*>()) {
            real_obj = obj.as<void*>();
        } else {
            real_obj = (void*)obj.as<uintptr_t>();
        }
    }

    for (auto&& arg : va) {
        auto i = arg.stack_index();

        // sol2 doesn't seem to differentiate between Lua integers and numbers. So
        // we must do it ourselves.
        if (lua_isinteger(l, i)) {
            auto n = (intptr_t)lua_tointeger(l, i);
            args.push_back((void*)n);
        } else if (lua_isnumber(l, i)) {
            auto f = lua_tonumber(l, i);
            auto n = *(intptr_t*)&f;
            args.push_back((void*)n);
        } else if (lua_isstring(l, i)) {
            auto s = lua_tostring(l, i);
            args.push_back(create_managed_string(s));
        } else {
            if (obj.is<Vector2f*>()) {
                auto& v = *obj.as<Vector2f*>();
                args.push_back(*(void**)&v);
            } else if (obj.is<Vector3f*>()) {
                auto v = obj.as<Vector3f*>();
                args.push_back((void*)v);
            } else if (obj.is<Vector4f*>()) {
                auto v = obj.as<Vector4f*>();
                args.push_back((void*)v);
            } else {
                args.push_back(arg.as<void*>());
            }
        }
    }

    auto ret_val = ::sdk::invoke_object_func(real_obj, ty, name, args);

    // Convert return values to the correct Lua types.
    auto fn = ty->get_method(name);
    auto ret_ty = fn->get_return_type();

    if (ret_ty != nullptr) {
        const auto full_name_hash = utility::hash(ret_ty->get_full_name());

        if (!ret_ty->is_value_type()) {
            if (ret_val.ptr == nullptr) {
                return sol::make_object(l, sol::nil);
            }
        }

        const auto vm_obj_type = ret_ty->get_vm_obj_type();

        switch (full_name_hash) {
        case "System.String"_fnv: {
            auto managed_ret_val = (::REManagedObject*)ret_val.ptr;
            auto managed_str = (SystemString*)((uintptr_t)utility::re_managed_object::get_field_ptr(managed_ret_val) - sizeof(::REManagedObject));
            auto str = utility::narrow(managed_str->data);

            return sol::make_object(l, str);
        }
        case "System.Single"_fnv: {
            // even though it's a single, it's actually a double because of the invoke wrapper conversion
            auto ret_val_f = *(double*)&ret_val;
            return sol::make_object(l, ret_val_f);
        }
        case "System.UInt32"_fnv: {
            auto ret_val_u = *(uint32_t*)&ret_val;
            return sol::make_object(l, ret_val_u);
        }
        case "System.Int32"_fnv: {
            auto ret_val_u = *(int32_t*)&ret_val;
            return sol::make_object(l, ret_val_u);
        }
        case "via.vec2"_fnv: {
            auto ret_val_v = *(Vector2f*)&ret_val;
            return sol::make_object(l, ret_val_v);
        }
        case "via.vec3"_fnv: {
            auto ret_val_v = *(Vector3f*)&ret_val;
            return sol::make_object(l, ret_val_v);
        }
        case "via.vec4"_fnv: {
            auto ret_val_v = *(Vector4f*)&ret_val;
            return sol::make_object(l, ret_val_v);
        }
        default:
            if (vm_obj_type > via::clr::VMObjType::NULL_ && vm_obj_type < via::clr::VMObjType::ValType) {
                return sol::make_object(l, (::REManagedObject*)ret_val.ptr);
            }
            break;
        }
    } else { 
        // A null void* will get converted into an userdata with value 0. That's not very useful in Lua, so
        // let's return nil instead since that's a much more usable value.
        if (ret_val.ptr == nullptr) {
            return sol::make_object(l, sol::nil);
        }
    }

    // TODO: handle more return type conversions.

    return sol::make_object(l, ret_val.ptr);
}

auto call_object_func(sol::object obj, const char* name, sol::variadic_args va) {
    void* real_obj = nullptr;

    if (!obj.is<sol::nil_t>()) {
        if (obj.is<REManagedObject*>()) {
            real_obj = (void*)obj.as<REManagedObject*>();
        } else if (obj.is<void*>()) {
            real_obj = obj.as<void*>();
        } else {
            real_obj = (void*)obj.as<uintptr_t>();
        }
    }

    auto def = utility::re_managed_object::get_type_definition((::REManagedObject*)real_obj);

    return call_native_func(obj, def, name, va);
}

void* get_primary_camera() {
    return ::sdk::get_primary_camera();
}

void hook(sol::this_state s, ::sdk::REMethodDefinition* fn, sol::function pre_cb, sol::function post_cb) {
    auto sol_state = sol::state_view{s};
    auto state = sol_state.registry()["state"].get<ScriptState*>();
    auto hook = std::make_unique<ScriptState::HookedFn>();

    hook->target_fn = (void*)fn->get_function();
    hook->script_pre_fn = pre_cb;
    hook->script_post_fn = post_cb;
    hook->script_args = sol_state.create_table();
    hook->arg_tys = fn->get_param_types();
    hook->ret_ty = fn->get_return_type();

    auto& args = hook->args;
    auto& arg_tys = hook->arg_tys;
    auto& fn_hook = hook->fn_hook;
    auto& g = hook->facilitator_gen;

    // Make sure we have room to store the arguments.
    args.resize(2 + fn->get_num_params());

    // Generate the facilitator function that will store the arguments, call on_hook, 
    // restore the arguments, and call the original function.
    Xbyak::Label hook_label{}, args_label{}, this_label{}, on_pre_hook_label{}, on_post_hook_label{}, 
        ret_addr_label{}, ret_val_label{}, orig_label{};

    // Store args.
    // TODO: Handle all the arguments the function takes.
    g.mov(g.rax, g.ptr[g.rip + args_label]);
    g.mov(g.ptr[g.rax], g.rcx); // current thread context.

    auto args_start_offset = 8;

    if (!fn->is_static()) {
        args_start_offset = 16;
        g.mov(g.ptr[g.rax + 8], g.rdx); // this ptr... probably.
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
                g.movq(g.ptr[g.rax + args_offset], g.xmm1);
            } else {
                g.mov(g.ptr[g.rax + args_offset], g.rdx);
            }
            break;

        case 16: // r8/xmm2
            if (is_float) {
                g.movq(g.ptr[g.rax + args_offset], g.xmm2);
            } else {
                g.mov(g.ptr[g.rax + args_offset], g.r8);
            }
            break;

        case 24: // r9/xmm3
            if (is_float) {
                g.movq(g.ptr[g.rax + args_offset], g.xmm3);
            } else {
                g.mov(g.ptr[g.rax + args_offset], g.r9);
            }
            break;

        default:
            // TODO: handle stack args.
            break;
        }
    }

    // Call on_pre_hook.
    g.mov(g.rcx, g.ptr[g.rip + this_label]);
    g.mov(g.rdx, g.ptr[g.rip + hook_label]);
    g.sub(g.rsp, 8);
    g.call(g.ptr[g.rip + on_pre_hook_label]);
    g.add(g.rsp, 8);

    // Restore args.
    g.mov(g.rax, g.ptr[g.rip + args_label]);
    g.mov(g.rcx, g.ptr[g.rax]); // current thread context.

    if (!fn->is_static()) {
        g.mov(g.rdx, g.ptr[g.rax + 8]); // this ptr... probably.
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
                g.movq(g.xmm1, g.ptr[g.rax + args_offset]);
            } else {
                g.mov(g.rdx, g.ptr[g.rax + args_offset]);
            }
            break;

        case 16: // r8/xmm2
            if (is_float) {
                g.movq(g.xmm2, g.ptr[g.rax + args_offset]);
            } else {
                g.mov(g.r8, g.ptr[g.rax + args_offset]);
            }
            break;

        case 24: // r9/xmm3
            if (is_float) {
                g.movq(g.xmm3, g.ptr[g.rax + args_offset]);
            } else {
                g.mov(g.r9, g.ptr[g.rax + args_offset]);
            }
            break;

        default:
            // TODO: handle stack args.
            break;
        }
    }

    // Call original function.
    Xbyak::Label ret_label{};

    // Save return address.
    g.mov(g.r10, g.ptr[g.rsp]);
    g.mov(g.rax, g.ptr[g.rip + ret_addr_label]);
    g.mov(g.ptr[g.rax], g.r10);

    // Overwrite return address.
    g.lea(g.rax, g.ptr[g.rip + ret_label]);
    g.mov(g.ptr[g.rsp], g.rax);

    // Jmp to original function.
    g.jmp(g.ptr[g.rip + orig_label]);

    g.L(ret_label);

    // Save return value.
    g.mov(g.rcx, g.ptr[g.rip + ret_val_label]);

    auto is_ret_ty_float = hook->ret_ty->get_full_name() == "System.Single";

    if (is_ret_ty_float) {
        g.movq(g.ptr[g.rcx], g.xmm0);
    } else {
        g.mov(g.ptr[g.rcx], g.rax);
    }

    // Call on_post_hook.
    g.mov(g.rcx, g.ptr[g.rip + this_label]);
    g.mov(g.rdx, g.ptr[g.rip + hook_label]);
    g.call(g.ptr[g.rip + on_post_hook_label]);

    // Restore return value.
    g.mov(g.rcx, g.ptr[g.rip + ret_val_label]);

    if (is_ret_ty_float) {
        g.movq(g.xmm0, g.ptr[g.rcx]);
    } else {
        g.mov(g.rax, g.ptr[g.rcx]);
    }

    g.mov(g.r10, g.ptr[g.rip + ret_addr_label]);
    g.jmp(g.ptr[g.r10]);
    
    g.L(hook_label);
    g.dq((uint64_t)hook.get());
    g.L(args_label);
    g.dq((uint64_t)args.data());
    g.L(this_label);
    g.dq((uint64_t)state);
    g.L(on_pre_hook_label);
    g.dq((uint64_t)&ScriptState::on_pre_hook_static);
    g.L(on_post_hook_label);
    g.dq((uint64_t)&ScriptState::on_post_hook_static);
    g.L(ret_addr_label);
    g.dq((uint64_t)&hook->ret_addr);
    g.L(ret_val_label);
    g.dq((uint64_t)&hook->ret_val);
    g.L(orig_label);
    // Can't do the following because the hook hasn't been created yet.
    //g.dq(fn_hook->get_original());
    g.dq(0);

    // Hook the function to our facilitator.
    fn_hook = std::make_unique<FunctionHook>(fn->get_function(), (void*)g.getCode());

    // Set the facilitators original function pointer.
    *(uintptr_t*)orig_label.getAddress() = fn_hook->get_original();

    fn_hook->create();
    state->hooked_fns().emplace_back(std::move(hook));
}
}

namespace api::log {
void info(const char* str) {
    spdlog::info(str);
}

void warn(const char* str) {
    spdlog::warn(str);
}

void error(const char* str) {
    spdlog::error(str);
}

void debug(const char* str) {
    OutputDebugString(str);
    spdlog::debug(str);
}
}

ScriptState::ScriptState() {
    m_lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::bit32, sol::lib::utf8);

    m_lua.registry()["state"] = this;

    auto re = m_lua.create_table();
    re["msg"] = api::re::msg;
    re["on_pre_application_entry"] = [this](const char* name, sol::function fn) { m_pre_application_entry_fns.emplace(name, fn); };
    re["on_application_entry"] = [this](const char* name, sol::function fn) { m_application_entry_fns.emplace(name, fn); };
    m_lua["re"] = re;

    auto sdk = m_lua.create_table();
    sdk["game_namespace"] = game_namespace;
    sdk["get_thread_context"] = api::sdk::get_thread_context;
    sdk["get_native_singleton"] = api::sdk::get_native_singleton;
    sdk["get_managed_singleton"] = api::sdk::get_managed_singleton;
    sdk["create_managed_string"] = api::sdk::create_managed_string;
    sdk["find_type_definition"] = api::sdk::find_type_definition;
    sdk["call_native_func"] = api::sdk::call_native_func;
    sdk["call_object_func"] = api::sdk::call_object_func;
    sdk["get_primary_camera"] = api::sdk::get_primary_camera;
    sdk["hook"] = api::sdk::hook;
    sdk["to_managed_object"] = [](void* ptr) { return (REManagedObject*)ptr; };
    sdk["to_double"] = [](void* ptr) { return *(double*)&ptr; };
    sdk["to_float"] = [](void* ptr) { return *(float*)&ptr; };
    sdk["to_int64"] = [](void* ptr) { return *(int64_t*)&ptr; };
    sdk["to_ptr"] = [](sol::object obj) {
        if (obj.is<int64_t>()) {
            auto n = obj.as<int64_t>();
            return *(void**)&n;
        } else if (obj.is<double>()) {
            auto n = obj.as<double>();
            return *(void**)&n;
        } else {
            return obj.as<void*>();
        }
    };
    sdk["float_to_ptr"] = [](float f) {
        return *(void**)&f;
    };
    m_lua["sdk"] = sdk;

    auto log = m_lua.create_table();
    log["info"] = api::log::info;
    log["warn"] = api::log::warn;
    log["error"] = api::log::error;
    log["debug"] = api::log::debug;
    m_lua["log"] = log;

    m_lua.new_usertype<::sdk::RETypeDefinition>("RETypeDefinition", 
        "get_method", &::sdk::RETypeDefinition::get_method);
    m_lua.new_usertype<REManagedObject>("REManagedObject", 
        "get_address", [](REManagedObject* obj) { return (void*)obj; },
        "get_type_definition", [](REManagedObject* obj) { return utility::re_managed_object::get_type_definition(obj); },
        "call",
        [this](REManagedObject* obj, const char* name, sol::variadic_args args) {
            return api::sdk::call_object_func(sol::make_object(m_lua, obj), name, args);
        });
    // add vec2 usertype
    m_lua.new_usertype<Vector2f>("Vector2f",
        "x", &Vector2f::x,
        "y", &Vector2f::y);

    // add vec3 usertype
    m_lua.new_usertype<Vector3f>("Vector3f",
        "x", &Vector3f::x,
        "y", &Vector3f::y,
        "z", &Vector3f::z);

    // add vec4 usertype
    m_lua.new_usertype<Vector4f>("Vector4f",
        "x", &Vector4f::x,
        "y", &Vector4f::y,
        "z", &Vector4f::z,
        "w", &Vector4f::w);
}

void ScriptState::run_script(const std::string& p) {
    /*m_lua.safe_script_file(p, [](lua_State*, sol::protected_function_result pfr) {
        if (!pfr.valid()) {
            sol::error err = pfr;
            api::re::msg(err.what());
        }
        return pfr;
    });*/
    try {
        m_lua.safe_script_file(p);
    } catch (const std::exception& e) {
        OutputDebugString(e.what());
        api::re::msg(e.what());
    }
}

void ScriptState::on_pre_application_entry(const char* name) {
    try {
        auto range = m_pre_application_entry_fns.equal_range(name);

        for (auto it = range.first; it != range.second; ++it) {
            it->second();
        }
    } catch (const std::exception& e) {
        OutputDebugString(e.what());
    }
}

void ScriptState::on_application_entry(const char* name) {
    try {
        auto range = m_application_entry_fns.equal_range(name);

        for (auto it = range.first; it != range.second; ++it) {
            it->second();
        }
    } catch (const std::exception& e) {
        OutputDebugString(e.what());
    }
}

void ScriptState::on_pre_hook(HookedFn* fn) {
    try {
        // Call the script function.
        // TODO: Take return value from the script function into account.

        // Convert the args to a table that we pass to the script function.
        for (auto i = 0u; i < fn->args.size(); ++i) {
            fn->script_args[i + 1] = (void*)fn->args[i];
        }

        fn->script_pre_fn(fn->script_args);

        // Apply the changes to arguments that the script function may have made.
        for (auto i = 0u; i < fn->args.size(); ++i) {
            auto arg = fn->script_args[i + 1];
            fn->args[i] = (uintptr_t)arg.get<void*>();
        }
    } catch (const std::exception& e) {
        OutputDebugString(e.what());
    }
}

void ScriptState::on_post_hook(HookedFn* fn) {
    try {
        fn->ret_val = (uintptr_t)fn->script_post_fn((void*)fn->ret_val).get<void*>();
    } catch (const std::exception& e) {
        OutputDebugString(e.what());
    }
}

void ScriptRunner::on_draw_ui() {
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_Once);

    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    if (ImGui::Button("Run script")) {
        OPENFILENAME ofn{};
        char file[260]{};

        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = g_framework->get_window();
        ofn.lpstrFile = file;
        ofn.nMaxFile = sizeof(file);
        ofn.lpstrFilter = "Lua script files (*.lua)\0*.lua\0";
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileName(&ofn) != FALSE) {
            m_state->run_script(file);
        }
    }

    if (ImGui::Button("Reset scripts")) {
        m_state = std::make_unique<ScriptState>();
    }
}

void ScriptRunner::on_pre_application_entry(void* entry, const char* name, size_t hash) {
    m_state->on_pre_application_entry(name);
}

void ScriptRunner::on_application_entry(void* entry, const char* name, size_t hash) {
    m_state->on_application_entry(name);
}
