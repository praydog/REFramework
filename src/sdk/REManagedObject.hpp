#pragma once

#include <windows.h>
#include <mutex>

#include "utility/Address.hpp"
#include "ReClass.hpp"

namespace utility::REManagedObject {
    // Forward declarations
    static bool isManagedObject(Address address);
    // Check object type name
    static bool isA(::REManagedObject* object, std::string_view name);

    // Get full type information about the object
    static REType* getType(::REManagedObject* object);
    static REType* safeGetType(::REManagedObject* object);
    static std::string getTypeName(::REManagedObject* object);

    // Get the VMObjType of the object
    static via::clr::VMObjType getVMType(::REManagedObject* object);

    // Get the total size of the object
    static uint32_t getSize(::REManagedObject* object);

    // Get the field/variable list for the type
    static REVariableList* getVariables(::REType* t);
    static REVariableList* getVariables(::REManagedObject* obj);

    // Get a field descriptor by name
    static VariableDescriptor* getFieldDesc(::REManagedObject* obj, std::string_view field);

    // Gets the base offset of the top class in the hierarchy for this object
    template <typename T>
    static T* getFieldPtr(::REManagedObject* object);

    // Get a field value by name
    // Be very careful with the type size here, stack corruption could occur if the size is not large enough!
    template <typename T>
    T getField(::REManagedObject* obj, std::string_view field);


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

    std::string getTypeName(::REManagedObject* object) {
        auto t = getType(object);

        if (t == nullptr) {
            return "";
        }

        return t->name;
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
    
    template<typename T = void*>
    static T* getFieldPtr(::REManagedObject* object) {
        if (object == nullptr) {
            return nullptr;
        }

        // object - 8. Dunno, just what the game does.
        auto offset = Address(object).deref().sub(sizeof(void*)).to<int32_t>();
        return Address(object).get(offset).as<T*>();
    }

    static via::clr::VMObjType getVMType(::REManagedObject* object) {
        auto info = object->info;

        if (info == nullptr || info->classInfo == nullptr) {
            return via::clr::VMObjType::NULL_;
        }

        return (via::clr::VMObjType)info->classInfo->objectType;
    }

    static uint32_t getSize(::REManagedObject* object) {
        auto info = object->info;

        if (info == nullptr || info->classInfo == nullptr) {
            return 0;
        }

        auto classInfo = info->classInfo;
        auto size = classInfo->size;

        switch ((via::clr::VMObjType)classInfo->objectType) {
        case via::clr::VMObjType::Array:
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