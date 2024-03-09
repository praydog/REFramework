#include <spdlog/spdlog.h>

#include "utility/Scan.hpp"
#include "utility/Module.hpp"

#include "ReClass.hpp"

#include "REManagedObject.hpp"

namespace utility::re_managed_object {

// Used all over the place. It may be one of the most referenced functions in a disassembler.
// According to my IDA script, it's the 4th most referenced function though sometimes it's 5th or 6th.
// Release usually has more references than add_ref.
// A dead giveaway is the comparison to byte ptr RCX + 0xE near the start of the function.
// Cheat sheet for functions to find these function(s) in:
// via.Transform::get_Children
static void (*add_ref_func)(::REManagedObject*) = nullptr;
static void (*release_func)(::REManagedObject*) = nullptr;

namespace detail {
void resolve_add_ref() {
    if (add_ref_func != nullptr) {
        return;
    }

    constexpr std::array<std::string_view, 3> possible_patterns{
        "40 ? 48 83 EC ? 8B 41 ? 48 8B ? 85 C0 0F ? ? ? ? ? 0F ? ? 0E", // RE2+
        "40 ? 48 83 EC ? 8B 41 ? 48 8B ? 85 C0 0F ? ? ? ? ? 80 ? 0E 00", // TDB73+/DD2+
        "48 89 ? ? ? 57 48 83 EC ? 0F ? ? 0E" // RE7 TDB49
    };

    spdlog::info("[REManagedObject] Finding add_ref function...");

    for (auto pattern : possible_patterns) {
        auto address = utility::scan(utility::get_executable(), pattern.data());

        if (address) {
            add_ref_func = (decltype(add_ref_func))*address;
            break;
        }
    }

    spdlog::info("[REManagedObject] Found add_ref function at {:x}", (uintptr_t)add_ref_func);
}

void resolve_release() {
    if (release_func != nullptr) {
        return;
    }

    // We also need to resolve add_ref
    // because we need to make sure we don't resolve release to the same function.
    resolve_add_ref();

    constexpr std::array<std::string_view, 2> possible_patterns{
        "40 53 48 83 EC ? 8B 41 08 48 8B D9 85 C0 0F", // RE2+
        "40 53 48 83 EC ? 8B 41 08 48 8B D9 48 83 C1 08 85 C0 78" // RE7
    };

    spdlog::info("[REManagedObject] Finding release function...");

    for (auto pattern : possible_patterns) {
        auto address = utility::scan(utility::get_executable(), pattern.data());

        if (address && *address != (uintptr_t)add_ref_func) {
            release_func = (decltype(release_func))*address;
            break;
        } else if (address) {
            spdlog::info("[REManagedObject] Found add_ref function while looking for release function, trying again...");

            const auto start = *address + 1;
            const auto module_size = *utility::get_module_size(utility::get_executable());
            const auto end = (uintptr_t)utility::get_executable() + module_size;
            const auto size = end - start;
            address = utility::scan(start, size - 0x1000, pattern.data());

            if (address && *address != (uintptr_t)add_ref_func) {
                release_func = (decltype(release_func))*address;
                break;
            }
        }
    }

    spdlog::info("[REManagedObject] Found release function at {:x}", (uintptr_t)release_func);
}
}

void add_ref(REManagedObject* object) {
    if (object == nullptr) {
        return;
    }

    detail::resolve_add_ref();

    //spdlog::info("Pushing: {} {} {:x}", (int32_t)object->referenceCount, utility::re_managed_object::get_type_definition(object)->get_full_name(), (uintptr_t)object);

#if TDB_VER <= 49
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

    detail::resolve_release();

    //spdlog::info("Popping: {} {} {:x}", (int32_t)object->referenceCount, utility::re_managed_object::get_type_definition(object)->get_full_name(), (uintptr_t)object);

    release_func(object);

    //spdlog::info("Now: {}", (int32_t)object->referenceCount);
}

std::vector<::REManagedObject*> deserialize(const uint8_t* data, size_t size, bool add_references) {
    static void (*deserialize_func)(void* placeholder, const sdk::NativeArray<::REManagedObject*>&, const uint8_t*, size_t) = []() -> decltype(deserialize_func) {
        spdlog::info("[REManagedObject] Finding deserialize function...");
        decltype(deserialize_func) result{nullptr};
        uintptr_t first{};

        constexpr std::array<std::string_view, 2> possible_patterns{
            "41 81 ? 52 53 5A 00", // Confirmed RE8+
            "41 81 7D 00 52 53 5A 00" // RE2 (TDB66) -> DMC5
        };

        for (auto pattern : possible_patterns) {
            auto address = utility::scan(utility::get_executable(), pattern.data());

            if (address) {
                first = *address;
                break;
            }
        }

        if (first != 0) {
            spdlog::info("[REManagedObject] Found first step at {:x}", first);

            auto second = utility::scan_reverse(first, 0x100, "48 8B C4");

            if (!second) {
                spdlog::error("[REManagedObject] Failed to find second step");
                return nullptr;
            }

            result = (decltype(result))*second;
        } else {
            spdlog::error("[REManagedObject] Failed to find first step");
            return nullptr;
        }

        spdlog::info("[REManagedObject] Found deserialize function at {:x}", (uintptr_t)result);

        return result;
    }();

    // Array gets resized in the function.
    sdk::NativeArray<::REManagedObject*> arr;
    deserialize_func(nullptr, arr, data, size);

    std::vector<::REManagedObject*> result{};
    
    for (auto object : arr) {
        if (object != nullptr) {
            if (add_references) {
                utility::re_managed_object::add_ref(object);
            }

            result.push_back(object);
        }
    }

    return result;
}

void deserialize_native(::REManagedObject* object, const uint8_t* data, size_t size, const std::vector<::REManagedObject*>& objects) {
    if (!is_managed_object(object)) {
        return;
    }

    const auto tdef = get_type_definition(object);

    if (tdef == nullptr) {
        return;
    }

    const auto t = tdef->get_type();

    if (t == nullptr || t->fields == nullptr || t->fields->deserializer == nullptr) {
        return;
    }

    struct DeserializeStream {
        uint8_t* head{nullptr};
        uint8_t* cur{nullptr};
        uint8_t* tail{nullptr};
        uintptr_t stackptr{0};
        uint8_t* stack[32]{0};
    }; static_assert(sizeof(DeserializeStream) == 0x120, "DeserializeStream is not the correct size");

    const auto deserializer = (void (*)(::REManagedObject*, DeserializeStream*, sdk::NativeArray<REManagedObject*>* objects))t->fields->deserializer;

    std::array<uint8_t, 1024 * 8> stack_buffer{};

    DeserializeStream stream{
        .head = (uint8_t*)data,
        .cur = (uint8_t*)data,
        .tail = (uint8_t*)data + size,
        .stackptr = (uintptr_t)stack_buffer.data() // No need to set the "stack" variable, it seems to get filled by the stackptr
    };

    sdk::NativeArray<::REManagedObject*> objects_array;

    if (!objects.empty()) {
        for (auto object : objects) {
            objects_array.push_back(object);
        }
    }

    deserializer(object, &stream, &objects_array);
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

#if TDB_VER >= 71
    const auto td = (sdk::RETypeDefinition*)class_info;

    if ((uintptr_t)td->managed_vt != (uintptr_t)object->info) {
        // This allows for cases when a vtable hook is being used to replace this pointer.
        if (IsBadReadPtr(td->managed_vt, sizeof(void*)) || *(sdk::RETypeDefinition**)td->managed_vt != td) {
            return false;
        }
    }

    if (td->type == nullptr) {
        return false;
    }

    if (IsBadReadPtr(td->type, sizeof(REType)) || td->type->name == nullptr) {
        return false;
    }

    if (IsBadReadPtr(td->type->name, sizeof(void*))) {
        return false;
    }
#elif TDB_VER > 49
    if (class_info->parentInfo != object->info) {
        // This allows for cases when a vtable hook is being used to replace this pointer.
        if (IsBadReadPtr(class_info->parentInfo, sizeof(void*)) || class_info->parentInfo->classInfo != class_info) {
            return false;
        }
    }

    if (class_info->type == nullptr) {
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

    static auto vm = sdk::VM::get();
    const auto tdef = (sdk::RETypeDefinition*)info->classInfo;
    
    if (&vm->types[tdef->get_index()] != (regenny::via::clr::VM::Type*)object->info) {
        return false;
    }

    /*if (info->type->classInfo != nullptr && info->type->classInfo != object->info) {
        return false;
    }*/
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

#if TDB_VER >= 71
    const auto td = get_type_definition(object);

    if (td == nullptr) {
        return nullptr;
    }

    return td->type;
#elif TDB_VER > 49
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

#if TDB_VER >= 71
    return get_type_definition(object)->get_vm_obj_type();
#elif TDB_VER >= 69
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

#if TDB_VER > 49
    const auto td = get_type_definition(object);
    auto size = td->get_size();
#else
    auto size = info->size;
#endif

    switch (get_vm_type(object)) {
    case via::clr::VMObjType::Array:
    {
        auto container = (::REArrayBase*)object;

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
