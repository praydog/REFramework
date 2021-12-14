#include <spdlog/spdlog.h>

#include "utility/Scan.hpp"

#include "ReClass.hpp"

#include "REFramework.hpp"
#include "REManagedObject.hpp"

namespace utility::re_managed_object {
void add_ref(REManagedObject* object) {
    if (object == nullptr) {
        return;
    }
    
    static void (*add_ref_func)(::REManagedObject*) = nullptr;

    if (add_ref_func == nullptr) {
        constexpr std::array<std::string_view, 2> possible_patterns{
            "40 57 48 83 EC ? 8B 41 ? 48 8B F9 85 C0 0F ? ? ? ? ? 0F ? ? 0E", // RE2+
            "48 89 ? ? ? 57 48 83 EC ? 0F ? ? 0E" // RE7
        };

        spdlog::info("[REManagedObject] Finding add_ref function...");

        for (auto pattern : possible_patterns) {
            auto address = utility::scan(g_framework->get_module().as<HMODULE>(), pattern.data());

            if (address) {
                add_ref_func = (decltype(add_ref_func))*address;
                break;
            }
        }

        spdlog::info("[REManagedObject] Found add_ref function at {:x}", (uintptr_t)add_ref_func);
    }

    //spdlog::info("Pushing: {} {} {:x}", (int32_t)object->referenceCount, utility::re_managed_object::get_type_definition(object)->get_full_name(), (uintptr_t)object);

#ifdef RE7
    if ((int32_t)object->referenceCount >= 0) {
        _InterlockedIncrement(&object->referenceCount);
    } else {
        add_ref_func(object);
    }
#else
    add_ref_func(object);
#endif
}

void release(REManagedObject* object) {
    if (object == nullptr) {
        return;
    }

    static void (*release_func)(::REManagedObject*) = nullptr;

    if (release_func == nullptr) {
        constexpr std::array<std::string_view, 2> possible_patterns{
            "40 53 48 83 EC ? 8B 41 08 48 8B D9 85 C0 0F", // RE2+
            "40 53 48 83 EC ? 8B 41 08 48 8B D9 48 83 C1 08 85 C0 78" // RE7
        };

        spdlog::info("[REManagedObject] Finding release function...");

        for (auto pattern : possible_patterns) {
            auto address = utility::scan(g_framework->get_module().as<HMODULE>(), pattern.data());

            if (address) {
                release_func = (decltype(release_func))*address;
                break;
            }
        }

        spdlog::info("[REManagedObject] Found release function at {:x}", (uintptr_t)release_func);
    }

    //spdlog::info("Popping: {} {} {:x}", (int32_t)object->referenceCount, utility::re_managed_object::get_type_definition(object)->get_full_name(), (uintptr_t)object);

    release_func(object);

    //spdlog::info("Now: {}", (int32_t)object->referenceCount);
}


bool is_managed_object(Address address) {
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

#ifndef RE7
    if (class_info->parentInfo != object->info || class_info->type == nullptr) {
        return false;
    }

    if (IsBadReadPtr(class_info->type, sizeof(REType)) || class_info->type->name == nullptr) {
        return false;
    }

    if (IsBadReadPtr(class_info->type->name, sizeof(void*))) {
        return false;
    }
#else
    auto info = object->info;

    if (info->type == nullptr) {
        return false;
    }

    if (IsBadReadPtr(info->type, sizeof(REType)) || info->type->name == nullptr) {
        return false;
    }

    if (IsBadReadPtr(info->type->name, sizeof(void*))) {
        return false;
    }

    if (info->type->super != nullptr && IsBadReadPtr(info->type->super, sizeof(REType))) {
        return false;
    }

    if (info->type->classInfo != nullptr && IsBadReadPtr(info->type->classInfo, sizeof(REObjectInfo))) {
        return false;
    }

    if (info->type->classInfo != nullptr && info->type->classInfo != object->info) {
        return false;
    }
#endif

    return true;
}

REType* get_type(::REManagedObject* object) {
    if (object == nullptr) {
        return nullptr;
    }

    auto info = object->info;

    if (info == nullptr) {
        return nullptr;
    }

#ifndef RE7
    auto class_info = info->classInfo;

    if (class_info == nullptr) {
        return nullptr;
    }

    return class_info->type;
#else
    return info->type;
#endif
}

sdk::RETypeDefinition* get_type_definition(::REManagedObject* object) {
    if (object == nullptr) {
        return nullptr;
    }

    auto info = object->info;

    if (info == nullptr) {
        return nullptr;
    }

    return (sdk::RETypeDefinition*)info->classInfo;
}

REType* safe_get_type(::REManagedObject* object) {
    return is_managed_object(object) ? get_type(object) : nullptr;
}

std::string get_type_name(::REManagedObject* object) {
    auto t = get_type(object);

    if (t == nullptr) {
        return "";
    }

    return t->name;
}

bool is_a(::REManagedObject* object, std::string_view name) {
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

via::clr::VMObjType get_vm_type(::REManagedObject* object) {
    auto info = object->info;

    if (info == nullptr || info->classInfo == nullptr) {
        return via::clr::VMObjType::NULL_;
    }

#if defined(RE8) || defined(MHRISE)
    return (via::clr::VMObjType)(info->classInfo->objectFlags >> 5);
#else
    return (via::clr::VMObjType)info->classInfo->objectType;
#endif
}

uint32_t get_size(::REManagedObject* object) {
    auto info = object->info;

    if (info == nullptr || info->classInfo == nullptr) {
        return 0;
    }

#ifndef RE7
    auto class_info = info->classInfo;
    auto size = class_info->size;
#else
    auto size = info->size;
#endif

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

REVariableList* get_variables(::REType* t) {
    return re_type::get_variables(t);
}

REVariableList* get_variables(::REManagedObject* obj) {
    return get_variables(get_type(obj));
}

VariableDescriptor* get_field_desc(::REManagedObject* obj, std::string_view field) {
    return re_type::get_field_desc(get_type(obj), field);
}

FunctionDescriptor* get_method_desc(::REManagedObject* obj, std::string_view name) {
    return re_type::get_method_desc(get_type(obj), name);
}
}