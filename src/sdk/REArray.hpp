#pragma once

#include "ReClass.hpp"

namespace utility::REArray {
    // Forward declarations
    static bool hasInlineElements(::REArrayBase* container);

    template<typename T> static T* getInlineElement(::REArrayBase* container, int idx);
    template<typename T> static T* getPtrElement(::REArrayBase* container, int idx);
    template<typename T> static T* getElement(::REArrayBase* container, int idx);

    static bool hasInlineElements(::REArrayBase* container) {
        auto info = container->info;

        if (info == nullptr || info->classInfo == nullptr) {
            return false;
        }

        return info->classInfo->objectType == (uint8_t)via::clr::VMObjType::ValType;
    }

    template<typename T>
    static T* getInlineElement(::REArrayBase* container, int idx) {
        if (idx < 0 || idx >= container->numElements) {
            return nullptr;
        }             

        auto data = Address{ REManagedObject::getFieldPtr(container) };
        return data.get(container->info->classInfo->elementSize * idx).as<T*>();
    }

    template<typename T>
    static T* getPtrElement(::REArrayBase* container, int idx) {
        if (idx < 0 || idx >= container->numElements) {
            return nullptr;
        }

        T** data = (T**)REManagedObject::getFieldPtr(container);
        return data[idx];
    }

    // may need more work to handle unseen cases.
    template<typename T> 
    static T* getElement(::REArrayBase* container, int idx) {
        return hasInlineElements(container) ? getInlineElement<T>(container, idx) : getPtrElement<T>(container, idx);
    }
}