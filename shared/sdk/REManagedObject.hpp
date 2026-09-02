#pragma once

#include <windows.h>
#include <mutex>
#include <memory>
#include <string_view>

#include "REContext.hpp"
#include "ReClass.hpp"
#include "REArray.hpp"
#include "Enums_Internal.hpp"
#include "utility/Address.hpp"

// Template implementations for REManagedObject member methods.
// These can't go in types/REManagedObject.hpp due to dependencies on ReClass.hpp etc.

struct REManagedObject::ParamWrapper {
    ParamWrapper(::REManagedObject* obj) {
        params.object_ptr = (void*)obj;
    }

    virtual ~ParamWrapper() {}

    MethodParams params{};
};

template<typename T>
T* REManagedObject::get_field_ptr() const {
    if (this == nullptr) {
        return nullptr;
    }

#if TDB_VER > 49
    // object - 8. Dunno, just what the game does.
    const auto offset = Address{const_cast<REManagedObject*>(this)}.deref().sub(sizeof(void*)).to<int32_t>();
    return Address{const_cast<REManagedObject*>(this)}.get(offset).as<T*>();
#else
    if (this->info == nullptr) {
        return nullptr;
    }

    const auto offset = this->info->size;
    return Address{const_cast<REManagedObject*>(this)}.get(offset).as<T*>(); 
#endif
}

template <typename T>
T REManagedObject::get_reflection_property(VariableDescriptor* desc) const {
    T data{};

    if (desc == nullptr) {
        return data;
    }

    auto get_value_func = (void* (*)(VariableDescriptor*, ::REManagedObject*, void*))desc->get_function();

    if (get_value_func != nullptr) {
        get_value_func(desc, const_cast<::REManagedObject*>(this), &data);
    }

    return data;
}

template <typename T>
T REManagedObject::get_reflection_property(std::string_view field) const {
    return get_reflection_property<T>(get_field_desc(field));
}

template <typename Arg>
std::unique_ptr<REManagedObject::ParamWrapper> REManagedObject::call_method(FunctionDescriptor* desc, const Arg& arg) {
    if (desc == nullptr) {
        return nullptr;
    }

    auto method_func = (ParamWrapper * (*)(MethodParams*, ::REThreadContext*)) desc->get_functionPtr();

    if (method_func != nullptr) {
        auto params = std::make_unique<ParamWrapper>(this);

        params->params.in_data = (void***)&arg;

        method_func(&params->params, sdk::get_thread_context());
        return std::move(params);
    }

    return nullptr;
}

template <typename Arg>
std::unique_ptr<REManagedObject::ParamWrapper> REManagedObject::call_method(std::string_view name, const Arg& arg) {
    return call_method(get_method_desc(name), arg);
}
