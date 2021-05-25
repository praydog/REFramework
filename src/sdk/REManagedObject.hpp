#pragma once

#include <windows.h>
#include <mutex>
#include <memory>
#include <string_view>

#include "utility/Address.hpp"

#include "ReClass.hpp"

namespace utility::re_managed_object {
    // Forward declarations
    struct ParamWrapper;
    static bool is_managed_object(Address address);

    // Check object type name
    static bool is_a(::REManagedObject* object, std::string_view name);
    // Check object type
    static bool is_a(::REManagedObject* object, REType* cmp);

    // Get full type information about the object
    static REType* get_type(::REManagedObject* object);
    static REType* safe_get_type(::REManagedObject* object);
    static std::string get_type_name(::REManagedObject* object);

    // Get the VMObjType of the object
    static via::clr::VMObjType get_vm_type(::REManagedObject* object);

    // Get the total size of the object
    static uint32_t get_size(::REManagedObject* object);

    // Get the field/variable list for the type
    static REVariableList* get_variables(::REType* t);
    static REVariableList* get_variables(::REManagedObject* obj);

    // Get a field descriptor by name
    static VariableDescriptor* get_field_desc(::REManagedObject* obj, std::string_view field);

    // Gets the base offset of the top class in the hierarchy for this object
    template <typename T>
    static T* get_field_ptr(::REManagedObject* object);

    // Get a field value by field descriptor
    template <typename T> 
    T get_field(::REManagedObject* obj, VariableDescriptor* desc);

    // Get a field value by name
    // Be very careful with the type size here, stack corruption could occur if the size is not large enough!
    template <typename T>
    T get_field(::REManagedObject* obj, std::string_view field);
    
    template <typename Arg> 
    static std::unique_ptr<ParamWrapper> call_method(::REManagedObject* obj, FunctionDescriptor* desc, const Arg& arg);

    template <typename Arg>
    static std::unique_ptr<ParamWrapper> call_method(::REManagedObject* obj, std::string_view name, const Arg& arg);

    struct ParamWrapper {
        ParamWrapper(::REManagedObject* obj) {
            params.object_ptr = (void*)obj;
        }

        virtual ~ParamWrapper() {}

        MethodParams params{};
    };

    static bool is_managed_object(Address address) {
        if (address == nullptr) {
            return false;
        }

        if (IsBadReadPtr(address.ptr(), sizeof(void*))) {
            return false;
        }

        auto object = address.as<::REManagedObject*>();

        if (object->info == nullptr || IsBadReadPtr(object->info, sizeof(void*))) {
            return false;
        }

        auto class_info = object->info->classInfo;

        if (class_info == nullptr || IsBadReadPtr(class_info, sizeof(void*))) {
            return false;
        }

        if (class_info->parentInfo != object->info || class_info->type == nullptr) {
            return false;
        }

        if (IsBadReadPtr(class_info->type, sizeof(REType)) || class_info->type->name == nullptr) {
            return false;
        }

        if (IsBadReadPtr(class_info->type->name, sizeof(void*))) {
            return false;
        }

        return true;
    }

    static REType* get_type(::REManagedObject* object) {
        if (object == nullptr) {
            return nullptr;
        }

        auto info = object->info;

        if (info == nullptr) {
            return nullptr;
        }

        auto class_info = info->classInfo;

        if (class_info == nullptr) {
            return nullptr;
        }

        return class_info->type;
    }

    static REType* safe_get_type(::REManagedObject* object) {
        return is_managed_object(object) ? get_type(object) : nullptr;
    }

    std::string get_type_name(::REManagedObject* object) {
        auto t = get_type(object);

        if (t == nullptr) {
            return "";
        }

        return t->name;
    }

    static bool is_a(::REManagedObject* object, std::string_view name) {
        if (object == nullptr) {
            return false;
        }

        for (auto t = re_managed_object::get_type(object); t != nullptr && t->name != nullptr; t = t->super) {
            if (name == t->name) {
                return true;
            }
        }

        return false;
    }

    bool is_a(::REManagedObject* object, REType* cmp) {
        if (object == nullptr) {
            return false;
        }

        for (auto t = re_managed_object::get_type(object); t != nullptr && t->name != nullptr; t = t->super) {
            if (cmp == t) {
                return true;
            }
        }

        return false;
    }


    template<typename T = void*>
    static T* get_field_ptr(::REManagedObject* object) {
        if (object == nullptr) {
            return nullptr;
        }

        // object - 8. Dunno, just what the game does.
        auto offset = Address(object).deref().sub(sizeof(void*)).to<int32_t>();
        return Address(object).get(offset).as<T*>();
    }

    static via::clr::VMObjType get_vm_type(::REManagedObject* object) {
        auto info = object->info;

        if (info == nullptr || info->classInfo == nullptr) {
            return via::clr::VMObjType::NULL_;
        }

#ifndef RE8
        return (via::clr::VMObjType)info->classInfo->objectType;
#else
        return (via::clr::VMObjType)(info->classInfo->objectFlags >> 5);
#endif
    }

    static uint32_t get_size(::REManagedObject* object) {
        auto info = object->info;

        if (info == nullptr || info->classInfo == nullptr) {
            return 0;
        }

        auto class_info = info->classInfo;
        auto size = class_info->size;

        switch (get_vm_type(object)) {
        case via::clr::VMObjType::Array:
        {
            auto container = (::REArrayBase*)object;
            auto contained_type = container->containedType;
            
            // array of ptrs by default
            auto element_size = utility::re_array::get_element_size(container);

            if (container->num1 <= 1) {
                size = element_size * container->numElements + sizeof(::REArrayBase);
            }
            else {
                size = element_size * container->numElements + sizeof(::REArrayBase) + 4 * container->num1;
            }

            break;
        }
        case via::clr::VMObjType::String:
            size = sizeof(uint16_t) * (Address(object).get(sizeof(::REManagedObject)).to<uint32_t>() + 1) + 0x14;
            break;
        case via::clr::VMObjType::Delegate:
            size = 0x10 * (Address(object).get(sizeof(::REManagedObject)).to<uint32_t>() - 1) + 0x28;
            break;
        case via::clr::VMObjType::Object:
        default:
            break;
        }
        
        return size;
    }

    static REVariableList* get_variables(::REType* t) {
        return re_type::get_variables(t);
    }

    static REVariableList* get_variables(::REManagedObject* obj) {
        return get_variables(get_type(obj));
    }

    static VariableDescriptor* get_field_desc(::REManagedObject* obj, std::string_view field) {
        return re_type::get_field_desc(get_type(obj), field);
    }

    static FunctionDescriptor* get_method_desc(::REManagedObject* obj, std::string_view name) {
        static std::mutex insertion_mutex{};
        static std::unordered_map<std::string, FunctionDescriptor*> var_map{};

        auto t = get_type(obj);

        if (t == nullptr) {
            return nullptr;
        }

        auto full_name = std::string{ t->name } + "." + name.data();

        for (; t != nullptr; t = t->super) {
            auto fields = t->fields;

            if (fields == nullptr || fields->methods == nullptr) {
                continue;
            }

            auto methods = fields->methods;

            for (auto i = 0; i < fields->num; ++i) {
                auto top = (*methods)[i];

                if (top == nullptr || *top == nullptr) {
                    continue;
                }

                auto& holder = **top;

                if (holder.descriptor == nullptr || holder.descriptor->name == nullptr) {
                    continue;
                }

                if (name == holder.descriptor->name) {
                    std::lock_guard _{ insertion_mutex };
                    var_map[full_name] = holder.descriptor;
                    return holder.descriptor;
                }
            }
        }

        return nullptr;
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