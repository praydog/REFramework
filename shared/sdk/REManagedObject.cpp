#include <spdlog/spdlog.h>

#include "utility/Scan.hpp"
#include "utility/Module.hpp"

#include "ReClass.hpp"

#include "REManagedObject.hpp"
#include "GameIdentity.hpp"

#include "RETypeDefDispatch.hpp"
using namespace utility::re_type_accessor;

size_t REManagedObject::runtime_size() {
    // RE7 TDB49 has REManagedObject at 0x20 (enlarged REObject base with vtable).
    // All other supported games: 0x10. Modern RE7 (TDB70) is also 0x10.
    return 0x10;  // Stable across all supported games. Update if a future game changes it.
}

// Used all over the place. It may be one of the most referenced functions in a disassembler.
// According to my IDA script, it's the 4th most referenced function though sometimes it's 5th or 6th.
// Release usually has more references than add_ref.
// A dead giveaway is the comparison to byte ptr RCX + 0xE near the start of the function.
// Cheat sheet for functions to find these function(s) in:
// via.Transform::get_Children
static void (*add_ref_func)(::REManagedObject*) = nullptr;
static void (*release_func)(::REManagedObject*) = nullptr;

void REManagedObject::resolve_add_ref() {
    if (add_ref_func != nullptr) {
        return;
    }

    constexpr std::array<std::string_view, 4> possible_patterns{
        "40 ? 48 83 EC ? 8B 41 ? 48 8B ? 85 C0 0F ? ? ? ? ? 0F ? ? 0E", // RE2+
        "40 ? 48 83 EC ? 8B 41 ? 48 8B ? 85 C0 0F ? ? ? ? ? 80 ? 0E 00", // TDB73+/DD2+
        "48 89 ? ? ? 57 48 83 EC ? 0F ? ? 0E", // RE7 TDB49
        "41 57 41 56 41 55 41 54 56 57 55 53 48 83 EC ? 48 89 CE 8B 41 08 85 C0", // MHWILDS+ (or unoptimized compiler builds?)
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

void REManagedObject::resolve_release() {
    if (release_func != nullptr) {
        return;
    }

    // We also need to resolve add_ref
    // because we need to make sure we don't resolve release to the same function.
    resolve_add_ref();

    constexpr std::array<std::string_view, 4> possible_patterns{
        "40 53 48 83 EC ? 8B 41 08 48 8B D9 85 C0 0F", // RE2+
        "40 53 48 83 EC ? 8B 41 08 48 8B D9 48 83 C1 08 85 C0 78", // RE7
        "41 57 41 56 41 55 41 54 56 57 55 53 48 83 EC ? 48 8B 05 ? ? ? ? 48 31 E0 48 89 ? ? ? 8B 41 08 85 C0", // MHWILDS TU4+
        "41 57 41 56 41 55 41 54 56 57 55 53 48 83 EC ? 48 8B 05 ? ? ? ? 48 31 E0 48 89 44 24 30 8B 41 08", // MHWILDS+ (or unoptimized compiler builds?)
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

void REManagedObject::add_ref() {
    resolve_add_ref();

#if TDB_VER <= 49
    if ((int32_t)get_ref_count() >= 0) {
        _InterlockedIncrement((volatile long*)ref_count_ptr());
    } else {
        add_ref_func(this);
    }
#else
    add_ref_func(this);
#endif
}

void REManagedObject::release() {
    resolve_release();

    release_func(this);
}

std::vector<::REManagedObject*> REManagedObject::deserialize(const uint8_t* data, size_t size, bool add_references) {
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
                object->add_ref();
            }

            result.push_back(object);
        }
    }

    return result;
}

void REManagedObject::deserialize_native(const uint8_t* data, size_t size, const std::vector<::REManagedObject*>& objects) {
    if (!is_managed_object(this)) {
        return;
    }

    const auto tdef = get_type_definition();

    if (tdef == nullptr) {
        return;
    }

    const auto t = tdef->get_type();

    if (t == nullptr || get_fields(t) == nullptr || get_fields(t)->get_deserializer() == nullptr) {
        return;
    }

    struct DeserializeStream {
        uint8_t* head{nullptr};
        uint8_t* cur{nullptr};
        uint8_t* tail{nullptr};
        uintptr_t stackptr{0};
        uint8_t* stack[32]{0};
    }; static_assert(sizeof(DeserializeStream) == 0x120, "DeserializeStream is not the correct size");

    const auto deserializer = (void (*)(::REManagedObject*, DeserializeStream*, sdk::NativeArray<REManagedObject*>* objects))get_fields(t)->get_deserializer();

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

    deserializer(this, &stream, &objects_array);
}

bool REManagedObject::is_managed_object(void* address) {
    if (address == nullptr) {
        return false;
    }

    if (IsBadReadPtr(address, sizeof(void*))) {
        return false;
    }

    auto object = (::REManagedObject*)address;

    if (object->info == nullptr || IsBadReadPtr(object->info, sizeof(void*))) {
        return false;
    }

    auto class_info = object->info->get_class_info();

    if (class_info == nullptr || IsBadReadPtr(class_info, sizeof(void*))) {
        return false;
    }

    const auto& gi = sdk::GameIdentity::get();
    if (gi.tdb_ver() >= 71) {
        const auto td = (sdk::RETypeDefinition*)class_info;

        if ((uintptr_t)TDEF_FIELD(td, managed_vt) != (uintptr_t)object->info) {
            // This allows for cases when a vtable hook is being used to replace this pointer.
            if (IsBadReadPtr(TDEF_FIELD(td, managed_vt), sizeof(void*)) || *(sdk::RETypeDefinition**)TDEF_FIELD(td, managed_vt) != td) {
                return false;
            }
        }

        if (TDEF_FIELD(td, type) == nullptr) {
            return false;
        }

        if (IsBadReadPtr(TDEF_FIELD(td, type), REType::runtime_size()) || TDEF_FIELD(td, type)->get_type_name() == nullptr) {
            return false;
        }

        if (IsBadReadPtr(TDEF_FIELD(td, type)->get_type_name(), sizeof(void*))) {
            return false;
        }
    } else {
        auto td = (sdk::RETypeDefinition*)class_info;
        auto ci_parentInfo = td->get_managed_vt();
        auto ci_type = td->get_type();
        if (ci_parentInfo != object->info) {
            // This allows for cases when a vtable hook is being used to replace this pointer.
            if (IsBadReadPtr(ci_parentInfo, sizeof(void*)) || ci_parentInfo->get_class_info() != class_info) {
                return false;
            }
        }

        if (ci_type == nullptr) {
            return false;
        }

        if (IsBadReadPtr(ci_type, REType::runtime_size()) || ci_type->get_type_name() == nullptr) {
            return false;
        }

        if (IsBadReadPtr(ci_type->get_type_name(), sizeof(void*))) {
            return false;
        }
    }

    return true;
}

REType* REManagedObject::get_type() const {
    auto info_ptr = info;

    if (info_ptr == nullptr) {
        return nullptr;
    }

    const auto& gi = sdk::GameIdentity::get();
    if (gi.tdb_ver() >= 71) {
        const auto td = get_type_definition();

        if (td == nullptr) {
            return nullptr;
        }

        return TDEF_FIELD(td, type);
    } else {
        auto class_info = info_ptr->get_class_info();

        if (class_info == nullptr) {
            return nullptr;
        }

        return ((sdk::RETypeDefinition*)class_info)->get_type();
    }
}

sdk::RETypeDefinition* REManagedObject::get_type_definition() const {
    auto info_ptr = info;

    if (info_ptr == nullptr) {
        return nullptr;
    }

    return (sdk::RETypeDefinition*)info_ptr->get_class_info();
}

REType* REManagedObject::safe_get_type() const {
    return is_managed_object(const_cast<REManagedObject*>(this)) ? get_type() : nullptr;
}

std::string REManagedObject::get_type_name() const {
    auto t = get_type();

    if (t == nullptr) {
        return "";
    }

    return t->get_type_name();
}

bool REManagedObject::is_a(std::string_view name) const {
    for (auto t = get_type(); t != nullptr && t->get_type_name() != nullptr; t = get_super(t)) {
        if (name == t->get_type_name()) {
            return true;
        }
    }

    return false;
}

bool REManagedObject::is_a(REType* cmp) const {
    for (auto t = get_type(); t != nullptr && t->get_type_name() != nullptr; t = get_super(t)) {
        if (cmp == t) {
            return true;
        }
    }

    return false;
}

via::clr::VMObjType REManagedObject::get_vm_type() const {
    auto info_ptr = info;

    if (info_ptr == nullptr || info_ptr->get_class_info() == nullptr) {
        return via::clr::VMObjType::NULL_;
    }

    return get_type_definition()->get_vm_obj_type();
}

uint32_t REManagedObject::get_size() const {
    auto info_ptr = info;

    if (info_ptr == nullptr || info_ptr->get_class_info() == nullptr) {
        return 0;
    }

    const auto td = get_type_definition();
    auto size = td->get_size();

    switch (get_vm_type()) {
    case via::clr::VMObjType::Array:
    {
        auto container = (::REArrayBase*)this;

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
        size = sizeof(uint16_t) * (Address(const_cast<REManagedObject*>(this)).get(REManagedObject::runtime_size()).to<uint32_t>() + 1) + 0x14;
        break;
    case via::clr::VMObjType::Delegate:
        size = 0x10 * (Address(const_cast<REManagedObject*>(this)).get(REManagedObject::runtime_size()).to<uint32_t>() - 1) + 0x28;
        break;
    case via::clr::VMObjType::Object:
    default:
        break;
    }
    
    return size;
}

REVariableList* REManagedObject::get_variables(::REType* t) {
    return utility::re_type::get_variables(t);
}

REVariableList* REManagedObject::get_variables() const {
    return get_variables(get_type());
}

VariableDescriptor* REManagedObject::get_field_desc(std::string_view field) const {
    return utility::re_type::get_field_desc(get_type(), field);
}

FunctionDescriptor* REManagedObject::get_method_desc(std::string_view name) const {
    return utility::re_type::get_method_desc(get_type(), name);
}
