#pragma once

#include <windows.h>
#include <mutex>

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

    // Get the total size of the object
    static uint32_t getSize(::REManagedObject* object) {
        auto info = object->info;

        if (info == nullptr || info->classInfo == nullptr) {
            return 0;
        }

        auto classInfo = info->classInfo;
        auto size = classInfo->size;

        switch (classInfo->objectType) {
        case 2:
        {
            auto container = (::REArrayBase*)object;
            auto containedType = container->containedType;
            
            // array of ptrs by default
            uint32_t elementSize = utility::REArray::hasInlineElements(container) ? containedType->elementSize : sizeof(void*);

            if (container->num1 <= 1) {
                size = elementSize * container->numElements + sizeof(::REArrayBase);
            }
            else {
                size = elementSize * container->numElements + sizeof(::REArrayBase) + 4 * container->num1;
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

    // Get the field/variable list for the type
    static REVariableList* getVariables(::REType* t) {
        if (t == nullptr || t->fields == nullptr || t->fields->variables == nullptr) {
            return nullptr;
        }

        auto vars = t->fields->variables;

        if (vars->data == nullptr || vars->num <= 0) {
            return nullptr;
        }

        return vars;
    }

    static REVariableList* getVariables(::REManagedObject* obj) {
        return getVariables(getType(obj));
    }

    // Get a field descriptor by name
    static VariableDescriptor* getFieldDesc(::REManagedObject* obj, std::string_view field) {
        static std::mutex insertionMutex{};
        static std::unordered_map<std::string, VariableDescriptor*> varMap{};
        
        auto t = getType(obj);

        if (t == nullptr) {
            return nullptr;
        }

        auto fullName = std::string{ t->name } + "." + field.data();

        if (varMap.find(fullName) != varMap.end()) {
            return varMap[fullName];
        }

        for (; t != nullptr; t = t->super) {
            auto vars = getVariables(t);

            if (vars == nullptr) {
                continue;
            }

            for (auto i = 0; i < vars->num; ++i) {
                auto& var = vars->data->descriptors[i];

                if (var == nullptr || var->name == nullptr) {
                    continue;
                }

                if (field == var->name) {
                    std::lock_guard _{ insertionMutex };
                    varMap[fullName] = var;
                    return var;
                }
            }
        }

        return nullptr;
    }

    // Get a field value by name
    // Be very careful with the type size here, stack corruption could occur if the size is not large enough!
    template <typename T>
    T getField(::REManagedObject* obj, std::string_view field) {
        T data{};

        auto desc = getFieldDesc(obj, field);

        if (desc != nullptr) {
            auto getValueFunc = (void* (*)(VariableDescriptor*, ::REManagedObject*, void*))desc->function;

            if (getValueFunc != nullptr) {
                getValueFunc(desc, obj, &data);
            }
        }

        return data;
    }
}