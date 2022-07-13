#include <cstdint>
#include <concepts>

#include <hde64.h>

#include "HookManager.hpp"
#include "sdk/REContext.hpp"
#include "sdk/REManagedObject.hpp"
#include "sdk/RETypeDB.hpp"
#include "sdk/SceneManager.hpp"
#include "sdk/ResourceManager.hpp"
#include "sdk/MotionFsm2Layer.hpp"
#include "sdk/TDBVer.hpp"
#include "utility/Memory.hpp"

#include "../ScriptRunner.hpp"
#include <lstate.h> // weird include order because of sol
#include <lgc.h>

#include "Sdk.hpp"

namespace api {
namespace sdk {
static std::unordered_map<::sdk::RETypeDefinition*, uint32_t> s_fnv_cache{};

struct BehaviorTreeCoreHandle : public ::REManagedObject {
    int unused;
};

struct BehaviorTree : public ::REManagedObject {
    int unused;
    int unused2;
};
}
}

namespace detail {
constexpr uintptr_t FAKE_OBJECT_ADDR = 12345;
}

namespace api::cached_usertype {
namespace detail {
    
}
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

::REManagedObject* add_ref_permanent(sol::this_state s, ::REManagedObject* obj) {
    if (!utility::re_managed_object::is_managed_object(obj)) {
        throw sol::error{(std::stringstream{} << "add_ref_permanent: " << (uintptr_t)obj << " is not a managed object").str()};
    } 

    utility::re_managed_object::add_ref(obj);

    return obj;
}

void release(sol::this_state s, ::REManagedObject* obj, bool force = false) {
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
        if (force && utility::re_managed_object::is_managed_object(obj)) {
            utility::re_managed_object::release(obj);
        }

        // ephemeral counts don't actually release the object, they just decrement the count.
        int new_ephemeral_count = *ephemeral_count - 1;

        if (new_ephemeral_count == 0) {
            objects[(uintptr_t)obj] = sol::make_object(l, sol::nil);
            ephemeral_counts[(uintptr_t)obj] = sol::make_object(l, sol::nil);
        } else {
            ephemeral_counts[(uintptr_t)obj] = new_ephemeral_count;
        }
    } else {
        if (force) {
            if (utility::re_managed_object::is_managed_object(obj)) {
                utility::re_managed_object::release(obj);
            }
        } else {
            spdlog::warn("REManagedObject:release attempted to release an object that was not managed by our Lua state");
        }
    }
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
                api::re_managed_object::detail::add_ref(l, (::REManagedObject*)obj, false);
            }

            int32_t backpedal = 0;

            if ((uintptr_t)obj != detail::FAKE_OBJECT_ADDR) {
                uint32_t typename_hash = 0;
                const auto td = utility::re_managed_object::get_type_definition(obj);

                if (td != nullptr) {
                    if (auto it = api::sdk::s_fnv_cache.find(td); it != api::sdk::s_fnv_cache.end()) {
                        typename_hash = it->second;
                    } else {
                        typename_hash = utility::hash(td->get_full_name());
                        api::sdk::s_fnv_cache[td] = typename_hash;
                    }

                    switch (typename_hash) {
                    case "via.Transform"_fnv:
                        backpedal = sol::stack::push<sol::detail::as_pointer_tag<std::remove_pointer_t<::RETransform>>>(l, (::RETransform*)obj);
                        break;
                    case "via.behaviortree.BehaviorTree"_fnv:[[fallthrough]];
                    case "via.motion.MotionFsm2"_fnv:[[fallthrough]];
                    case "via.motion.MotionJackFsm2"_fnv:[[fallthrough]];
                    case "snow.PlayerMotionFsm"_fnv:
                        backpedal = sol::stack::push<sol::detail::as_pointer_tag<std::remove_pointer_t<api::sdk::BehaviorTree>>>(l, (api::sdk::BehaviorTree*)obj);
                        break;
                    case "via.behaviortree.BehaviorTree.CoreHandle"_fnv: [[fallthrough]];
                    case "via.motion.MotionFsm2Layer"_fnv: [[fallthrough]];
                    case "via.timeline.TimelineFsm2Layer"_fnv:
                        backpedal = sol::stack::push<sol::detail::as_pointer_tag<std::remove_pointer_t<api::sdk::BehaviorTreeCoreHandle>>>(l, (api::sdk::BehaviorTreeCoreHandle*)obj);
                        break;
                    default:
                        backpedal = sol::stack::push<sol::detail::as_pointer_tag<std::remove_pointer_t<T>>>(l, obj);
                        break;
                    };
                } else {
                    backpedal = sol::stack::push<sol::detail::as_pointer_tag<std::remove_pointer_t<T>>>(l, obj);
                }

                auto ref = sol::stack::get<sol::object>(l, -backpedal);

                // keep a weak reference to the object for caching
                objects[(uintptr_t)obj] = ref;

                return backpedal;
            } else {
                backpedal = sol::stack::push<sol::detail::as_pointer_tag<std::remove_pointer_t<T>>>(l, obj);
            }

            return backpedal;
        }
    }

    return sol::stack::push(l, sol::nil);
}

// specialization for any custom usertype we exposed to sol to automatically add a reference
// when lua pushes a pointer to the object onto the stack
template<detail::CachedUserType T>
int sol_lua_push(sol::types<T*>, lua_State* l, T* obj) {
    if (obj != nullptr) {
        // this is a weak table, so we don't need to set the index to nil when the object is no longer referenced
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
            int32_t backpedal = sol::stack::push<sol::detail::as_pointer_tag<std::remove_pointer_t<T>>>(l, obj);

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
sol::object get_field_or_method(sol::object obj, const char* name);
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

    ValueType(const void* raw_data, size_t raw_data_size) {
        if (raw_data_size > 0 && raw_data != nullptr) {
            data.resize(raw_data_size);
            memcpy(data.data(), raw_data, raw_data_size);
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

struct MemoryView {
    uint8_t* data{nullptr};
    size_t size{0};

    MemoryView(uint8_t* d, size_t s)
        : data(d)
        , size(s)
    {
    }

    template <typename T>
    bool is_valid_offset(int32_t offset, T& value) const {
        return offset >= 0 && offset + sizeof(T) <= (int32_t)size;
    }

    bool is_valid_offset(int32_t offset) const {
        return offset >= 0 && offset <= (int32_t)size;
    }

    template <typename T>
    void write_memory(int32_t offset, T value) {
        if (!is_valid_offset(offset, value)) {
            return;
        }

        *(T*)((uintptr_t)data + offset) = value;
    }

    template <typename T>
    T read_memory(int32_t offset) {
        if (!is_valid_offset(offset)) {
            return {};
        }

        return *(T*)((uintptr_t)data + offset);
    }

    uintptr_t address() const {
        return (uintptr_t)data;
    }
};

void* get_thread_context() {
    return (void*)::sdk::get_thread_context();
}

auto find_type_definition(const char* name) {
    return ::sdk::find_type_definition(name);
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

auto get_managed_singleton(sol::this_state s, const char* name) {
    if (name == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    auto out = ::sdk::get_managed_singleton<::REManagedObject>(name);

    if (out == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, out);
}

sol::object create_managed_string(sol::this_state s, const char* text) {
    if (text == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    auto new_str = ::sdk::VM::create_managed_string(utility::widen(text));

    if (new_str == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_str);
}

sol::object create_managed_array(sol::this_state s, sol::object t_obj, uint32_t length) {
    ::REManagedObject* t{nullptr};

    if (t_obj.is<::REManagedObject*>()) {
        t = t_obj.as<::REManagedObject*>();
    } else if (t_obj.is<::sdk::RETypeDefinition*>()) {
        t = t_obj.as<::sdk::RETypeDefinition*>()->get_runtime_type();
    } else if (t_obj.is<const char*>()) {
        const auto tdef = ::sdk::find_type_definition(t_obj.as<const char*>());

        if (tdef != nullptr) {
            t = tdef->get_runtime_type();
        }
    } else {
        throw sol::error("Invalid type passed to create_managed_array. Must be a runtime type, a type definition, or a type name.");
    }

    if (t == nullptr) {
        throw sol::error("Type passed to create_managed_array resolved to null.");
    }

    auto out = ::sdk::VM::create_managed_array(t, length);

    if (out == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::sdk::SystemArray*)out);
}

sol::object create_sbyte(sol::this_state s, int8_t value) {
    auto new_obj = ::sdk::VM::create_sbyte(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_byte(sol::this_state s, uint8_t value) {
    auto new_obj = ::sdk::VM::create_byte(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_int16(sol::this_state s, int16_t value)  {
    auto new_obj = ::sdk::VM::create_int16(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_uint16(sol::this_state s, uint16_t value)  {
    auto new_obj = ::sdk::VM::create_uint16(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_int32(sol::this_state s, int32_t value)  {
    auto new_obj = ::sdk::VM::create_int32(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_uint32(sol::this_state s, int32_t value)  {
    auto new_obj = ::sdk::VM::create_uint32(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_int64(sol::this_state s, int64_t value)  {
    auto new_obj = ::sdk::VM::create_int64(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_uint64(sol::this_state s, int64_t value)  {
    auto new_obj = ::sdk::VM::create_uint64(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_single(sol::this_state s, float value)  {
    auto new_obj = ::sdk::VM::create_single(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_double(sol::this_state s, double value)  {
    auto new_obj = ::sdk::VM::create_double(value);

    if (new_obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, (::REManagedObject*)new_obj);
}

sol::object create_resource(sol::this_state s, std::string type_name, std::string name) {
    auto& types = reframework::get_types();

    // NOT a type definition!!
    // this is a via::typeinfo::TypeInfo
    const auto t = types->get(type_name);

    if (t == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    auto resource_manager = ::sdk::ResourceManager::get();
    return sol::make_object(s, resource_manager->create_resource(t, utility::widen(name)));
}

sol::object create_instance(sol::this_state s, const char* name, sol::object simplify_obj) {
    bool simplify = false;

    if (simplify_obj.is<bool>()) {
        simplify = simplify_obj.as<bool>();
    }

    if (name == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    auto type_definition = find_type_definition(name);

    if (type_definition == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    return sol::make_object(s, type_definition->create_instance_full(simplify));
}

void* get_real_obj(sol::object obj) {
    void* real_obj = nullptr;

    if (!obj.is<sol::nil_t>()) {
        if (obj.is<REManagedObject*>()) {
            real_obj = (void*)obj.as<REManagedObject*>();
        } else if (obj.is<::sdk::Resource*>()) {
            real_obj = (void*)obj.as<::sdk::Resource*>();
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
            static auto object_ref_type = ::sdk::find_type_definition("via.GameObjectRef");
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
            args.push_back(::sdk::VM::create_managed_string(utility::widen(s)));
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

void hook(sol::this_state s, ::sdk::REMethodDefinition* fn, sol::protected_function pre_cb, sol::protected_function post_cb, sol::object ignore_jmp_object) {
    auto sol_state = sol::state_view{s};
    auto state = sol_state.registry()["state"].get<ScriptState*>();
    state->add_hook(fn, pre_cb, post_cb, ignore_jmp_object);
}

void hook_vtable(sol::this_state s, ::REManagedObject* obj, ::sdk::REMethodDefinition* fn, sol::protected_function pre_cb, sol::protected_function post_cb) {
    if (obj == nullptr) {
        throw sol::error("Object is null");
        return;
    }
    
    auto sol_state = sol::state_view{s};
    auto state = sol_state.registry()["state"].get<ScriptState*>();
    state->add_vtable(obj, fn, pre_cb, post_cb);
}
}

namespace api::re_managed_object {
sol::object index(sol::this_state s, REManagedObject* obj, const char* name) {
    if (obj == nullptr) {
        return sol::make_object(s, sol::nil);
    }

    auto type_def = utility::re_managed_object::get_type_definition(obj);
    auto field = type_def->get_field("name");
    if (field != nullptr) {
        return api::sdk::get_native_field_from_field(sol::make_object(s, obj), type_def, field);
    }
    auto method = type_def->get_method(name);
    if (method != nullptr) {
        return sol::make_object(s, method);
    }
    return sol::make_object(s, sol::nil);
}

void new_index(sol::this_state s, REManagedObject* obj, const char* name, sol::object value) {
    if (obj == nullptr) {
        return;
    }

    return api::sdk::set_native_field(sol::make_object(s, obj), utility::re_managed_object::get_type_definition(obj), name, value); 
}

bool is_valid_offset(::REManagedObject* obj, int32_t offset) {
    if (obj == nullptr || !::utility::re_managed_object::is_managed_object(obj)) {
        return false;
    }

    const auto typedefinition = ::utility::re_managed_object::get_type_definition(obj);

    if (typedefinition == nullptr) {
        return false;
    }

    auto size = typedefinition->get_size();

    if (typedefinition->is_array()) {
        size = utility::re_managed_object::get_size(obj);
    }

    // trying to limit the impact of this function.
    if (offset < -0x1000 || (uint32_t)offset >= size) {
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
    sdk["get_tdb_version"] = []() -> int { return sdk::RETypeDB::get()->version; };
    sdk["game_namespace"] = game_namespace;
    sdk["get_thread_context"] = api::sdk::get_thread_context;
    sdk["get_native_singleton"] = api::sdk::get_native_singleton;
    sdk["get_managed_singleton"] = api::sdk::get_managed_singleton;
    sdk["create_managed_string"] = api::sdk::create_managed_string;
    sdk["create_managed_array"] = api::sdk::create_managed_array;
    sdk["create_sbyte"] = api::sdk::create_sbyte;
    sdk["create_byte"] = api::sdk::create_byte;
    sdk["create_int16"] = api::sdk::create_int16;
    sdk["create_uint16"] = api::sdk::create_uint16;
    sdk["create_int32"] = api::sdk::create_int32;
    sdk["create_uint32"] = api::sdk::create_uint32;
    sdk["create_int64"] = api::sdk::create_int64;
    sdk["create_uint64"] = api::sdk::create_uint64;
    sdk["create_single"] = api::sdk::create_single;
    sdk["create_double"] = api::sdk::create_double;
    sdk["create_resource"] = api::sdk::create_resource;
    sdk["create_instance"] = api::sdk::create_instance;
    sdk["find_type_definition"] = api::sdk::find_type_definition;
    sdk["typeof"] = api::sdk::typeof;
    sdk["call_native_func"] = api::sdk::call_native_func;
    sdk["call_object_func"] = api::sdk::call_object_func;
    sdk["get_native_field"] = api::sdk::get_native_field;
    sdk["set_native_field"] = api::sdk::set_native_field;
    sdk["get_primary_camera"] = api::sdk::get_primary_camera;
    sdk["hook"] = api::sdk::hook;
    sdk["hook_vtable"] = api::sdk::hook_vtable;
    sdk.new_enum("PreHookResult", "CALL_ORIGINAL", HookManager::PreHookResult::CALL_ORIGINAL, "SKIP_ORIGINAL", HookManager::PreHookResult::SKIP_ORIGINAL);
    sdk["is_managed_object"] = api::sdk::is_managed_object;
    sdk["to_managed_object"] = [](sol::this_state s, sol::object ptr) { 
        if (ptr.is<::REManagedObject*>()) {
            // no need to convert to managed object.
            return sol::make_object(s, ptr.as<::REManagedObject*>());
        } else if (ptr.is<uintptr_t>()) {
            return sol::make_object(s, (::REManagedObject*)ptr.as<uintptr_t>());
        } else if (ptr.is<void*>()) {
            return sol::make_object(s, (::REManagedObject*)ptr.as<void*>());
        } else {
            //throw sol::error("Object passed was not a managed object, uintptr_t, or void*.");
            return sol::make_object(s, sol::nil);
        }
    };
    sdk["to_resource"] = [](sol::this_state s, void* ptr) { return sol::make_object(s, (::sdk::Resource*)ptr); };
    sdk["to_double"] = [](void* ptr) { return *(double*)&ptr; };
    sdk["to_float"] = [](void* ptr) { return *(float*)&ptr; };
    sdk["to_int64"] = [](void* ptr) { return *(int64_t*)&ptr; };
    sdk["to_ptr"] = [](sol::object obj) {
        if (obj.is<int64_t>()) {
            const auto n = obj.as<int64_t>();
            return *(void**)&n;
        } else if (obj.is<::REManagedObject*>()) {
            return (void*)obj.as<::REManagedObject*>();
        } else if (obj.is<double>()) {
            const auto n = obj.as<double>();
            return *(void**)&n;
        } else if (obj.is<bool>()) {
            const auto n = (uintptr_t)obj.as<bool>();
            return *(void**)&n;
        } else {
            return obj.as<void*>();
        }
    };
    sdk["float_to_ptr"] = [](float f) {
        uintptr_t n = *(uintptr_t*)&f;
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
        "is_enum", &::sdk::RETypeDefinition::is_enum,
        "is_array", &::sdk::RETypeDefinition::is_array,
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

    auto method_call = [](sdk::REMethodDefinition* def, sol::object obj, sol::variadic_args va) {
        auto l = va.lua_state();

        auto real_obj = ::api::sdk::get_real_obj(obj);
        auto ret_val = def->invoke(real_obj, ::api::sdk::build_args(va));

        if (ret_val.exception_thrown) {
            throw sol::error("Invoke threw an exception");
        }

        // Convert return values to the correct Lua types.
        auto ret_ty = def->get_return_type();

        return ::api::sdk::parse_data(l, &ret_val, ret_ty, true);
    };
    
    lua.new_usertype<sdk::REMethodDefinition>("REMethodDefinition",
        sol::meta_function::call, method_call,
        "get_name", &sdk::REMethodDefinition::get_name,
        "get_return_type", &sdk::REMethodDefinition::get_return_type,
        "get_function", &sdk::REMethodDefinition::get_function,
        "get_declaring_type", &sdk::REMethodDefinition::get_declaring_type,
        "get_num_params", &sdk::REMethodDefinition::get_num_params,
        "get_param_types", &sdk::REMethodDefinition::get_param_types,
        "get_param_names", &sdk::REMethodDefinition::get_param_names,
        "is_static", &sdk::REMethodDefinition::is_static,
        "call", method_call
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
        sol::meta_function::index, &api::re_managed_object::index,
        sol::meta_function::new_index, &api::re_managed_object::new_index,
        "add_ref", &api::re_managed_object::add_ref,
        "add_ref_permanent", &api::re_managed_object::add_ref_permanent,
        "force_release", [](sol::this_state s, ::REManagedObject* obj) {
            api::re_managed_object::release(s, obj, true);
        },
        "release", [](sol::this_state s, ::REManagedObject* obj) {
            api::re_managed_object::release(s, obj, false);
        },
        "get_reference_count", [] (::REManagedObject* obj) { return obj->referenceCount; },
        "get_address", [](REManagedObject* obj) { return (uintptr_t)obj; },
        "get_type_definition", &utility::re_managed_object::get_type_definition,
        "get_field", [s](REManagedObject* obj, const char* name) {
            if (obj == nullptr) {
                return sol::make_object(s->lua(), sol::nil);
            }

            return api::sdk::get_native_field(sol::make_object(s->lua(), obj), utility::re_managed_object::get_type_definition(obj), name); 
        },
        "get_object_size", &utility::re_managed_object::get_size,
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
            return sol::make_object(s->lua(), (T*)detail::FAKE_OBJECT_ADDR);
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
        sol::base_classes, sol::bases<::REManagedObject>(),
        sol::meta_function::index, &api::re_managed_object::index,
        sol::meta_function::new_index, &api::re_managed_object::new_index
    );

    create_managed_object_ptr_gc((::REComponent*)nullptr);

    lua.new_usertype<RETransform>("RETransform",
        sol::meta_function::index, &api::re_managed_object::index,
        sol::meta_function::new_index, &api::re_managed_object::new_index,
        "calculate_base_transform", &utility::re_transform::calculate_base_transform,
        "calculate_tpose_pos_world", &utility::re_transform::calculate_tpose_pos_world,
        "apply_joints_tpose", [](RETransform* t, sol::object joints, uint32_t additional_parents) {
            if (t == nullptr) {
                return;
            }

            if (!joints.is<sol::table>()) {
                throw sol::error("RETransform:apply_joints_tpose expected a table of joints");
                return;
            }

            std::vector<::REJoint*> joints_vec{};
            auto joint_tbl = joints.as<sol::table>();

            // convert sol::table to std::vector
            for (auto i = 1; i <= joint_tbl.size(); i++) {
                auto joint = joint_tbl.get<::REManagedObject*>(i);
                joints_vec.push_back((::REJoint*)joint);
            }

            utility::re_transform::apply_joints_tpose(*t, joints_vec, additional_parents);
        },
        "set_position", &sdk::set_transform_position,
        "set_rotation", &sdk::set_transform_rotation,
        "get_position", &sdk::get_transform_position,
        "get_rotation", &sdk::get_transform_rotation,
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
        sol::meta_function::new_index, [](sdk::SystemArray* arr, int32_t index, ::REManagedObject* value) {
            arr->set_element(index, value);
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
        "get_address", &api::sdk::ValueType::address,
        "call", &api::sdk::ValueType::call,
        "get_field", &api::sdk::ValueType::get_field,
        "set_field", &api::sdk::ValueType::set_field,
        "get_type_definition", [](api::sdk::ValueType* b) { return b->type; }
    );

    lua.new_usertype<api::sdk::MemoryView>("MemoryView",
        "write_byte", &api::sdk::MemoryView::write_memory<uint8_t>,
        "write_short", &api::sdk::MemoryView::write_memory<uint16_t>,
        "write_dword", &api::sdk::MemoryView::write_memory<uint32_t>,
        "write_qword", &api::sdk::MemoryView::write_memory<uint64_t>,
        "write_float", &api::sdk::MemoryView::write_memory<float>,
        "write_double", &api::sdk::MemoryView::write_memory<double>,
        "read_byte", &api::sdk::MemoryView::read_memory<uint8_t>,
        "read_short", &api::sdk::MemoryView::read_memory<uint16_t>,
        "read_dword", &api::sdk::MemoryView::read_memory<uint32_t>,
        "read_qword", &api::sdk::MemoryView::read_memory<uint64_t>,
        "read_float", &api::sdk::MemoryView::read_memory<float>,
        "read_double", &api::sdk::MemoryView::read_memory<double>,
        "address", &api::sdk::MemoryView::address,
        "get_address", &api::sdk::MemoryView::address
    );

    lua.new_usertype<::sdk::Resource>("REResource",
        "add_ref", [](sol::this_state s, ::sdk::Resource* res) { 
            res->add_ref();

            return sol::make_object(s, res);
        },
        "release", [](::sdk::Resource* res) {
            res->release();
        },
        "get_address", [](::sdk::Resource* res) { return (uintptr_t)res; }
    );

#define DYNAMIC_ARRAY_TYPE(C, T, name) \
    lua.new_usertype< C < T >>( name , \
        "as_memoryview", [](C < T >& data) {\
            return api::sdk::MemoryView((uint8_t*)&data, sizeof(C < T >));\
        },\
        "empty", & C < T >::empty, \
        "emplace", & C < T >::emplace, \
        "push_back", [](C < T >& arr, T value) { arr.push_back(value); },\
        "pop_back", & C < T >::pop_back,\
        "size", & C < T >::size,\
        "get_size", & C < T >::size,\
        "erase", & C < T >::erase,\
        "clear", & C < T >::clear,\
        sol::meta_function::index, [](C < T >& arr, uint32_t i) -> T {\
            return arr[i];\
        },\
        sol::meta_function::new_index, [](C < T >& arr, uint32_t i, T value) {\
            arr[i] = value;\
        },\
        sol::meta_function::length, &C< T >::size\
    )

#define DYNAMIC_ARRAY_TYPE_REF(C, T, name) \
    lua.new_usertype< C < T >>( name , \
        "as_memoryview", [](C < T >& data) {\
            return api::sdk::MemoryView((uint8_t*)&data, sizeof(C < T >));\
        },\
        "empty", & C < T >::empty, \
        "emplace", & C < T >::emplace, \
        "push_back", [](C < T >& arr, T* value) { arr.push_back(*value); },\
        "pop_back", & C < T >::pop_back,\
        "size", & C < T >::size,\
        "get_size", & C < T >::size,\
        "erase", & C < T >::erase,\
        "clear", & C < T >::clear,\
        sol::meta_function::index, [](C < T >& arr, uint32_t i) -> T* {\
            return &arr[i];\
        },\
        sol::meta_function::new_index, [](C < T >& arr, uint32_t i, T* value) {\
            arr[i] = *value;\
        },\
        sol::meta_function::length, &C< T >::size\
    )


#define DYNAMIC_ARRAY_TYPE_PTR(C, T, name) \
    lua.new_usertype< C < T >>( name , \
        "as_memoryview", [](C < T >& data) {\
            return api::sdk::MemoryView((uint8_t*)&data, sizeof(C < T >));\
        },\
        "empty", & C < T >::empty, \
        "emplace", & C < T >::emplace, \
        "push_back", [](C < T >& arr, T value) { arr.push_back(value); },\
        "pop_back", & C < T >::pop_back,\
        "size", & C < T >::size,\
        "get_size", & C < T >::size,\
        "erase", & C < T >::erase,\
        "clear", & C < T >::clear,\
        sol::meta_function::index, [](C < T >& arr, uint32_t i) -> T {\
            return arr[i];\
        },\
        sol::meta_function::new_index, [](C < T >& arr, uint32_t i, T value) {\
            arr[i] = value;\
        },\
        sol::meta_function::length, &C< T >::size\
    )

#define DYNAMIC_ARRAY_NOCAP_TYPE(T, name) DYNAMIC_ARRAY_TYPE(sdk::NativeArrayNoCapacity, T, name)
#define DYNAMIC_ARRAY_NOCAP_TYPE_REF(T, name) DYNAMIC_ARRAY_TYPE_REF(sdk::NativeArrayNoCapacity, T, name)
#define DYNAMIC_ARRAY_CAP_TYPE_PTR(T, name) DYNAMIC_ARRAY_TYPE_PTR(sdk::NativeArray, T, name)

    DYNAMIC_ARRAY_NOCAP_TYPE(uint8_t, "DynamicArrayNoCapacityUInt8");
    DYNAMIC_ARRAY_NOCAP_TYPE(int32_t, "DynamicArrayNoCapacityInt32");
    DYNAMIC_ARRAY_NOCAP_TYPE(uint32_t, "DynamicArrayNoCapacityUInt32");

    DYNAMIC_ARRAY_NOCAP_TYPE_REF(sdk::NativeArrayNoCapacity<uint32_t>, "DynamicArrayNoCapacityUInt32Array");

    lua.new_usertype<::sdk::behaviortree::TreeNodeData>("BehaviorTreeNodeData",
        "as_memoryview", [](::sdk::behaviortree::TreeNodeData* data) {
            return api::sdk::MemoryView((uint8_t*)data, sizeof(::sdk::behaviortree::TreeNodeData));
        },
        "to_valuetype", [](::sdk::behaviortree::TreeNodeData* data) {
            return *data;
        },
        "id", &::sdk::behaviortree::TreeNodeData::id,
        "parent", &::sdk::behaviortree::TreeNodeData::parent,
        "is_branch", &::sdk::behaviortree::TreeNodeData::is_branch,
        "is_end", &::sdk::behaviortree::TreeNodeData::is_end,
        "has_selector", &::sdk::behaviortree::TreeNodeData::has_selector,
        //"selector_id", &::sdk::behaviortree::TreeNodeData::selector_id,
        "attr", &::sdk::behaviortree::TreeNodeData::attr,
        "parent", &::sdk::behaviortree::TreeNodeData::parent,
        "parent_2", &::sdk::behaviortree::TreeNodeData::parent_2,
        "get_children", &::sdk::behaviortree::TreeNodeData::get_children,
        "get_actions", &::sdk::behaviortree::TreeNodeData::get_actions,
        "get_states", &::sdk::behaviortree::TreeNodeData::get_states,
        "get_states_2", &::sdk::behaviortree::TreeNodeData::get_states_2,
        "get_start_states", &::sdk::behaviortree::TreeNodeData::get_start_states,
        "get_start_transitions", &::sdk::behaviortree::TreeNodeData::get_start_transitions,
        "get_conditions", &::sdk::behaviortree::TreeNodeData::get_conditions,
        "get_transition_conditions", &::sdk::behaviortree::TreeNodeData::get_transition_conditions,
        "get_transition_events", &::sdk::behaviortree::TreeNodeData::get_transition_events,
        "get_transition_ids", &::sdk::behaviortree::TreeNodeData::get_transition_ids,
        "get_transition_attributes", &::sdk::behaviortree::TreeNodeData::get_transition_attributes,
        "get_tags", &::sdk::behaviortree::TreeNodeData::get_tags
    );

    lua.new_usertype<::sdk::behaviortree::TreeNode>("BehaviorTreeNode",
        "as_memoryview", [](::sdk::behaviortree::TreeNode* node) {
            return api::sdk::MemoryView((uint8_t*)node, sizeof(::sdk::behaviortree::TreeNode));
        },
        "to_valuetype", [](::sdk::behaviortree::TreeNode* node) {
            return *node;
        },
        "get_id", &::sdk::behaviortree::TreeNode::get_id,
        "get_data", &::sdk::behaviortree::TreeNode::get_data,
        "get_owner", &::sdk::behaviortree::TreeNode::get_owner,
        "get_parent", &::sdk::behaviortree::TreeNode::get_parent,
        "get_name", &::sdk::behaviortree::TreeNode::get_name,
        "get_full_name", &::sdk::behaviortree::TreeNode::get_full_name,
        "get_children", &::sdk::behaviortree::TreeNode::get_children,
        "get_actions", &::sdk::behaviortree::TreeNode::get_actions,
        "get_unloaded_actions", &::sdk::behaviortree::TreeNode::get_unloaded_actions,
        "get_conditions", &::sdk::behaviortree::TreeNode::get_conditions,
        "get_transition_conditions", &::sdk::behaviortree::TreeNode::get_transition_conditions,
        "get_transition_events", &::sdk::behaviortree::TreeNode::get_transition_events,
        "get_states", &::sdk::behaviortree::TreeNode::get_states,
        "get_start_states", &::sdk::behaviortree::TreeNode::get_start_states,
        "get_status1", &::sdk::behaviortree::TreeNode::get_status1,
        "get_status2", &::sdk::behaviortree::TreeNode::get_status2,
        "relocate", &::sdk::behaviortree::TreeNode::relocate
    );

    DYNAMIC_ARRAY_NOCAP_TYPE_REF(::sdk::behaviortree::TreeNode, "DynamicArrayNoCapacityTreeNode");
    DYNAMIC_ARRAY_NOCAP_TYPE_REF(::sdk::behaviortree::TreeNodeData, "DynamicArrayNoCapacityTreeNodeData");
    DYNAMIC_ARRAY_CAP_TYPE_PTR(::REManagedObject*, "DynamicArrayManagedObject");

    lua.new_usertype<::sdk::behaviortree::TreeObjectData>("BehaviorTreeObjectData",
        "as_memoryview", [](::sdk::behaviortree::TreeObjectData* data) {
            return api::sdk::MemoryView((uint8_t*)data, sizeof(::sdk::behaviortree::TreeObjectData));
        },
        "get_nodes", &::sdk::behaviortree::TreeObjectData::get_nodes,
        "get_static_actions", &::sdk::behaviortree::TreeObjectData::get_static_actions,
        "get_static_conditions", &::sdk::behaviortree::TreeObjectData::get_static_conditions,
        "get_static_transitions", &::sdk::behaviortree::TreeObjectData::get_static_transitions,
        "get_action_methods", &::sdk::behaviortree::TreeObjectData::get_action_methods,
        "get_static_action_methods", &::sdk::behaviortree::TreeObjectData::get_static_action_methods
    );

    lua.new_usertype<::sdk::behaviortree::TreeObject>("BehaviorTreeObject",
        "as_memoryview", [](::sdk::behaviortree::TreeObject* obj) {
            return api::sdk::MemoryView((uint8_t*)obj, sizeof(::sdk::behaviortree::TreeObject));
        },
        "get_data", &::sdk::behaviortree::TreeObject::get_data,
        "get_node_by_id", &::sdk::behaviortree::TreeObject::get_node_by_id,
        "get_node_by_name", [](::sdk::behaviortree::TreeObject* obj, const char* name) {
            return obj->get_node_by_name(name);
        },
        "get_actions", &::sdk::behaviortree::TreeObject::get_action_array,
        "get_nodes", &::sdk::behaviortree::TreeObject::get_node_array,
        "get_conditions", &::sdk::behaviortree::TreeObject::get_condition_array,
        "get_transitions", &::sdk::behaviortree::TreeObject::get_transition_array,
        "get_selectors", &::sdk::behaviortree::TreeObject::get_selector_array,
        "get_node", &::sdk::behaviortree::TreeObject::get_node,
        "get_node_count", &::sdk::behaviortree::TreeObject::get_node_count,
        "get_action", &::sdk::behaviortree::TreeObject::get_action,
        "get_unloaded_action", &::sdk::behaviortree::TreeObject::get_unloaded_action,
        "get_transition", &::sdk::behaviortree::TreeObject::get_transition,
        "get_condition", &::sdk::behaviortree::TreeObject::get_condition,
        "get_action_count", &::sdk::behaviortree::TreeObject::get_action_count,
        "get_condition_count", &::sdk::behaviortree::TreeObject::get_condition_count,
        "get_transition_count", &::sdk::behaviortree::TreeObject::get_transition_count,
        "get_unloaded_action_count", &::sdk::behaviortree::TreeObject::get_unloaded_action_count,
        "get_static_action_count", &::sdk::behaviortree::TreeObject::get_static_action_count,
        "get_static_condition_count", &::sdk::behaviortree::TreeObject::get_static_condition_count,
        "get_static_transition_count", &::sdk::behaviortree::TreeObject::get_static_transition_count,
        "relocate", &::sdk::behaviortree::TreeObject::relocate
    );

    lua.new_usertype<api::sdk::BehaviorTreeCoreHandle>("BehaviorTreeCoreHandle",
        sol::meta_function::index, &api::re_managed_object::index,
        sol::meta_function::new_index, &api::re_managed_object::new_index,
        sol::base_classes, sol::bases<::REManagedObject>(),
        "get_tree_object", [](api::sdk::BehaviorTreeCoreHandle* handle) {
            return ((sdk::behaviortree::CoreHandle*)handle)->get_tree_object();
        },
        "relocate", [](api::sdk::BehaviorTreeCoreHandle* handle, uintptr_t old_start, uintptr_t old_end, sdk::NativeArrayNoCapacity<sdk::behaviortree::TreeNode>& new_nodes) {
            ((sdk::behaviortree::CoreHandle*)handle)->relocate(old_start, old_end, new_nodes);
        }
    );

    create_managed_object_ptr_gc((api::sdk::BehaviorTreeCoreHandle*)nullptr);

    lua.new_usertype<api::sdk::BehaviorTree>("BehaviorTree",
        sol::meta_function::index, &api::re_managed_object::index,
        sol::meta_function::new_index, &api::re_managed_object::new_index,
        sol::base_classes, sol::bases<::REManagedObject>(),
        "get_tree", [](api::sdk::BehaviorTree* tree, uint32_t index) {
            return ((sdk::behaviortree::BehaviorTree*)tree)->get_tree<api::sdk::BehaviorTreeCoreHandle>(index);
        },
        "get_tree_count", [](api::sdk::BehaviorTree* tree) {
            return ((sdk::behaviortree::BehaviorTree*)tree)->get_tree_count();
        },
        "get_trees", [](api::sdk::BehaviorTree* tree) {
            return ((sdk::behaviortree::BehaviorTree*)tree)->get_trees<api::sdk::BehaviorTreeCoreHandle>();
        }
    );

    create_managed_object_ptr_gc((api::sdk::BehaviorTree*)nullptr);
}
