#include <spdlog/spdlog.h>
#include <utility/Scan.hpp>
#include <utility/Module.hpp>

#include "reframework/API.hpp"
#include "RETypeDB.hpp"

namespace sdk {
RETypeDB* RETypeDB::get() {
    auto vm = VM::get();

    if (vm == nullptr) {
        return nullptr;
    }

    return vm->get_type_db();
}

static std::shared_mutex g_tdb_type_mtx{};
static std::unordered_map<std::string, sdk::RETypeDefinition*> g_tdb_type_map{};

reframework::InvokeRet invoke_object_func(void* obj, sdk::RETypeDefinition* t, std::string_view name, const std::vector<void*>& args) {
    const auto method = t->get_method(name);

    if (method == nullptr) {
        return reframework::InvokeRet{};
    }

    return method->invoke(obj, args);
}

reframework::InvokeRet invoke_object_func(::REManagedObject* obj, std::string_view name, const std::vector<void*>& args) {
   return invoke_object_func((void*)obj, utility::re_managed_object::get_type_definition(obj), name, args);
}

sdk::RETypeDefinition* RETypeDB::find_type(std::string_view name) const {
    {
        std::shared_lock _{ g_tdb_type_mtx };

        if (auto it = g_tdb_type_map.find(name.data()); it != g_tdb_type_map.end()) {
            return it->second;
        }
    }

    for (uint32_t i = 0; i < this->numTypes; ++i) {
        auto t = get_type(i);

        if (t->get_full_name() == name) {
            std::unique_lock _{ g_tdb_type_mtx };

            g_tdb_type_map[name.data()] = t;
            return g_tdb_type_map[name.data()];
        }
    }

    return g_tdb_type_map[name.data()];
}

sdk::RETypeDefinition* RETypeDB::find_type_by_fqn(uint32_t fqn) const {
    for (uint32_t i = 0; i< this->numTypes; ++i) {
        auto t = get_type(i);

        if (t->get_fqn_hash() == fqn) {
            return t;
        }
    }

    return nullptr;
}

sdk::REMethodDefinition* get_object_method(::REManagedObject* object, std::string_view name) {
    auto t = utility::re_managed_object::get_type_definition(object);

    if (t == nullptr) {
        return nullptr;
    }

    return t->get_method(name);
}


sdk::RETypeDefinition* find_type_definition(std::string_view type_name) {
    auto tdb = sdk::RETypeDB::get();

    if (tdb == nullptr) {
        return nullptr;
    }

    return tdb->find_type(type_name);
}

sdk::RETypeDefinition* find_type_definition_by_fqn(uint32_t fqn) {
    auto tdb = sdk::RETypeDB::get();

    if (tdb == nullptr) {
        return nullptr;
    }

    return tdb->find_type_by_fqn(fqn);
}

sdk::REMethodDefinition* find_method_definition(std::string_view type_name, std::string_view method_name) {
    auto t = find_type_definition(type_name);

    if (t == nullptr) {
        return nullptr;
    }

    return t->get_method(method_name);
}

void* find_native_method(sdk::RETypeDefinition* t, std::string_view method_name) {
    const auto method = t->get_method(method_name);

    if (method == nullptr) {
        // spdlog::error("Cannot find {:s}", method_name.data());
        return nullptr;
    }

    return method->get_function();
}

void* find_native_method(std::string_view type_name, std::string_view method_name) {
    auto t = sdk::find_type_definition(type_name);

    if (t == nullptr) {
        //spdlog::error("Cannot find type {:s}", type_name.data());
        return nullptr;
    }

    return find_native_method(t, method_name);
}

sdk::RETypeDefinition* RETypeDB::get_type(uint32_t index) const {
    index &= get_type_bitmask();

    if (index >= this->numTypes) {
        return nullptr;
    }

    return &(*this->types)[index];
}

sdk::REMethodDefinition* RETypeDB::get_method(uint32_t index) const {
    if (index >= this->numMethods) {
        return nullptr;
    }

    return &(*this->methods)[index];
}

sdk::REField* RETypeDB::get_field(uint32_t index) const {
    if (index >= this->numFields) {
        return nullptr;
    }

    return &(*this->fields)[index];
}

sdk::REProperty* RETypeDB::get_property(uint32_t index) const {
    if (index >= this->numProperties) {
        return nullptr;
    }

    return &(*this->properties)[index];
}

const char* RETypeDB::get_string(uint32_t offset) const {
    offset &= get_string_pool_bitmask();

    if (offset >= this->numStringPool) {
        return nullptr;
    }

    return (const char*)((uintptr_t)this->stringPool + offset);
}

uint8_t* RETypeDB::get_bytes(uint32_t offset) const {
    offset &= get_byte_pool_bitmask();

    if (offset >= this->numBytePool) {
        return nullptr;
    }

    return (uint8_t*)((uintptr_t)this->bytePool + offset);
}
}

namespace sdk {
sdk::RETypeDefinition* REField::get_declaring_type() const {
    auto tdb = RETypeDB::get();

    if (this->declaring_typeid == 0) {
        return nullptr;
    }

    return tdb->get_type(this->declaring_typeid);
}

sdk::RETypeDefinition* REField::get_type() const {
    auto tdb = RETypeDB::get();

#if TDB_VER >= 71
    const auto field_typeid = this->field_typeid;
#elif TDB_VER >= 69
    auto& impl = (*tdb->fieldsImpl)[this->impl_id];
    const auto field_typeid = (uint32_t)impl.field_typeid;
#else
    const auto field_typeid = (uint32_t)this->field_typeid;
#endif

    if (field_typeid == 0) {
        return nullptr;
    }

    return tdb->get_type(field_typeid);
}

const char* REField::get_name() const {
    auto tdb = RETypeDB::get();

#if TDB_VER >= 69
    auto& impl = (*tdb->fieldsImpl)[this->impl_id];
    const auto name_offset = impl.name_offset & tdb->get_string_pool_bitmask();
#else
    const auto name_offset = this->name_offset;
#endif

    return tdb->get_string(name_offset);
}

uint32_t REField::get_flags() const {
    auto tdb = RETypeDB::get();

#if TDB_VER >= 69
    auto& impl = (*tdb->fieldsImpl)[this->impl_id];
    return impl.flags;
#else
    return this->flags;
#endif
}

uint32_t REField::get_init_data_index() const {
    auto tdb = RETypeDB::get();
    
#if TDB_VER >= 71
    auto& impl = (*tdb->fieldsImpl)[this->impl_id];
    const auto init_data_index = impl.init_data_lo | (impl.init_data_mid << 6) | (this->init_data_hi << 10); // what the FUCK!!! IS THIS SHIT!!!
#elif TDB_VER >= 69
    auto& impl = (*tdb->fieldsImpl)[this->impl_id];
    const auto init_data_index = impl.init_data_lo | (impl.init_data_hi << 14);
#elif TDB_VER > 49
    const auto init_data_index = this->init_data_index;
#else
    const auto init_data_index = 0;
#endif

    return init_data_index;
}

void* REField::get_init_data() const {
    auto tdb = RETypeDB::get();

    const auto init_data_index = get_init_data_index();

#if TDB_VER > 49
    const auto init_data_offset = (*tdb->initData)[init_data_index];
#else
    const auto init_data_offset = this->init_data_offset;
#endif

    auto init_data = tdb->get_bytes(init_data_offset);

    // WACKY
    if (init_data_offset < 0) {
        init_data = (uint8_t*)tdb->get_string(init_data_offset * -1);
    }

    return init_data;
}

uint32_t REField::get_offset_from_fieldptr() const {
#if TDB_VER >= 71
    const auto tdb = sdk::RETypeDB::get();
    return (*tdb->fieldsImpl)[this->impl_id].offset;
#else
    return this->offset;
#endif
}

uint32_t REField::get_offset_from_base() const {
    auto offset_from_fieldptr = get_offset_from_fieldptr();
    auto declaring_type = this->get_declaring_type();

    if (declaring_type != nullptr) {
        if (declaring_type->has_fieldptr_offset()) {
            return declaring_type->get_fieldptr_offset() + offset_from_fieldptr;
        }
    }

    return offset_from_fieldptr;
}

bool sdk::REField::is_static() const {
    const auto field_flags = this->get_flags();

    return (field_flags & (uint16_t)::via::clr::FieldFlag::Static) != 0;
}

bool sdk::REField::is_literal() const  {
    const auto field_flags = this->get_flags();

    return (field_flags & (uint16_t)::via::clr::FieldFlag::Literal) != 0;
}

void* REField::get_data_raw(void* object, bool is_value_type) const {
    const auto field_flags = get_flags();

    if ((field_flags & (uint16_t)::via::clr::FieldFlag::Static) != 0) {
        if ((field_flags & (uint16_t)::via::clr::FieldFlag::Literal) != 0) {
            return this->get_init_data();
        }

        auto tbl = sdk::VM::get()->get_static_tbl_for_type(this->get_declaring_type()->get_index());

        if (tbl != nullptr) {
            return Address{tbl}.get(this->get_offset_from_fieldptr());
        }
    } else {
        if (is_value_type) {
            return Address{object}.get(this->get_offset_from_fieldptr());
        }

        return Address{object}.get(this->get_offset_from_base());
    }

    return nullptr;
}
} // namespace sdk

// methods
namespace sdk {
sdk::RETypeDefinition* REMethodDefinition::get_declaring_type() const {
    auto tdb = RETypeDB::get();

    if (this->declaring_typeid == 0) {
        return nullptr;
    }

    return tdb->get_type(this->declaring_typeid);
}

sdk::RETypeDefinition* REMethodDefinition::get_return_type() const {
    auto tdb = RETypeDB::get();
    const auto params_index = get_param_index();

#if TDB_VER >= 69
    auto param_ids = tdb->get_data<sdk::ParamList>(params_index);
    
    const auto return_param_id = param_ids->returnType;
    const auto& p = (*tdb->params)[return_param_id];

    if (p.type_id == 0) {
        return nullptr;
    }

    const auto return_typeid = p.type_id;
#elif TDB_VER >= 66
    const auto return_typeid = (uint32_t)this->return_typeid;
#else
    const auto return_typeid = tdb->get_data<sdk::REMethodParamDef>(this->params)->return_typeid;
#endif

    if (return_typeid == 0) {
        return nullptr;
    }

    return tdb->get_type(return_typeid);
}

const char* REMethodDefinition::get_name() const {
    auto tdb = RETypeDB::get();

#if TDB_VER >= 69
    auto& impl = (*tdb->methodsImpl)[this->impl_id];
    const auto name_offset = impl.name_offset & tdb->get_string_pool_bitmask();
#else
    const auto name_offset = this->name_offset;
#endif

    return tdb->get_string(name_offset);
}

void* REMethodDefinition::get_function() const {
#if TDB_VER >= 71
    if (this->encoded_offset == 0) {
        /*const auto dt = this->get_declaring_type();
        
        if (dt != nullptr) {
            if ((dt->type_flags & (uint32_t)via::clr::TypeFlag::Interface) != 0) {
                spdlog::error("[REMethodDefinition::get_function] Interface methods are not implemented yet (method: {})", this->get_name());
                return nullptr;
            }

            if (dt->managed_vt != nullptr && this->get_virtual_index() != -1) {
                auto tdb = RETypeDB::get();
                const auto& impl = (*tdb->typesImpl)[this->impl_id];

                const auto vt = (void**)(dt->managed_vt + impl.num_native_vtable);
                const auto r = vt[this->get_virtual_index()];

                spdlog::info("[REMethodDefinition] {} Found function {:s} at {:x}", this->get_index(), this->get_name(), (uintptr_t)r);

                return r;
            }
        }*/

        auto decl_type = this->get_declaring_type();
        auto name = decl_type != nullptr ? decl_type->get_full_name() : std::string{"null"};
        spdlog::error("[REMethodDefinition::get_function] Encoded offset is 0 (vindex {}) (method: {}.{})", this->get_virtual_index(), name, this->get_name());
        return nullptr;
    }

    // The function this function uses to offset from has a predictable pattern as well.
    // So the pattern for that can be searched for if this one breaks.
    // It remains to be seen if this "encoding" is used in games other than MHRise.
    static void* (*get_encoded_pointer)(int32_t offset) = []() {
        spdlog::info("[REMethodDefinition] Finding get_encoded_pointer");

        auto fn = utility::scan(utility::get_executable(), "85 C9 75 03 33 C0 C3 48 63 C1 48 8d 0D ? ? ? ? 48 03 C1 C3");

        if (!fn) {
            spdlog::error("[REMethodDefinition] Failed to find get_encoded_pointer");
            return (void* (*)(int32_t))nullptr;
        }

        spdlog::info("[REMethodDefinition] Found get_encoded_pointer at {:x}", *fn);

        return (void* (*)(int32_t))*fn;
    }();

    if (get_encoded_pointer == nullptr) {
        return nullptr;
    }

    auto result = get_encoded_pointer(this->encoded_offset);

    if (result == nullptr) {
        spdlog::error("[REMethodDefinition] Failed to get function from encoded offset, method index: {}", this->get_index());
    }

    return result;
#elif TDB_VER > 49
    return this->function;
#else
    auto vm = sdk::VM::get();
    auto& m = vm->methods[this->get_index()];

    return m.function;
#endif
}

uint32_t sdk::REMethodDefinition::get_invoke_id() const {
    auto tdb = RETypeDB::get();

    const auto params_index = get_param_index();

#if TDB_VER >= 69
    const auto param_list = params_index;
    const auto param_ids = tdb->get_data<sdk::ParamList>(param_list);
    const auto num_params = param_ids->numParams;
    const auto invoke_id = param_ids->invokeID;
#else
    const auto invoke_id = (uint16_t)this->invoke_id;
#endif

    return invoke_id;
}

reframework::InvokeRet sdk::REMethodDefinition::invoke(void* object, const std::vector<void*>& args) const {
    const auto num_params = get_num_params();

    if (num_params != args.size()) {
        //throw std::runtime_error("Invalid number of arguments");
        spdlog::warn("Invalid number of arguments passed to REMethodDefinition::invoke for {}", get_name());
        return reframework::InvokeRet{};
    }

#if TDB_VER > 49
    const auto invoke_tbl = sdk::get_invoke_table();
    auto invoke_wrapper = invoke_tbl[get_invoke_id()];

    struct StackFrame {
        char pad_0000[8+8]; //0x0000
        const sdk::REMethodDefinition* method;
        char pad_0010[24]; //0x0018
        void* in_data; //0x0030 can point to data
        void* out_data; //0x0038 can be whatever, can be a dword, can point to data
        void* object_ptr; //0x0040 aka "this" pointer
    };

    reframework::InvokeRet out{};

    StackFrame stack_frame{};
    stack_frame.method = this;
    stack_frame.object_ptr = object;
    stack_frame.in_data = (void*)args.data();
    
    auto ret_ty = get_return_type();
    bool is_ptr = false;
 
    // vec3 and stuff that is > sizeof(void*) requires special handling
    // by preallocating the output buffer
    if (ret_ty != nullptr && ret_ty->is_value_type()) {
        if (ret_ty->get_valuetype_size() > sizeof(void*) || (!ret_ty->is_primitive() && !ret_ty->is_enum())) {
            stack_frame.out_data = &out;
            is_ptr = false;
        } else {
            stack_frame.out_data = nullptr;
            is_ptr = true;
        }
    } else {
        stack_frame.out_data = nullptr;
        is_ptr = true;
    }
    
    {
        auto context = sdk::get_thread_context();
        sdk::VMContext::ScopedTranslator scoped_translator{context};

        bool corrupted_before_call = context->unkPtr != nullptr && context->unkPtr->unkPtr != nullptr;

        try {
            invoke_wrapper((void*)&stack_frame, context);
            out.exception_thrown = false;

            // exception pointer
            if (context->unkPtr->unkPtr != nullptr) {
                spdlog::error("Internal game exception thrown in REMethodDefinition::invoke for {}", get_name());

                const auto exception_managed_object = (::REManagedObject*)context->unkPtr->unkPtr;

                if (utility::re_managed_object::is_managed_object(exception_managed_object)) {
                    const auto exception_tdb_type = utility::re_managed_object::get_type_definition(exception_managed_object);

                    if (exception_tdb_type != nullptr) {
                        const auto exception_name = exception_tdb_type->get_full_name();
                        spdlog::error(" Exception name: {}", exception_name.data());
                    }
                }

                if (corrupted_before_call) {
                    spdlog::error("VMContext was already corrupted before this call, a previous exception may not have been handled properly");
                }

                out.exception_thrown = true;

                context->unkPtr->unkPtr = nullptr;
            }
        } catch (sdk::VMContext::Exception&) {
            spdlog::error("Exception thrown in REMethodDefinition::invoke for {}", get_name());
            context->cleanup_after_exception(scoped_translator.get_prev_reference_count());
            
            memset(&out, 0, sizeof(out));

            if (!is_ptr) {
                out.ptr = out.bytes.data();
            }

            out.exception_thrown = true;

            return out;
        }
    }

    if (stack_frame.out_data != &out) {
        out.ptr = stack_frame.out_data;
        return out;
    }

    return out;
#else
    // RE7 doesn't have the invoke wrappers that the newer games use...
    if (num_params > 3) {
        spdlog::warn("REMethodDefinition::invoke for {} has more than 2 parameters, which is not supported at this time (RE7)", get_name());
        return reframework::InvokeRet{};
    }

    const bool is_static = this->is_static();

    if (!is_static && object == nullptr) {
        spdlog::warn("REMethodDefinition::invoke for {} is not static, but object is nullptr", get_name());
        return reframework::InvokeRet{};
    }

    auto ret_ty = get_return_type();
    size_t ret_hash = ret_ty != nullptr ? utility::hash(ret_ty->get_full_name()) : 0;
    bool is_ptr = false;
 
    // vec3 and stuff that is > sizeof(void*) requires special handling
    // by preallocating the output buffer
    if (ret_ty != nullptr && ret_ty->is_value_type()) {
        if (ret_ty->get_valuetype_size() > sizeof(void*) || (!ret_ty->is_primitive() && !ret_ty->is_enum())) {
            is_ptr = false;
        } else {
            is_ptr = true;
        }
    } else {
        is_ptr = true;
    }

    reframework::InvokeRet out{};

    const auto param_types = get_param_types();
    std::vector<size_t> param_hashes{};
    std::vector<void*> converted_args(args.size());

    for (auto& ty : param_types) {
        param_hashes.push_back(utility::hash(ty->get_full_name()));
    }

    // convert necessary args to float
    // we pass the args as double, but because there's no invoke wrappers
    // in RE7, we must convert them back to float
    for (size_t i = 0; i < args.size(); i++) {
        auto& arg = args[i];
        auto& ty = param_types[i];
        auto& hash = param_hashes[i];

        switch (hash) {
        case "System.Single"_fnv:
            *(float*)&converted_args[i] = (float)*(double*)&arg;
            break;
        default:
            converted_args[i] = arg;
            break;
        }
    }

    auto unpack_and_call = [&]<typename ...Types>() {
        if constexpr (sizeof...(Types) == 0) {
            if (is_static) {
                if (!is_ptr) {
                    this->call<void*>(out.bytes.data(), sdk::get_thread_context());
                } else {
                    if (ret_hash == "System.Single"_fnv) {
                        out.d = (double)this->call<float>(sdk::get_thread_context());
                    } else if (ret_hash == "System.Double"_fnv) {
                        out.d = this->call<double>(sdk::get_thread_context());
                    } else {
                        out.ptr = this->call<void*>(sdk::get_thread_context());
                    }
                }
            } else {
                if (!is_ptr) {
                    this->call<void*>(out.bytes.data(), sdk::get_thread_context(), object);
                } else {
                    if (ret_hash == "System.Single"_fnv) {
                        out.d = (double)this->call<float>(sdk::get_thread_context(), object);
                    } else if (ret_hash == "System.Double"_fnv) {
                        out.d = this->call<double>(sdk::get_thread_context(), object);
                    } else {
                        out.ptr = this->call<void*>(sdk::get_thread_context(), object);
                    }
                }
            }
        } else if constexpr (sizeof...(Types) > 0) {
            // now we must do the same as above but unpack the converted_args and cast them to the correct type
            if (is_static) {
                if (!is_ptr) {
                    CallHelper<void*, Types...>::create(converted_args.data())(this, out.bytes.data(), sdk::get_thread_context());
                } else {
                    if (ret_hash == "System.Single"_fnv) {
                        out.d = (double)CallHelper<float, Types...>::create(converted_args.data())(this, sdk::get_thread_context());
                    } else if (ret_hash == "System.Double"_fnv) {
                        out.d = CallHelper<double, Types...>::create(converted_args.data())(this, sdk::get_thread_context());
                    } else {
                        out.ptr = CallHelper<void*, Types...>::create(converted_args.data())(this, sdk::get_thread_context());
                    }
                }
            } else {
                if (!is_ptr) {
                    CallHelper<void*, Types...>::create(converted_args.data())(this, out.bytes.data(), sdk::get_thread_context(), object);
                } else {
                    if (ret_hash == "System.Single"_fnv) {
                        out.d = (double)CallHelper<float, Types...>::create(converted_args.data())(this, sdk::get_thread_context(), object);
                    } else if (ret_hash == "System.Double"_fnv) {
                        out.d = CallHelper<double, Types...>::create(converted_args.data())(this, sdk::get_thread_context(), object);
                    } else {
                        out.ptr = CallHelper<void*, Types...>::create(converted_args.data())(this, sdk::get_thread_context(), object);
                    }
                }
            }
        }
    };

    switch (num_params) {
    case 0:
        unpack_and_call();
        return out;

        break;
    case 1:
        // now we must check each parameter to check if it's a float/double
        if (param_hashes[0] == "System.Single"_fnv) {
            unpack_and_call.operator()<float>();
        } else if (param_hashes[0] == "System.Double"_fnv) {
            unpack_and_call.operator()<double>();
        } else {
            unpack_and_call.operator()<void*>();
        }

        return out;

        break;
    case 2:
        // oh god now we need to handle more permutations
        if (param_hashes[0] == "System.Single"_fnv && param_hashes[1] == "System.Single"_fnv) {
            unpack_and_call.operator()<float, float>();
        } else if (param_hashes[0] == "System.Single"_fnv && param_hashes[1] == "System.Double"_fnv) {
            unpack_and_call.operator()<float, double>();
        } else if (param_hashes[0] == "System.Double"_fnv && param_hashes[1] == "System.Single"_fnv) {
            unpack_and_call.operator()<double, float>();
        } else if (param_hashes[0] == "System.Double"_fnv && param_hashes[1] == "System.Double"_fnv) {
            unpack_and_call.operator()<double, double>();
        } else if (param_hashes[0] == "System.Single"_fnv) {
            unpack_and_call.operator()<float, void*>();
        } else if (param_hashes[0] == "System.Double"_fnv) {
            unpack_and_call.operator()<double, void*>();
        } else if (param_hashes[1] == "System.Single"_fnv) {
            unpack_and_call.operator()<void*, float>();
        } else if (param_hashes[1] == "System.Double"_fnv) {
            unpack_and_call.operator()<void*, double>();
        } else {
            unpack_and_call.operator()<void*, void*>();
        }

        return out;

        break;
    case 3:
        // uhhhhhhh now this is just getting ridiculous
        switch (param_hashes[0]) {
        case "System.Single"_fnv:
            switch (param_hashes[1]) {
            case "System.Single"_fnv:
                switch (param_hashes[2]) {
                case "System.Single"_fnv:
                    unpack_and_call.operator()<float, float, float>();
                    break;
                case "System.Double"_fnv:
                    unpack_and_call.operator()<float, float, double>();
                    break;
                default:
                    unpack_and_call.operator()<float, float, void*>();
                    break;
                }
                break;
            case "System.Double"_fnv:
                switch (param_hashes[2]) {
                case "System.Single"_fnv:
                    unpack_and_call.operator()<float, double, float>();
                    break;
                case "System.Double"_fnv:
                    unpack_and_call.operator()<float, double, double>();
                    break;
                default:
                    unpack_and_call.operator()<float, double, void*>();
                    break;
                }
                break;
            default:
                switch (param_hashes[2]) {
                case "System.Single"_fnv:
                    unpack_and_call.operator()<float, void*, float>();
                    break;
                case "System.Double"_fnv:
                    unpack_and_call.operator()<float, void*, double>();
                    break;
                default:
                    unpack_and_call.operator()<float, void*, void*>();
                    break;
                }
                break;
            }
            break;
        case "System.Double"_fnv:
            switch (param_hashes[1]) {
            case "System.Single"_fnv:
                switch (param_hashes[2]) {
                case "System.Single"_fnv:
                    unpack_and_call.operator()<double, float, float>();
                    break;
                case "System.Double"_fnv:
                    unpack_and_call.operator()<double, float, double>();
                    break;
                default:
                    unpack_and_call.operator()<double, float, void*>();
                    break;
                }
                break;
            case "System.Double"_fnv:
                switch (param_hashes[2]) {
                case "System.Single"_fnv:
                    unpack_and_call.operator()<double, double, float>();
                    break;
                case "System.Double"_fnv:
                    unpack_and_call.operator()<double, double, double>();
                    break;
                default:
                    unpack_and_call.operator()<double, double, void*>();
                    break;
                }
                break;
            default:
                switch (param_hashes[2]) {
                case "System.Single"_fnv:
                    unpack_and_call.operator()<double, void*, float>();
                    break;
                case "System.Double"_fnv:
                    unpack_and_call.operator()<double, void*, double>();
                    break;
                default:
                    unpack_and_call.operator()<double, void*, void*>();
                    break;
                }
                break;
            }
            break;
        default:
            switch (param_hashes[1]) {
            case "System.Single"_fnv:
                switch (param_hashes[2]) {
                case "System.Single"_fnv:
                    unpack_and_call.operator()<void*, float, float>();
                    break;
                case "System.Double"_fnv:
                    unpack_and_call.operator()<void*, float, double>();
                    break;
                default:
                    unpack_and_call.operator()<void*, float, void*>();
                    break;
                }
                break;
            case "System.Double"_fnv:
                switch (param_hashes[2]) {
                case "System.Single"_fnv:
                    unpack_and_call.operator()<void*, double, float>();
                    break;
                case "System.Double"_fnv:
                    unpack_and_call.operator()<void*, double, double>();
                    break;
                default:
                    unpack_and_call.operator()<void*, double, void*>();
                    break;
                }
                break;
            default:
                switch (param_hashes[2]) {
                case "System.Single"_fnv:
                    unpack_and_call.operator()<void*, void*, float>();
                    break;
                case "System.Double"_fnv:
                    unpack_and_call.operator()<void*, void*, double>();
                    break;
                default:
                    unpack_and_call.operator()<void*, void*, void*>();
                    break;
                }
                break;
            }
            break;
        }
    default:
        // for now, we aren't going to handle the case where there are more than 3 parameters
        // because that will get very very messy
        // maybe try to fix it with templates or JIT or something...
        // but for now, just return an empty struct
        break;
    }

    return out;
#endif
}

uint32_t sdk::REMethodDefinition::get_index() const {
    auto tdb = RETypeDB::get();

    return (uint32_t)(((uintptr_t)this - (uintptr_t)tdb->methods) / sizeof(sdk::REMethodDefinition));
}

int32_t REMethodDefinition::get_virtual_index() const {
#if TDB_VER >= 69
    auto tdb = RETypeDB::get();
    auto& impl = (*tdb->methodsImpl)[this->impl_id];
    return impl.vtable_index;
#else
    return this->vtable_index;
#endif
}

uint16_t REMethodDefinition::get_flags() const {
#if TDB_VER >= 69
    auto tdb = RETypeDB::get();
    auto& impl = (*tdb->methodsImpl)[this->impl_id];
    return impl.flags;
#else
    return this->flags;
#endif
}

uint16_t REMethodDefinition::get_impl_flags() const {
#if TDB_VER >= 69
    auto tdb = RETypeDB::get();
    auto& impl = (*tdb->methodsImpl)[this->impl_id];
    return impl.impl_flags;
#else
    return this->impl_flags;
#endif
}

bool REMethodDefinition::is_static() const {
    const auto method_flags = this->get_flags();

    return (method_flags & (uint16_t)::via::clr::MethodFlag::Static) != 0;
}

uint32_t sdk::REMethodDefinition::get_num_params() const {
    const auto params_index = get_param_index();

#if TDB_VER >= 69
    auto tdb = RETypeDB::get();
    const auto param_list = params_index;
    const auto param_ids = tdb->get_data<sdk::ParamList>(param_list);
    return param_ids->numParams;
#else
#if TDB_VER >= 66
    return this->num_params;
#else
    auto tdb = RETypeDB::get();
    return tdb->get_data<sdk::REMethodParamDef>(this->params)->num_params;
#endif
#endif
}

std::vector<uint32_t> REMethodDefinition::get_param_typeids() const {
    const auto param_list = get_param_index();

    auto tdb = RETypeDB::get();

    std::vector<uint32_t> out{};

    // Parameters
#if TDB_VER >= 69
    auto param_ids = tdb->get_data<sdk::ParamList>(param_list);
    const auto num_params = param_ids->numParams;

    // Parse all params
    for (auto f = 0; f < num_params; ++f) {
        const auto param_index = param_ids->params[f] & tdb->get_param_bitmask();

        if (param_index >= tdb->numParams) {
            break;
        }

        auto& p = (*tdb->params)[param_index];

        out.push_back(p.type_id);
    }

    return out;
#else
#if TDB_VER >= 66
    const auto param_ids = tdb->get_data<sdk::REMethodParamDef>(param_list);
#else
    const auto param_data = tdb->get_data<sdk::REMethodParamDef>(param_list);
    const auto param_ids = param_data->params;
#endif

    const auto num_params = get_num_params();

    for (uint32_t f = 0; f < num_params; ++f) {
        auto& p = param_ids[f];
        out.push_back((uint32_t)p.param_typeid);
    }

    return out;
#endif
}

std::vector<sdk::RETypeDefinition*> REMethodDefinition::get_param_types() const {
    auto typeids = get_param_typeids();

    if (typeids.empty()) {
        return {};
    }

    auto tdb = RETypeDB::get();
    std::vector<sdk::RETypeDefinition*> out{};

    static const auto system_object = tdb->find_type("System.Object");

    for (auto id : typeids) {
        const auto t = tdb->get_type(id);

        if (t == nullptr) {
            out.push_back(system_object); // TODO: this is a hack. not sure why this happens
            continue;
        }
        out.push_back(t);
    }

    return out;
}

std::vector<const char*> REMethodDefinition::get_param_names() const {
    std::vector<const char*> out{};

    auto tdb = RETypeDB::get();
    const auto param_list = get_param_index();

#if TDB_VER >= 69
    auto param_ids = tdb->get_data<sdk::ParamList>(param_list);
    const auto num_params = param_ids->numParams;

    // Parse all params
    for (auto f = 0; f < num_params; ++f) {
        const auto param_index = param_ids->params[f];

        if (param_index >= tdb->numParams) {
            break;
        }

        auto& p = (*tdb->params)[param_index];

        out.push_back(tdb->get_string(p.name_offset));
    }

    return out;
#else
    const auto num_params = get_num_params();

#if TDB_VER >= 66
    const auto param_ids = tdb->get_data<sdk::REMethodParamDef>(param_list);
#else
    const auto param_data = tdb->get_data<sdk::REMethodParamDef>(param_list);
    const auto param_ids = param_data->params;
#endif

    for (uint32_t f = 0; f < num_params; ++f) {
        const auto param_index = param_ids[f].param_typeid;
        const auto name_offset = (uint32_t)param_ids[f].name_offset;
        out.push_back(tdb->get_string(name_offset));
    }
    
    return out;
#endif
}
} // namespace sdk