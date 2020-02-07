#pragma once

#include "ReClass.hpp"

namespace utility::re_array {
    // Forward declarations
    static bool has_inline_elements(::REArrayBase* container);

    template<typename T> static T* get_inline_element(::REArrayBase* container, int idx);
    template<typename T> static T* get_ptr_element(::REArrayBase* container, int idx);
    template<typename T> static T* get_element(::REArrayBase* container, int idx);

    static bool has_inline_elements(::REArrayBase* container) {
        if (container->containedType == nullptr) {
            return false;
        }

        return container->containedType->objectType == (uint8_t)via::clr::VMObjType::ValType;
    }

    template<typename T>
    static T* get_inline_element(::REArrayBase* container, int idx) {
        if (idx < 0 || idx >= container->numElements) {
            return nullptr;
        }             

        auto data = Address{ REManagedObject::get_field_ptr(container) };
        return data.get(container->info->classInfo->elementSize * idx).as<T*>();
    }

    template<typename T>
    static T* get_ptr_element(::REArrayBase* container, int idx) {
        if (idx < 0 || idx >= container->numElements) {
            return nullptr;
        }

        T** data = (T**)REManagedObject::get_field_ptr(container);
        return data[idx];
    }

    // may need more work to handle unseen cases.
    template<typename T> 
    static T* get_element(::REArrayBase* container, int idx) {
        return has_inline_elements(container) ? get_inline_element<T>(container, idx) : get_ptr_element<T>(container, idx);
    }
}