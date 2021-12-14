#include <cstdint>
#include <concepts>

#include <hde64.h>

#include "../../sdk/REContext.hpp"
#include "../../sdk/REManagedObject.hpp"
#include "../../sdk/RETypeDB.hpp"
#include "../../sdk/SceneManager.hpp"
#include "../../utility/Memory.hpp"

#include "../ScriptRunner.hpp"
#include <lstate.h> // weird include order because of sol
#include <lgc.h>

#include "Sdk.hpp"

namespace detail {
constexpr uintptr_t FAKE_OBJECT_ADDR = 12345;
}

namespace api::re_managed_object {
namespace detail {
void add_ref(lua_State* l, ::REManagedObject* obj, bool force = false) {
    auto sv = sol::state_view{l};
    
    // we shouldn't really do this very much
    // so it shouldn't be too terrible on performance
    if (!utility::re_managed_object::is_managed_object(obj)) {
        throw sol::error{(std::stringstream{} << "sol_lua_push: " << (uintptr_t)obj << " is not a managed object").str()};
    }

    // Throwing automatic add_ref on the backburner; it doesn't seem to work
    // very well with lua's garbage collector.
    // addendum: maybe figured it out

    // only add a reference if it's a "local" object, indicated by a negative reference count
    //if ((int32_t)obj->referenceCount < 0 || (current_ref_count && *current_ref_count > 0)) {
    // addendum: only do it when reference count is > 0, local objects seem buggy...
    if (force || (int32_t)obj->referenceCount > 0) {
        if (!force) {
            utility::re_managed_object::add_ref(obj);
        }

        // the reference counting is not necessary, but it will let us
        // catch bugs if an REManagedObject pointer is being created
        // without coming through our sol_lua_push function
        sol::lua_table ref_counts = sv["_sol_lua_push_ref_counts"];
        sol::lua_table ephemeral_counts = sv["_sol_lua_push_ephemeral_counts"];
        std::optional<int> current_ref_count = ref_counts[(uintptr_t)obj];

        if (current_ref_count && *current_ref_count > 0) {
            // don't unnecessarily increase the ref count
            // if the user is the one doing it
            if (force) {
                return;
            }

            ref_counts[(uintptr_t)obj] = *current_ref_count + 1;
        } else {
            // only add the ref once when the user requests it
            // so they don't screw something up
            if (force) {
                utility::re_managed_object::add_ref(obj);
            }

            ref_counts[(uintptr_t)obj] = 1;
        }

        // when the user adds a ref to an ephemeral object
        if (force) {
            std::optional<int> current_ephemeral_count = ephemeral_counts[(uintptr_t)obj];

            if (current_ephemeral_count && *current_ephemeral_count > 0) {
                ephemeral_counts[(uintptr_t)obj] = *current_ephemeral_count - 1;
            } else {
                ephemeral_counts[(uintptr_t)obj] = sol::make_object(l, sol::nil);
            }
        }
    } else {
        // ephemeral counts is just a table
        // to help with tracking the local objects
        // so we don't spam the log with warnings when they get gc'd
        sol::lua_table ephemeral_counts = sv["_sol_lua_push_ephemeral_counts"];
        std::optional<int> current_ref_count = ephemeral_counts[(uintptr_t)obj];

        if (current_ref_count) {
            ephemeral_counts[(uintptr_t)obj] = *current_ref_count + 1;
        } else {
            ephemeral_counts[(uintptr_t)obj] = 1;
        }

        //ref_counts[(uintptr_t)obj] = sol::make_object(l, sol::nil);
    }
}
}

// used by metatable for REManagedObject
::REManagedObject* add_ref(sol::this_state s, ::REManagedObject* obj) {
    detail::add_ref(s.lua_state(), obj, true);

    return obj;
}
}

// specialization for REManagedObject to automatically add a reference
// when lua pushes a pointer to the object onto the stack
template<detail::ManagedObjectBased T>
int sol_lua_push(sol::types<T*>, lua_State* l, T* obj) {
    if (obj != nullptr) {
        auto sv = sol::state_view{l};
        sol::lua_table objects = sv["_sol_lua_push_objects"];

        if (sol::object lua_obj = objects[(uintptr_t)obj]; !lua_obj.is<sol::nil_t>()) {
            // renew the reference so it doesn't get collected
            // had to dig deep in the lua source to figure out this nonsense
            lua_obj.push();
            auto g = G(l);
            auto tv = s2v(l->top - 1);
            auto& gc = tv->value_.gc;
            resetbits(gc->marked, bitmask(BLACKBIT) | WHITEBITS); // "touches" the object, marking it gray. lowers the insane GC frequency on our weak table

            return 1;
        } else {
            if ((uintptr_t)obj != detail::FAKE_OBJECT_ADDR) {
                api::re_managed_object::detail::add_ref(l, obj, false);
            }

            auto backpedal = sol::stack::push<sol::detail::as_pointer_tag<std::remove_pointer_t<T>>>(l, obj);

            if ((uintptr_t)obj != detail::FAKE_OBJECT_ADDR) {
                auto ref = sol::stack::get<sol::object>(l, -backpedal);

                // keep a weak reference to the object for caching
                objects[(uintptr_t)obj] = ref;

                return backpedal;
            }

            return backpedal;
        }
    }

    return sol::stack::push(l, sol::nil);
}

namespace api::sdk {
std::vector<void*>& build_args(sol::variadic_args va);
sol::object parse_data(lua_State* l, void* data, ::sdk::RETypeDefinition* data_type, bool from_method);
sol::object get_native_field(sol::object obj, ::sdk::RETypeDefinition* ty, const char* name);
void set_native_field(sol::object obj, ::sdk::RETypeDefinition* ty, const char* name, sol::object value);

struct ValueType {
    std::vector<uint8_t> data{};
    ::sdk::RETypeDefinition* type{nullptr};

    ValueType(::sdk::RETypeDefinition* t)
        : type(t)
    {
        if (type != nullptr) {
            data.resize(type->get_size());
        }
    }

    template <typename T>
    bool is_valid_offset(int32_t offset, T& value) const {
        return offset >= 0 && offset + sizeof(T) <= (int32_t)data.size();
    }

    bool is_valid_offset(int32_t offset) const {
        return offset >= 0 && offset <= (int32_t)data.size();
    }

    template <typename T>
    void write_memory(int32_t offset, T value) {
        if (!is_valid_offset(offset, value)) {
            return;
        }

        *(T*)((uintptr_t)data.data() + offset) = value;
    }

    template <typename T>
    T read_memory(int32_t offset) {
        if (!is_valid_offset(offset)) {
            return {};
        }

        return *(T*)((uintptr_t)data.data() + offset);
    }

    uintptr_t address() const {
        return (uintptr_t)data.data();
    }

    sol::object call(sol::this_state l, const char* name, sol::variadic_args va) {
        if (type == nullptr) {
            return sol::make_object(l, sol::nil);
        }

        auto real_obj = (void*)address();
        auto def = type->get_method(name);

        if (def == nullptr) {
            return sol::make_object(l, sol::nil);
        }

        auto ret_val = def->invoke(real_obj, ::api::sdk::build_args(va));

        if (ret_val.exception_thrown) {
            throw sol::error("Invoke threw an exception");
        }

        // Convert return values to the correct Lua types.
        auto ret_ty = def->get_return_type();

        return ::api::sdk::parse_data(l, &ret_val, ret_ty, true);
    }

    sol::object get_field(sol::this_state l, const char* name) {
        if (type == nullptr) {
            return sol::make_object(l, sol::nil);
        }

        return ::api::sdk::get_native_field(sol::make_object(l, (void*)address()), type, name);
    }

    void set_field(sol::this_state l, const char* name, sol::object value) {
        if (type == nullptr) {
            return;
        }

        ::api::sdk::set_native_field(sol::make_object(l, (void*)address()), type, name, value);
    }
};

void* get_thread_context() {
    return (void*)::sdk::get_thread_context();
}

auto find_type_definition(const char* name) {
    return ::sdk::RETypeDB::get()->find_type(name);
}

sol::object typeof(sol::this_state s, const char* name) {
    auto type_definition = find_type_definition(name);

    if (type_definition == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, type_definition->get_runtime_type());
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

sol::object create_instance(sol::this_state s, const char* name) {
    auto type_definition = find_type_definition(name);

    if (type_definition == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, type_definition->create_instance_full());
}

void* get_real_obj(sol::object obj) {
    void* real_obj = nullptr;

    if (!obj.is<sol::nil_t>()) {
        if (obj.is<REManagedObject*>()) {
            real_obj = (void*)obj.as<REManagedObject*>();
        } else if (obj.is<ValueType>()) {
            real_obj = (void*)obj.as<ValueType&>().address();
        } else if (obj.is<void*>()) {
            real_obj = obj.as<void*>();
        } else {
            real_obj = (void*)obj.as<uintptr_t>();
        }
    }

    return real_obj;
}

sol::object parse_data(lua_State* l, void* data, ::sdk::RETypeDefinition* data_type, bool from_method) {
    if (data_type != nullptr) {
        if (!data_type->is_value_type()) {
            if (data == nullptr || *(void**)data == nullptr) {
                return sol::make_object(l, sol::nil);
            }
        }

        size_t full_name_hash{};

        // Slightly different logic for enums
        if (data_type->is_enum()) {
            auto underlying_type = data_type->get_underlying_type();

            if (underlying_type != nullptr) {
                full_name_hash = utility::hash(underlying_type->get_full_name());
            }
        } else {
            full_name_hash = utility::hash(data_type->get_full_name());
        }

        const auto vm_obj_type = data_type->get_vm_obj_type();

        switch (full_name_hash) {
        case "via.Transform"_fnv: {
            return sol::make_object(l, *(::RETransform**)data);
        }
        case "System.String"_fnv: {
            const auto managed_ret_val = *(::REManagedObject**)data;
            const auto managed_str = (SystemString*)((uintptr_t)utility::re_managed_object::get_field_ptr(managed_ret_val) - sizeof(::REManagedObject));
            const auto str = utility::narrow(managed_str->data);

            return sol::make_object(l, str);
        }
        case "System.Single"_fnv: {
            if (from_method) {
                // even though it's a single, it's actually a double because of the invoke wrapper conversion
                auto ret_val_f = *(double*)data;
                return sol::make_object(l, ret_val_f);
            } else {
                auto ret_val_f = *(float*)data;
                return sol::make_object(l, ret_val_f);
            }
        }
        case "System.Boolean"_fnv: {
            auto ret_val_b = *(bool*)data;
            return sol::make_object(l, ret_val_b);
        }
        case "System.SByte"_fnv: {
            auto ret_val_i = *(int8_t*)data;
            return sol::make_object(l, ret_val_i);
        }
        case "System.Byte"_fnv: {
            auto ret_val_b = *(uint8_t*)data;
            return sol::make_object(l, ret_val_b);
        }
        case "System.Int16"_fnv: {
            auto ret_val_i = *(int16_t*)data;
            return sol::make_object(l, ret_val_i);
        }
        case "System.UInt16"_fnv: {
            auto ret_val_i = *(uint16_t*)data;
            return sol::make_object(l, ret_val_i);
        }
        case "System.UInt32"_fnv: {
            auto ret_val_u = *(uint32_t*)data;
            return sol::make_object(l, ret_val_u);
        }
        case "System.Int32"_fnv: {
            auto ret_val_u = *(int32_t*)data;
            return sol::make_object(l, ret_val_u);
        }
        case "System.Int64"_fnv: {
            auto ret_val_u = *(int64_t*)data;
            return sol::make_object(l, ret_val_u);
        }
        case "System.UInt64"_fnv: {
            auto ret_val_u = *(uint64_t*)data;
            return sol::make_object(l, ret_val_u);
        }
        case "via.Float2"_fnv: [[fallthrough]];
        case "via.vec2"_fnv: {
            auto ret_val_v = *(Vector2f*)data;
            return sol::make_object<Vector2f>(l, ret_val_v);
        }
        case "via.Float3"_fnv: [[fallthrough]];
        case "via.vec3"_fnv: {
            auto ret_val_v = *(Vector3f*)data;
            return sol::make_object<Vector3f>(l, ret_val_v);
        }
        case "via.Float4"_fnv: [[fallthrough]];
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
        case "via.GameObjectRef"_fnv: {
            static auto object_ref_type = ::sdk::RETypeDB::get()->find_type("via.GameObjectRef");
            static auto get_target_func = object_ref_type->get_method("get_Target");
            auto obj = get_target_func->call<::REManagedObject*>(sdk::get_thread_context(), data);

            if (obj == nullptr) {
                return sol::make_object(l, sol::nil);
            }

            return sol::make_object(l, obj);
        }
        default:
            if (vm_obj_type > via::clr::VMObjType::NULL_ && vm_obj_type < via::clr::VMObjType::ValType) {
                switch (vm_obj_type) {
                case via::clr::VMObjType::Array:
                    return sol::make_object(l, *(::sdk::SystemArray**)data);
                default: {
                    const auto td = utility::re_managed_object::get_type_definition(*(::REManagedObject**)data);

                    // another fallback incase the method returns an object which is an array
                    if (td != nullptr && td->get_vm_obj_type() == via::clr::VMObjType::Array) {
                        return sol::make_object(l, *(::sdk::SystemArray**)data);
                    }

                    return sol::make_object(l, *(::REManagedObject**)data);
                }
                }
            }
            break;
        }

        // so, we managed to get here, but we don't know what to do with the data
        // check if it's a valuetype first
        if (data_type->is_value_type()) {
            auto new_obj = sol::make_object(l, ValueType{ data_type });
            auto& bytes = new_obj.as<ValueType&>();
            
            memcpy(bytes.data.data(), data, data_type->get_size());

            return new_obj;
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
        size_t full_name_hash{};
        const auto vm_obj_type = data_type->get_vm_obj_type();

        if (data_type->is_enum()) {
            auto underlying_type = data_type->get_underlying_type();

            if (underlying_type != nullptr) {
                full_name_hash = utility::hash(underlying_type->get_full_name());
            }
        } else {
            full_name_hash = utility::hash(data_type->get_full_name());
        }

        switch (full_name_hash) {
        case "System.Single"_fnv:
            *(float*)data = value.as<float>();
            return;
        case "System.Boolean"_fnv:
            *(bool*)data = value.as<bool>();
            return;
        case "System.SByte"_fnv:
            *(int8_t*)data = value.as<int8_t>();
            return;
        case "System.Byte"_fnv:
            *(uint8_t*)data = value.as<uint8_t>();
            return;
        case "System.Int16"_fnv:
            *(int16_t*)data = value.as<int16_t>();
            return;
        case "System.UInt16"_fnv:
            *(uint16_t*)data = value.as<uint16_t>();
            return;
        case "System.UInt32"_fnv:
            *(uint32_t*)data = value.as<uint32_t>();
            return;
        case "System.Int32"_fnv:
            *(int32_t*)data = value.as<int32_t>();
            return;
        case "System.Int64"_fnv:
            *(int64_t*)data = value.as<int32_t>();
            return;
        case "System.UInt64"_fnv:
            *(uint64_t*)data = value.as<int32_t>();
            return;
        case "via.Float2"_fnv: [[fallthrough]];
        case "via.vec2"_fnv:
            *(Vector2f*)data = value.as<Vector2f>();
            return;
        case "via.Float3"_fnv: [[fallthrough]];
        case "via.vec3"_fnv:
            *(Vector3f*)data = value.as<Vector3f>();
            return;
        case "via.Float4"_fnv: [[fallthrough]];
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
    auto real_obj = get_real_obj(obj);

    bool managed_obj_passed = obj.is<REManagedObject*>();

    const auto field = ty->get_field(name);

    if (field == nullptr) {
        return;
    }

    const auto field_type = field->get_type();

    if (!field->is_static() && !field_type->is_value_type() && real_obj == nullptr) {
        return;
    }

    auto data = field->get_data_raw(real_obj, ty->is_value_type() && !managed_obj_passed);

    if (data == nullptr) {
        return;
    }

    set_data(data, field_type, value);
}

sol::object get_native_field_from_field(sol::object obj, ::sdk::RETypeDefinition* ty, ::sdk::REField* field) {
    auto l = obj.lua_state();

    auto real_obj = get_real_obj(obj);
    bool managed_obj_passed = obj.is<REManagedObject*>();

    if (field == nullptr || (!field->is_static() && real_obj == nullptr)) {
        return sol::make_object(l, sol::nil);
    }

    const auto field_type = field->get_type();
    auto data = field->get_data_raw(real_obj, ty->is_value_type() && !managed_obj_passed);

    if (data == nullptr) {
        return sol::make_object(l, sol::nil);
    }

    return parse_data(l, data, field_type, false);
}

sol::object get_native_field(sol::object obj, ::sdk::RETypeDefinition* ty, const char* name) {
    auto l = obj.lua_state();
    const auto field = ty->get_field(name);

    if (field == nullptr) {
        return sol::make_object(l, sol::nil);
    }

    return get_native_field_from_field(obj, ty, field);
}

std::vector<void*>& build_args(sol::variadic_args va) {
    auto l = va.lua_state();

    static std::vector<void*> args{};
    static std::vector<Vector4f> vec_storage{};

    args.clear();
    vec_storage.clear();

    for (auto&& arg : va) {
        auto i = arg.stack_index();

        if (lua_isnil(l, i)) {
            args.push_back(nullptr);
            continue;
        }

        // sol2 doesn't seem to differentiate between Lua integers and numbers. So
        // we must do it ourselves.
        if (lua_isboolean(l, i)) {
            auto b = lua_toboolean(l, i);
            args.push_back((void*)(intptr_t)b);
        } else if (lua_isinteger(l, i)) {
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
        } else if (arg.is<ValueType>()) {
            auto& b = arg.as<ValueType&>();
            args.push_back((void*)b.address());
        } else {
            args.push_back(arg.as<void*>());
        }
    }

    return args;
}

sol::object call_native_func(sol::object obj, ::sdk::RETypeDefinition* ty, const char* name, sol::variadic_args va) {
    auto l = va.lua_state();

    
    // Convert return values to the correct Lua types.
    auto fn = ty->get_method(name);

    if (fn == nullptr) {
        return sol::make_object(l, sol::nil);
    }

    auto ret_ty = fn->get_return_type();

    if (ret_ty == nullptr) {
        return sol::make_object(l, sol::nil);
    }

    auto real_obj = get_real_obj(obj);
    auto ret_val = fn->invoke(real_obj, build_args(va));

    if (ret_val.exception_thrown) {
        throw sol::error("Invoke threw an exception");
    }

    return parse_data(l, &ret_val, ret_ty, true);
}

auto call_object_func(sol::object obj, const char* name, sol::variadic_args va) {
    auto real_obj = get_real_obj(obj);

    if (real_obj == nullptr) {
        return sol::make_object(obj.lua_state(), sol::nil);
    }

    auto def = utility::re_managed_object::get_type_definition((::REManagedObject*)real_obj);

    if (def == nullptr) {
        return sol::make_object(obj.lua_state(), sol::nil);
    }

    return call_native_func(obj, def, name, va);
}

sol::object get_primary_camera(sol::this_state s) {
    return sol::make_object(s, (::REManagedObject*)::sdk::get_primary_camera());
}

bool is_managed_object(sol::object obj) {
    auto real_obj = get_real_obj(obj);

    if (real_obj == nullptr) {
        return false;
    }

    return utility::re_managed_object::is_managed_object(real_obj);
}

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

void hook(sol::this_state s, ::sdk::REMethodDefinition* fn, sol::function pre_cb, sol::function post_cb) {
    auto sol_state = sol::state_view{s};
    auto state = sol_state.registry()["state"].get<ScriptState*>();
    auto& hooked_fns = state->hooked_fns();

    if (auto search = hooked_fns.find(fn); search != hooked_fns.end()) {
        auto& hooked_fn = search->second;
        hooked_fn->script_pre_fns.emplace_back(pre_cb);
        hooked_fn->script_post_fns.emplace_back(post_cb);
        return;
    }

    auto hook = std::make_unique<ScriptState::HookedFn>();

    hook->target_fn = detail::get_actual_function(fn->get_function());
    hook->script_pre_fns.emplace_back(pre_cb);
    hook->script_post_fns.emplace_back(post_cb);
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
    g.sub(g.rsp, 40);
    g.call(g.ptr[g.rip + on_pre_hook_label]);
    g.add(g.rsp, 40);

    // Save the return value so we can see if we need to call the original later.
    g.mov(g.r11, g.rax);

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
    Xbyak::Label ret_label{}, skip_label{};

    // Save return address.
    g.mov(g.r10, g.ptr[g.rsp]);
    g.mov(g.rax, g.ptr[g.rip + ret_addr_label]);
    g.mov(g.ptr[g.rax], g.r10);

    // Overwrite return address.
    g.lea(g.rax, g.ptr[g.rip + ret_label]);
    g.mov(g.ptr[g.rsp], g.rax);

    // Determine if we need to skip the original function or not.
    g.cmp(g.r11, (int)ScriptState::PreHookResult::CALL_ORIGINAL);
    g.jnz(skip_label);

    // Jmp to original function.
    g.jmp(g.ptr[g.rip + orig_label]);

    g.L(skip_label);
    g.add(g.rsp, 8); // pop ret address.

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
    g.mov(g.r10, g.ptr[g.r10]);
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
    g.jmp(g.r10);
    
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
    fn_hook = std::make_unique<FunctionHook>(hook->target_fn, (void*)g.getCode());

    // Set the facilitators original function pointer.
    *(uintptr_t*)orig_label.getAddress() = fn_hook->get_original();

    fn_hook->create();
    hooked_fns.emplace(fn, std::move(hook));
}
}

namespace api::re_managed_object {
bool is_valid_offset(::REManagedObject* obj, int32_t offset) {
    if (obj == nullptr || !::utility::re_managed_object::is_managed_object(obj)) {
        return false;
    }

    const auto typedefinition = ::utility::re_managed_object::get_type_definition(obj);

    if (typedefinition == nullptr) {
        return false;
    }

    // trying to limit the impact of this function.
    if (offset < -0x1000 || (uint32_t)offset >= typedefinition->get_size()) {
        return false;
    }

    return true;
}

template <typename T>
void write_memory(::REManagedObject* obj, int32_t offset, T value) {
    if (!is_valid_offset(obj, offset)) {
        return;
    }

    *(T*)((uintptr_t)obj + offset) = value;
}

template <typename T>
T read_memory(::REManagedObject* obj, int32_t offset) {
    if (!is_valid_offset(obj, offset)) {
        return 0;
    }

    return *(T*)((uintptr_t)obj + offset);
}
} 

void bindings::open_sdk(ScriptState* s) {
    auto& lua = s->lua();

    //lua["_sol_lua_push_objects"] = std::unordered_map<::REManagedObject*, sol::object>();
    lua.do_string(R"(
        _sol_lua_push_objects = setmetatable({}, { __mode = "v" })
        _sol_lua_push_ref_counts = {}
        _sol_lua_push_ephemeral_counts = {}
    )");

    auto sdk = lua.create_table();
    sdk["game_namespace"] = game_namespace;
    sdk["get_thread_context"] = api::sdk::get_thread_context;
    sdk["get_native_singleton"] = api::sdk::get_native_singleton;
    sdk["get_managed_singleton"] = api::sdk::get_managed_singleton;
    sdk["create_managed_string"] = api::sdk::create_managed_string;
    sdk["create_instance"] = api::sdk::create_instance;
    sdk["find_type_definition"] = api::sdk::find_type_definition;
    sdk["typeof"] = api::sdk::typeof;
    sdk["call_native_func"] = api::sdk::call_native_func;
    sdk["call_object_func"] = api::sdk::call_object_func;
    sdk["get_native_field"] = api::sdk::get_native_field;
    sdk["set_native_field"] = api::sdk::set_native_field;
    sdk["get_primary_camera"] = api::sdk::get_primary_camera;
    sdk["hook"] = api::sdk::hook;
    sdk.new_enum("PreHookResult", "CALL_ORIGINAL", ScriptState::PreHookResult::CALL_ORIGINAL, "SKIP_ORIGINAL", ScriptState::PreHookResult::SKIP_ORIGINAL);
    sdk["is_managed_object"] = api::sdk::is_managed_object;
    sdk["to_managed_object"] = [s](void* ptr) { return sol::make_object(s->lua(), (REManagedObject*)ptr); };
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
        sol::meta_function::equal_to, [](::sdk::RETypeDefinition* lhs, ::sdk::RETypeDefinition* rhs) { return lhs == rhs; },
        "get_methods", [](sdk::RETypeDefinition* def) -> std::vector<sdk::REMethodDefinition*> {
            std::vector<sdk::REMethodDefinition*> methods{};

            for (auto& method : def->get_methods()) {
                const auto func = method.get_function();

                if (method.get_function() == nullptr) {
                    continue;
                }

                const auto bytes = (uint8_t*)func;

                if (utility::is_stub_code(bytes)) {
                    continue;
                }

                methods.push_back(&method);
            }

            return methods;
        },
        "get_fields", [](sdk::RETypeDefinition* def) -> std::vector<sdk::REField*> {
            std::vector<sdk::REField*> fields{};

            for (auto field : def->get_fields()) {
                fields.push_back(field);
            }

            return fields;
        },
        "get_full_name", &sdk::RETypeDefinition::get_full_name,
        "get_name", &sdk::RETypeDefinition::get_name,
        "get_namespace", &sdk::RETypeDefinition::get_namespace,
        "get_method", &::sdk::RETypeDefinition::get_method,
        "get_field", &::sdk::RETypeDefinition::get_field,
        "get_runtime_type", &::sdk::RETypeDefinition::get_runtime_type,
        "get_parent_type", &::sdk::RETypeDefinition::get_parent_type,
        "get_size", &::sdk::RETypeDefinition::get_size,
        "get_valuetype_size", &::sdk::RETypeDefinition::get_valuetype_size,
        "is_value_type", &::sdk::RETypeDefinition::is_value_type,
        "is_by_ref", &::sdk::RETypeDefinition::is_by_ref,
        "is_pointer", &::sdk::RETypeDefinition::is_pointer,
        "is_primitive", &::sdk::RETypeDefinition::is_primitive,
        "is_a", [](sdk::RETypeDefinition* def, sol::object comp) -> bool {
            if (comp.is<sdk::RETypeDefinition*>()) {
                return def->is_a(comp.as<sdk::RETypeDefinition*>());
            } else if (comp.is<const char*>()) {
                return def->is_a(comp.as<const char*>());
            }

            return false;
        },
        "create_instance", &::sdk::RETypeDefinition::create_instance_full);
    
    lua.new_usertype<sdk::REMethodDefinition>("REMethodDefinition",
        "get_name", &sdk::REMethodDefinition::get_name,
        "get_return_type", &sdk::REMethodDefinition::get_return_type,
        "get_function", &sdk::REMethodDefinition::get_function,
        "get_declaring_type", &sdk::REMethodDefinition::get_declaring_type,
        "get_num_params", &sdk::REMethodDefinition::get_num_params,
        "get_param_types", &sdk::REMethodDefinition::get_param_types,
        "get_param_names", &sdk::REMethodDefinition::get_param_names,
        "is_static", &sdk::REMethodDefinition::is_static,
        "call", [](sdk::REMethodDefinition* def, sol::object obj, sol::variadic_args va) {
            auto l = va.lua_state();

            auto real_obj = ::api::sdk::get_real_obj(obj);
            auto ret_val = def->invoke(real_obj, ::api::sdk::build_args(va));

            if (ret_val.exception_thrown) {
                throw sol::error("Invoke threw an exception");
            }

            // Convert return values to the correct Lua types.
            auto ret_ty = def->get_return_type();

            return ::api::sdk::parse_data(l, &ret_val, ret_ty, true);
        }
    );
    
    lua.new_usertype<sdk::REField>("REField",
        "get_name", &sdk::REField::get_name,
        "get_type", &sdk::REField::get_type,
        "get_offset_from_base", &sdk::REField::get_offset_from_base,
        "get_offset_from_fieldptr", &sdk::REField::get_offset_from_fieldptr,
        "get_declaring_type", &sdk::REField::get_declaring_type,
        "get_flags", &sdk::REField::get_flags,
        "is_static", &sdk::REField::is_static,
        "is_literal", &sdk::REField::is_literal,
        "get_data", [s](sdk::REField* f, sol::object obj) -> sol::object {
            auto& l = s->lua();

            if (obj.is<::REManagedObject*>()) {
                auto managed_obj = obj.as<::REManagedObject*>();

                if (managed_obj == nullptr) {
                    return sol::make_object(l, sol::nil);
                }

                auto ty = utility::re_managed_object::get_type_definition(managed_obj);

                if (ty == nullptr) {
                    return sol::make_object(l, sol::nil);
                }

                return api::sdk::get_native_field_from_field(obj, ty, f);
            }

            auto ty = f->get_declaring_type();

            if (ty == nullptr) {
                return sol::make_object(l, sol::nil);
            }

            return api::sdk::get_native_field_from_field(obj, ty, f);
        }
    );

    lua.new_usertype<::REManagedObject>("REManagedObject",
        sol::meta_function::equal_to, [s](REManagedObject* lhs, REManagedObject* rhs) { return lhs == rhs; },
        "add_ref", &api::re_managed_object::add_ref,
        "release", [](sol::this_state s, ::REManagedObject* obj) {
            auto l = s.lua_state();
            auto sv = sol::state_view(l);

            sol::lua_table objects = sv["_sol_lua_push_objects"];
            sol::lua_table ref_counts = sv["_sol_lua_push_ref_counts"];
            sol::lua_table ephemeral_counts = sv["_sol_lua_push_ephemeral_counts"];

            if (std::optional<int> ref_count = ref_counts[(uintptr_t)obj]; ref_count && *ref_count > 0) {
                // because of our internal refcount keeping, we shouldn't need to double check
                // whether it's an actual object or not. hopefully?
                //if (utility::re_managed_object::is_managed_object(obj)) {
                    utility::re_managed_object::release(obj);
                //}

                int new_ref_count = *ref_count - 1;

                if (new_ref_count == 0) {
                    if (sol::object object = ephemeral_counts[(uintptr_t)obj]; !object.valid()) {
                        objects[(uintptr_t)obj] = sol::make_object(l, sol::nil);
                    }

                    ref_counts[(uintptr_t)obj] = sol::make_object(l, sol::nil);
                } else {
                    ref_counts[(uintptr_t)obj] = new_ref_count;
                }

                //ephemeral_counts[(uintptr_t)obj] = sol::make_object(l, sol::nil);
            } else if (std::optional<int> ephemeral_count = ephemeral_counts[(uintptr_t)obj]; ephemeral_count && *ephemeral_count > 0) {
                // ephemeral counts don't actually release the object, they just decrement the count.
                int new_ephemeral_count = *ephemeral_count - 1;

                if (new_ephemeral_count == 0) {
                    objects[(uintptr_t)obj] = sol::make_object(l, sol::nil);
                    ephemeral_counts[(uintptr_t)obj] = sol::make_object(l, sol::nil);
                } else {
                    ephemeral_counts[(uintptr_t)obj] = new_ephemeral_count;
                }
            } else {
                spdlog::warn("REManagedObject:release attempted to release an object that was not managed by our Lua state");
            }
        },
        "get_address", [](REManagedObject* obj) { return (uintptr_t)obj; },
        "get_type_definition", &utility::re_managed_object::get_type_definition,
        "get_field", [s](REManagedObject* obj, const char* name) {
            if (obj == nullptr) {
                return sol::make_object(s->lua(), sol::nil);
            }

            return api::sdk::get_native_field(sol::make_object(s->lua(), obj), utility::re_managed_object::get_type_definition(obj), name); 
        },
        "set_field", [s](REManagedObject* obj, const char* name, sol::object value) {
            if (obj == nullptr) {
                return;
            }

            return api::sdk::set_native_field(sol::make_object(s->lua(), obj), utility::re_managed_object::get_type_definition(obj), name, value); 
        },
        "call", [s](REManagedObject* obj, const char* name, sol::variadic_args args) {
            if (obj == nullptr) {
                return sol::make_object(s->lua(), sol::nil);
            }

            return api::sdk::call_object_func(sol::make_object(s->lua(), obj), name, args);
        },
        "write_byte", &api::re_managed_object::write_memory<uint8_t>,
        "write_short", &api::re_managed_object::write_memory<uint16_t>,
        "write_dword", &api::re_managed_object::write_memory<uint32_t>,
        "write_qword", &api::re_managed_object::write_memory<uint64_t>,
        "write_float", &api::re_managed_object::write_memory<float>,
        "write_double", &api::re_managed_object::write_memory<double>,
        "read_byte", &api::re_managed_object::read_memory<uint8_t>,
        "read_short", &api::re_managed_object::read_memory<uint16_t>,
        "read_dword", &api::re_managed_object::read_memory<uint32_t>,
        "read_qword", &api::re_managed_object::read_memory<uint64_t>,
        "read_float", &api::re_managed_object::read_memory<float>,
        "read_double", &api::re_managed_object::read_memory<double>
    );

    // templated lambda
    auto create_managed_object_ptr_gc = [&]<detail::ManagedObjectBased T>(T* obj) {
        lua["__REManagedObjectPtrInternalCreate"] = [s]() -> sol::object {
            return sol::make_object(s->lua(), (T*)12345);
        };

        lua.do_string(R"(
            local fake_obj = __REManagedObjectPtrInternalCreate()
            local mt = getmetatable(fake_obj)

            fake_obj = nil
            collectgarbage("collect")

            mt.__gc = function(obj)
                obj:release()
            end
        )");

        lua["__REManagedObjectPtrInternalCreate"] = sol::make_object(lua, sol::nil);
    };

    create_managed_object_ptr_gc((::REManagedObject*)nullptr);

    lua.new_usertype<REComponent>("REComponent",
        sol::base_classes, sol::bases<::REManagedObject>()
    );

    create_managed_object_ptr_gc((::REComponent*)nullptr);

    lua.new_usertype<RETransform>("RETransform",
        "calculate_base_transform", &utility::re_transform::calculate_base_transform,
        sol::base_classes, sol::bases<::REComponent, ::REManagedObject>()
    );

    create_managed_object_ptr_gc((::RETransform*)nullptr);

    lua.new_usertype<sdk::SystemArray>("SystemArray",
        "get_size", &sdk::SystemArray::size,
        "get_element", &sdk::SystemArray::get_element,
        "get_elements", &sdk::SystemArray::get_elements,
        sol::meta_function::index, [](sdk::SystemArray* arr, int32_t index) {
            return arr->get_element(index);
        },
        sol::base_classes, sol::bases<::REManagedObject>()
    );

    create_managed_object_ptr_gc((sdk::SystemArray*)nullptr);
    
    lua.new_usertype<api::sdk::ValueType>("ValueType",
        sol::meta_function::construct, sol::constructors<api::sdk::ValueType(sdk::RETypeDefinition*)>(),
        "data", &api::sdk::ValueType::data,
        "type", &api::sdk::ValueType::type,
        "write_byte", &api::sdk::ValueType::write_memory<uint8_t>,
        "write_short", &api::sdk::ValueType::write_memory<uint16_t>,
        "write_dword", &api::sdk::ValueType::write_memory<uint32_t>,
        "write_qword", &api::sdk::ValueType::write_memory<uint64_t>,
        "write_float", &api::sdk::ValueType::write_memory<float>,
        "write_double", &api::sdk::ValueType::write_memory<double>,
        "read_byte", &api::sdk::ValueType::read_memory<uint8_t>,
        "read_short", &api::sdk::ValueType::read_memory<uint16_t>,
        "read_dword", &api::sdk::ValueType::read_memory<uint32_t>,
        "read_qword", &api::sdk::ValueType::read_memory<uint64_t>,
        "read_float", &api::sdk::ValueType::read_memory<float>,
        "read_double", &api::sdk::ValueType::read_memory<double>,
        "address", &api::sdk::ValueType::address,
        "call", &api::sdk::ValueType::call,
        "get_field", &api::sdk::ValueType::get_field,
        "set_field", &api::sdk::ValueType::set_field,
        "get_type_definition", [](api::sdk::ValueType* b) { return b->type; }
    );
}
