#include <cstdint>

#include "../../sdk/REContext.hpp"
#include "../../sdk/REManagedObject.hpp"
#include "../../sdk/RETypeDB.hpp"
#include "../../sdk/SceneManager.hpp"

#include "../ScriptRunner.hpp"

#include "Sdk.hpp"

namespace api {
struct SystemArray;
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

sol::object parse_data(lua_State* l, void* data, ::sdk::RETypeDefinition* data_type) {
    if (data_type != nullptr) {
        if (!data_type->is_value_type()) {
            if (data == nullptr || *(void**)data == nullptr) {
                return sol::make_object(l, sol::nil);
            }
        }

        const auto full_name_hash = utility::hash(data_type->get_full_name());
        const auto vm_obj_type = data_type->get_vm_obj_type();

        switch (full_name_hash) {
        case "via.Transform"_fnv: {
            return sol::make_object(l, *(::RETransform**)data);
        }
        case "System.String"_fnv: {
            auto managed_ret_val = *(::REManagedObject**)data;
            auto managed_str = (SystemString*)((uintptr_t)utility::re_managed_object::get_field_ptr(managed_ret_val) - sizeof(::REManagedObject));
            auto str = utility::narrow(managed_str->data);

            return sol::make_object(l, str);
        }
        case "System.Single"_fnv: {
            // even though it's a single, it's actually a double because of the invoke wrapper conversion
            auto ret_val_f = *(double*)data;
            return sol::make_object(l, ret_val_f);
        }
        case "System.UInt32"_fnv: {
            auto ret_val_u = *(uint32_t*)data;
            return sol::make_object(l, ret_val_u);
        }
        case "System.Int32"_fnv: {
            auto ret_val_u = *(int32_t*)data;
            return sol::make_object(l, ret_val_u);
        }
        case "via.vec2"_fnv: {
            auto ret_val_v = *(Vector2f*)data;
            return sol::make_object<Vector2f>(l, ret_val_v);
        }
        case "via.vec3"_fnv: {
            auto ret_val_v = *(Vector3f*)data;
            return sol::make_object<Vector3f>(l, ret_val_v);
        }
        case "via.vec4"_fnv: {
            auto ret_val_v = *(Vector4f*)data;
            return sol::make_object<Vector4f>(l, ret_val_v);
        }
        case "via.mat4"_fnv: {
            auto ret_val_m = *(Matrix4x4f*)data;
            return sol::make_object<Matrix4x4f>(l, ret_val_m);
        }
        case "via.Quaternion"_fnv: {
            auto ret_val_q = *(glm::quat*)data;
            return sol::make_object<glm::quat>(l, ret_val_q);
        }
        default:
            if (vm_obj_type > via::clr::VMObjType::NULL_ && vm_obj_type < via::clr::VMObjType::ValType) {
                switch (vm_obj_type) {
                case via::clr::VMObjType::Array:
                    return sol::make_object(l, *(::api::SystemArray**)data);
                default: {
                    const auto td = utility::re_managed_object::get_type_definition(*(::REManagedObject**)data);

                    // another fallback incase the method returns an object which is an array
                    if (td != nullptr && td->get_vm_obj_type() == via::clr::VMObjType::Array) {
                        return sol::make_object(l, *(::api::SystemArray**)data);
                    }

                    return sol::make_object(l, *(::REManagedObject**)data);
                }
                }
            }
            break;
        }
    }

    // A null void* will get converted into an userdata with value 0. That's not very useful in Lua, so
    // let's return nil instead since that's a much more usable value.
    if (data == nullptr || *(void**)data == nullptr) {
        return sol::make_object(l, sol::nil);
    }

    return sol::make_object(l, *(void**)data);
}

void set_data(void* data, ::sdk::RETypeDefinition* data_type, sol::object& value) {
    if (data_type != nullptr) {
        const auto full_name_hash = utility::hash(data_type->get_full_name());
        const auto vm_obj_type = data_type->get_vm_obj_type();

        switch (full_name_hash) {
        case "System.Single"_fnv:
            *(float*)data = value.as<float>();
            return;
        case "System.UInt32"_fnv:
            *(uint32_t*)data = value.as<uint32_t>();
            break;
        case "System.Int32"_fnv:
            *(int32_t*)data = value.as<int32_t>();
            return;
        case "via.vec2"_fnv:
            *(Vector2f*)data = value.as<Vector2f>();
            return;
        case "via.vec3"_fnv:
            *(Vector3f*)data = value.as<Vector3f>();
            return;
        case "via.vec4"_fnv:
            *(Vector4f*)data = value.as<Vector4f>();
            return;
        case "via.mat4"_fnv:
            *(Matrix4x4f*)data = value.as<Matrix4x4f>();
            return;
        case "via.Quaternion"_fnv:
            *(glm::quat*)data = value.as<glm::quat>();
            return;
        default:
            if (vm_obj_type > via::clr::VMObjType::NULL_ && vm_obj_type < via::clr::VMObjType::ValType) {
                *(::REManagedObject**)data = value.as<::REManagedObject*>();
                return;
            }
        }
    }

    // A null void* will get converted into an userdata with value 0. That's not very useful in Lua, so
    // let's return nil instead since that's a much more usable value.
    if (data == nullptr || *(void**)data == nullptr) {
        return;
    }

    *(void**)data = value.as<void*>();
}

void set_native_field(sol::object obj, ::sdk::RETypeDefinition* ty, const char* name, sol::object value) {
    auto l = value.lua_state();
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

    const auto field = ty->get_field(name);

    if (field == nullptr) {
        return;
    }

    const auto field_type = field->get_type();

    if (!field->is_static() && !field_type->is_value_type() && real_obj == nullptr) {
        return;
    }

    auto data = field->get_data_raw(real_obj, ty->is_value_type());

    if (data == nullptr) {
        return;
    }

    set_data(data, field_type, value);
}

sol::object get_native_field(sol::object obj, ::sdk::RETypeDefinition* ty, const char* name, sol::variadic_args va) {
    auto l = va.lua_state();
    void* real_obj = nullptr;

    bool managed_obj_passed = false;

    if (!obj.is<sol::nil_t>()) {
        if (obj.is<REManagedObject*>()) {
            real_obj = (void*)obj.as<REManagedObject*>();
            managed_obj_passed = true;
        } else if (obj.is<void*>()) {
            real_obj = obj.as<void*>();
        } else {
            real_obj = (void*)obj.as<uintptr_t>();
        }
    }

    const auto field = ty->get_field(name);

    if (field == nullptr || (!field->is_static() && real_obj == nullptr)) {
        return sol::make_object(l, sol::nil);
    }

    const auto field_type = field->get_type();
    auto data = field->get_data_raw(real_obj, ty->is_value_type() && !managed_obj_passed);

    if (data == nullptr) {
        return sol::make_object(l, sol::nil);
    }

    return parse_data(l, data, field_type);
}

sol::object call_native_func(sol::object obj, ::sdk::RETypeDefinition* ty, const char* name, sol::variadic_args va) {
    static std::vector<void*> args{};
    static std::vector<Vector4f> vec_storage{};
    auto l = va.lua_state();

    args.clear();
    vec_storage.clear();

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
        } else if (arg.is<Vector2f>()) {
            auto& v = arg.as<Vector2f&>();
            args.push_back((void*)&vec_storage.emplace_back(v.x, v.y, 0.0f, 0.0f));
        } else if (arg.is<Vector3f>()) {
            auto& v = arg.as<Vector3f&>();
            args.push_back((void*)&vec_storage.emplace_back(v.x, v.y, v.z, 0.0f));
        } else if (arg.is<Vector4f>()) {
            auto& v = arg.as<Vector4f&>();
            args.push_back((void*)&v);
        } else if (arg.is<Matrix4x4f>()) {
            auto& v = arg.as<Matrix4x4f&>();
            args.push_back((void*)&v);
        } else if (arg.is<glm::quat>()) {
            auto& v = arg.as<glm::quat&>();
            args.push_back((void*)&v);
        } else if (arg.is<::REManagedObject*>()) {
            args.push_back(arg.as<::REManagedObject*>());
        } else {
            args.push_back(arg.as<void*>());
        }
    }

    auto ret_val = ::sdk::invoke_object_func(real_obj, ty, name, args);

    // Convert return values to the correct Lua types.
    auto fn = ty->get_method(name);
    auto ret_ty = fn->get_return_type();

    return parse_data(l, &ret_val, ret_ty);
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

::REManagedObject* get_primary_camera() {
    return (::REManagedObject*)::sdk::get_primary_camera();
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
        ret_addr_label{}, ret_val_label{}, orig_label{}, lock_label{}, unlock_label{};

    // Save state.
    g.push(g.rcx);
    g.push(g.rdx);
    g.push(g.r8);
    g.push(g.r9);

    // Lock context.
    g.mov(g.rcx, g.ptr[g.rip + this_label]);
    g.sub(g.rsp, 40);
    g.call(g.ptr[g.rip + lock_label]);
    g.add(g.rsp, 40);

    // Restore state.
    g.pop(g.r9);
    g.pop(g.r8);
    g.pop(g.rdx);
    g.pop(g.rcx);

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

    // Store state.
    g.push(g.rax);
    g.mov(g.r10, g.ptr[g.rip + ret_addr_label]);
    g.push(g.r10);

    // Unlock context.
    g.mov(g.rcx, g.ptr[g.rip + this_label]);
    g.sub(g.rsp, 32);
    g.call(g.ptr[g.rip + unlock_label]);
    g.add(g.rsp, 32);

    // Restore state.
    g.pop(g.r10);
    g.pop(g.rax);

    // Return.
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
    g.L(lock_label);
    g.dq((uint64_t)&ScriptState::lock_static);
    g.L(unlock_label);
    g.dq((uint64_t)&ScriptState::unlock_static);
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

namespace api {
struct SystemArray : public ::REManagedObject {
    int32_t size() {
        static auto system_array_type = api::sdk::find_type_definition("System.Array");
        static auto get_length_method = system_array_type->get_method("GetLength");

        return get_length_method->call<int32_t>(sdk::get_thread_context(), this, 0);
    }

    ::REManagedObject* get_element(int32_t index) {
        static auto system_array_type = api::sdk::find_type_definition("System.Array");
        static auto get_element_method = system_array_type->get_method("GetValue(System.Int32)");

        return get_element_method->call<::REManagedObject*>(sdk::get_thread_context(), this, index);
    }

    std::vector<::REManagedObject*> get_elements() {
        std::vector<::REManagedObject*> elements;

        for (int32_t i = 0; i < size(); i++) {
            elements.push_back(get_element(i));
        }

        return elements;
    }
};
}

void bindings::open_sdk(ScriptState* s) {
    auto& lua = s->lua();
    auto sdk = lua.create_table();
    sdk["game_namespace"] = game_namespace;
    sdk["get_thread_context"] = api::sdk::get_thread_context;
    sdk["get_native_singleton"] = api::sdk::get_native_singleton;
    sdk["get_managed_singleton"] = api::sdk::get_managed_singleton;
    sdk["create_managed_string"] = api::sdk::create_managed_string;
    sdk["find_type_definition"] = api::sdk::find_type_definition;
    sdk["call_native_func"] = api::sdk::call_native_func;
    sdk["call_object_func"] = api::sdk::call_object_func;
    sdk["get_native_field"] = api::sdk::get_native_field;
    sdk["set_native_field"] = api::sdk::set_native_field;
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
    lua["sdk"] = sdk;

    lua.new_usertype<::sdk::RETypeDefinition>("RETypeDefinition", 
        "get_method", &::sdk::RETypeDefinition::get_method);
    lua.new_usertype<REManagedObject>("REManagedObject", 
        "get_address", [](REManagedObject* obj) { return (void*)obj; },
        "get_type_definition", [](REManagedObject* obj) { return utility::re_managed_object::get_type_definition(obj); },
        "get_field", [s](REManagedObject* obj, const char* name, sol::variadic_args args) {
            return api::sdk::get_native_field(sol::make_object(s->lua(), obj), utility::re_managed_object::get_type_definition(obj), name, args); 
        },
        "set_field", [s](REManagedObject* obj, const char* name, sol::object value) {
            return api::sdk::set_native_field(sol::make_object(s->lua(), obj), utility::re_managed_object::get_type_definition(obj), name, value); 
        },
        "call", [s](REManagedObject* obj, const char* name, sol::variadic_args args) {
            return api::sdk::call_object_func(sol::make_object(s->lua(), obj), name, args);
        }
    );

    lua.new_usertype<REComponent>("REComponent",
        sol::base_classes, sol::bases<REManagedObject>());

    lua.new_usertype<RETransform>("RETransform",
        "calculate_base_transform", &utility::re_transform::calculate_base_transform,
        sol::base_classes, sol::bases<REComponent, REManagedObject>());

    lua.new_usertype<api::SystemArray>("SystemArray",
        "get_size", &api::SystemArray::size,
        "get_element", &api::SystemArray::get_element,
        "get_elements", &api::SystemArray::get_elements,
        sol::meta_function::index, [](api::SystemArray* arr, int32_t index) {
            return arr->get_element(index);
        },
        sol::base_classes, sol::bases<REManagedObject>());
}
