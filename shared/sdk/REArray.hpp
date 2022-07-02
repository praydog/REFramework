class REArrayBase;

namespace utility::re_array {
    // Forward declarations
    bool has_inline_elements(::REArrayBase* container);

    ::sdk::RETypeDefinition* get_contained_type(::REArrayBase* container);
    uint32_t get_element_size(::REArrayBase* container);
    bool has_inline_elements(::REArrayBase* container);

    template<typename T> static T* get_inline_element(::REArrayBase* container, int idx);
    template<typename T> static T* get_ptr_element(::REArrayBase* container, int idx);
    template<typename T> static T* get_element(::REArrayBase* container, int idx);

}

#pragma once

#include "utility/Address.hpp"

#include "REManagedObject.hpp"
#include "REType.hpp"
#include "ReClass.hpp"

namespace sdk {
struct RETypeDefinition;
}

namespace utility::re_array {
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