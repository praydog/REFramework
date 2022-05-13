class REArrayBase;

namespace utility::re_array {
    // Forward declarations
    static bool has_inline_elements(::REArrayBase* container);
    static uint32_t get_element_size(::REArrayBase* container);

    template <typename T> static T* get_inline_element(::REArrayBase* container, int idx);
    template <typename T> static T* get_ptr_element(::REArrayBase* container, int idx);
    template <typename T> static T* get_element(::REArrayBase* container, int idx);

}

#pragma once

#include "utility/Address.hpp"

#include "REManagedObject.hpp"
#include "REType.hpp"
#include "ReClass.hpp"

namespace sdk {
struct RETypeDefinition;
}

namespace utility::re_class_info {
static via::clr::VMObjType get_vm_type(::REClassInfo* c) {
    if (c == nullptr) {
        return via::clr::VMObjType::NULL_;
    }

#if defined(RE8) || defined(MHRISE)
    return (via::clr::VMObjType)(c->objectFlags >> 5);
#else
    return (via::clr::VMObjType)c->objectType;
#endif
}
} // namespace utility::re_type

namespace utility::re_array {
    // Forward declarations
    static bool has_inline_elements(::REArrayBase* container);

    template<typename T> static T* get_inline_element(::REArrayBase* container, int idx);
    template<typename T> static T* get_ptr_element(::REArrayBase* container, int idx);
    template<typename T> static T* get_element(::REArrayBase* container, int idx);

    static ::sdk::RETypeDefinition* get_contained_type(::REArrayBase* container) {
        if (container == nullptr || container->containedType == nullptr) {
            return nullptr;
        }

    #ifndef RE7
        return (::sdk::RETypeDefinition*)container->containedType;
    #else
        if (container->containedType->classInfo == nullptr) {
            return nullptr;
        }

        return (::sdk::RETypeDefinition*)container->containedType->classInfo;
    #endif
    }

    static uint32_t get_element_size(::REArrayBase* container) {
        if (container == nullptr || container->containedType == nullptr) {
            return 0;
        }

        if (!utility::re_array::has_inline_elements(container)) {
            return sizeof(void*);
        }

#if defined(RE8) || defined(MHRISE)
        const auto element_size = utility::re_type::get_value_type_size(container->containedType->type);
#elif defined(RE7)
        const auto element_size = container->containedType->classInfo->elementSize;
#else
        const auto element_size = container->info->classInfo->elementSize;
#endif

        return element_size;
    }

    static bool has_inline_elements(::REArrayBase* container) {
        if (container->containedType == nullptr) {
            return false;
        }

    #ifndef RE7
        return utility::re_class_info::get_vm_type(container->containedType) == via::clr::VMObjType::ValType;
    #else
        if (container->containedType->classInfo == nullptr) {
            return false;
        }

        return utility::re_class_info::get_vm_type(container->containedType->classInfo) == via::clr::VMObjType::ValType;
    #endif
    }

    template<typename T>
    static T* get_inline_element(::REArrayBase* container, int idx) {
        if (idx < 0 || idx >= container->numElements) {
            return nullptr;
        }

        const auto element_size = utility::re_array::get_element_size(container);

        auto data = Address{ (uintptr_t)((REArrayBase*)utility::re_managed_object::get_field_ptr(container) + 1) - sizeof(REManagedObject) };
        return data.get(element_size * idx).as<T*>();
    }

    template<typename T>
    static T* get_ptr_element(::REArrayBase* container, int idx) {
        if (idx < 0 || idx >= container->numElements) {
            return nullptr;
        }

        auto data = (T**)((uintptr_t)((REArrayBase*)utility::re_managed_object::get_field_ptr(container) + 1) - sizeof(REManagedObject));
        return data[idx];
    }

    // may need more work to handle unseen cases.
    template<typename T> 
    static T* get_element(::REArrayBase* container, int idx) {
        return has_inline_elements(container) ? get_inline_element<T>(container, idx) : get_ptr_element<T>(container, idx);
    }
}