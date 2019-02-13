#pragma once

#include <windows.h>

#include "utility/Address.hpp"
#include "ReClass.hpp"

namespace utility::REManagedObject {
    static bool isManagedObject(Address address) {
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

        auto classInfo = object->info->classInfo;

        if (classInfo == nullptr || IsBadReadPtr(classInfo, sizeof(void*))) {
            return false;
        }

        if (classInfo->parentInfo != object->info || classInfo->type == nullptr) {
            return false;
        }

        if (IsBadReadPtr(classInfo->type, sizeof(REType)) || classInfo->type->name == nullptr) {
            return false;
        }

        if (IsBadReadPtr(classInfo->type->name, sizeof(void*))) {
            return false;
        }

        return true;
    }

    static REType* getType(::REManagedObject* object) {
        if (object == nullptr) {
            return nullptr;
        }

        auto info = object->info;

        if (info == nullptr) {
            return nullptr;
        }

        auto classInfo = info->classInfo;

        if (classInfo == nullptr) {
            return nullptr;
        }

        return classInfo->type;
    }

    static REType* safeGetType(::REManagedObject* object) {
        return isManagedObject(object) ? getType(object) : nullptr;
    }

    static bool isA(::REManagedObject* object, std::string_view name) {
        if (object == nullptr) {
            return false;
        }

        for (auto t = REManagedObject::getType(object); t != nullptr && t->name != nullptr; t = t->super) {
            if (name == t->name) {
                return true;
            }
        }

        return false;
    }
    
    // Gets the base offset of the top class in the hierarchy for this object
    template<typename T = void*>
    static T* getFieldPtr(::REManagedObject* object) {
        if (object == nullptr) {
            return nullptr;
        }

        // object - 8. Dunno, just what the game does.
        auto offset = *(int32_t*)(Address(object).to<uintptr_t>() - sizeof(void*));
        return Address(object).get(offset).as<T*>();
    }

    static uint32_t getSize(::REManagedObject* object) {
        auto info = object->info;

        if (info == nullptr || info->classInfo == nullptr) {
            return 0;
        }

        auto classInfo = info->classInfo;
        auto size = classInfo->size;

        switch (classInfo->objectType) {
        // Maybe should be called an array, idk.
        case 2:
        {
            auto ind = (::IndirectType*)object;

            auto containedType = ind->containedType;
            
            // array of ptrs by default
            uint32_t multiplierThing = sizeof(void*);

            // inline elements?
            if (containedType->objectType == 5) {
                multiplierThing = containedType->sizeThing;
            }

            if (ind->num1 <= 1) {
                size = multiplierThing * ind->numElements + sizeof(::IndirectType);
            }
            else {
                size = multiplierThing * ind->numElements + sizeof(::IndirectType) + 4 * ind->num1;
            }

            break;
        }
        case 3:
            size = sizeof(uint16_t) * (Address(object).get(sizeof(::REManagedObject)).to<uint32_t>() + 1) + 0x14;
            break;
        case 4:
            size = 0x10 * (Address(object).get(sizeof(::REManagedObject)).to<uint32_t>() - 1) + 0x28;
            break;

        // 1 is normal type that we can inspect easily
        default:
            break;
        }

        return size;
    }
}