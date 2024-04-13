#include <string_view>
#include <memory>
#include <vector>

#include "Enums_Internal.hpp"
#include "utility/Address.hpp"

#include "Enums_Internal.hpp"

class REManagedObject;
class REType;
class FunctionDescriptor;
class REVariableList;
class VariableDescriptor;

namespace sdk {
struct RETypeDefinition;
}

namespace utility::re_managed_object {
// Exposed because these take forever to scan for
// Maybe we can just do some clever scanning through some reflected methods in the future.
namespace detail {
void resolve_add_ref();
void resolve_release();
}

// Forward declarations
struct ParamWrapper;
bool is_managed_object(Address address);

// Check object type name
bool is_a(::REManagedObject* object, std::string_view name);
// Check object type
bool is_a(::REManagedObject* object, REType* cmp);

void add_ref(::REManagedObject* object);
void release(::REManagedObject* object);
std::vector<::REManagedObject*> deserialize(const uint8_t* data, size_t size, bool add_references);
void deserialize_native(::REManagedObject* object, const uint8_t* data, size_t size, const std::vector<::REManagedObject*>& objects);

// Get full type information about the object
REType* get_type(::REManagedObject* object);
sdk::RETypeDefinition* get_type_definition(::REManagedObject* object);
REType* safe_get_type(::REManagedObject* object);
std::string get_type_name(::REManagedObject* object);

// Get the VMObjType of the object
via::clr::VMObjType get_vm_type(::REManagedObject* object);

// Get the total size of the object
uint32_t get_size(::REManagedObject* object);

// Get the field/variable list for the type
REVariableList* get_variables(::REType* t);
REVariableList* get_variables(::REManagedObject* obj);

// Get a field descriptor by name
VariableDescriptor* get_field_desc(::REManagedObject* obj, std::string_view field);

// Get a method descriptor by name
FunctionDescriptor* get_method_desc(::REManagedObject* obj, std::string_view name);

// Gets the base offset of the top class in the hierarchy for this object
template <typename T = void*> static T* get_field_ptr(::REManagedObject* object);

// Get a field value by field descriptor
template <typename T> T get_field(::REManagedObject* obj, VariableDescriptor* desc);

// Get a field value by name
// Be very careful with the type size here, stack corruption could occur if the size is not large enough!
template <typename T> T get_field(::REManagedObject* obj, std::string_view field);

template <typename Arg> static std::unique_ptr<ParamWrapper> call_method(::REManagedObject* obj, FunctionDescriptor* desc, const Arg& arg);

template <typename Arg> static std::unique_ptr<ParamWrapper> call_method(::REManagedObject* obj, std::string_view name, const Arg& arg);
}

#pragma once

#include <windows.h>
#include <mutex>
#include <memory>
#include <string_view>

#include "REContext.hpp"
#include "ReClass.hpp"
#include "REArray.hpp"

namespace utility::re_managed_object {
struct ParamWrapper {
    ParamWrapper(::REManagedObject* obj) {
        params.object_ptr = (void*)obj;
    }

    virtual ~ParamWrapper() {}

    MethodParams params{};
};

template<typename T>
T* get_field_ptr(::REManagedObject* object) {
    if (object == nullptr) {
        return nullptr;
    }

#if TDB_VER > 49
    // object - 8. Dunno, just what the game does.
    const auto offset = Address(object).deref().sub(sizeof(void*)).to<int32_t>();
    return Address(object).get(offset).as<T*>();
#else
    if (object->info == nullptr) {
        return nullptr;
    }

    const auto offset = object->info->size;
    return Address(object).get(offset).as<T*>();
#endif
}

template <typename T> 
T get_field(::REManagedObject* obj, VariableDescriptor* desc) {
    T data{};

    if (desc == nullptr) {
        return data;
    }

    auto get_value_func = (void* (*)(VariableDescriptor*, ::REManagedObject*, void*))desc->function;

    if (get_value_func != nullptr) {
        get_value_func(desc, obj, &data);
    }

    return data;
}

template <typename T>
T get_field(::REManagedObject* obj, std::string_view field) {
    return get_field<T>(obj, get_field_desc(obj, field));
}

template <typename Arg> 
std::unique_ptr<ParamWrapper> call_method(::REManagedObject* obj, FunctionDescriptor* desc, const Arg& arg) {
    if (desc == nullptr) {
        return nullptr;
    }

    auto method_func = (ParamWrapper * (*)(MethodParams*, ::REThreadContext*)) desc->functionPtr;

    if (method_func != nullptr) {
        auto params = std::make_unique<ParamWrapper>(obj);

        params->params.in_data = (void***)&arg;

        method_func(&params->params, sdk::get_thread_context());
        return std::move(params);
    }

    return nullptr;
}

template <typename Arg>
std::unique_ptr<ParamWrapper> call_method(::REManagedObject* obj, std::string_view name, const Arg& arg) {
    return call_method(obj, get_method_desc(obj, name), arg);
}
}