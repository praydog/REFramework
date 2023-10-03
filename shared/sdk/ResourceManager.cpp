#include <algorithm>
#include <spdlog/spdlog.h>
#include <hde64.h>

#include "utility/Scan.hpp"
#include "utility/Module.hpp"

#include "RETypeDB.hpp"
#include "ResourceManager.hpp"

namespace sdk {
// static definitions
decltype(ResourceManager::s_create_resource_fn) ResourceManager::s_create_resource_fn = nullptr;
decltype(ResourceManager::s_create_resource_reference) ResourceManager::s_create_resource_reference = 0;
decltype(ResourceManager::s_create_userdata_fn) ResourceManager::s_create_userdata_fn = nullptr;
decltype(Resource::s_add_ref_fn) Resource::s_add_ref_fn = nullptr;
decltype(Resource::s_release_fn) Resource::s_release_fn = nullptr;

void Resource::add_ref() {
    ResourceManager::update_pointers();

    s_add_ref_fn(this);
}

void Resource::release() {
    ResourceManager::update_pointers();

    s_release_fn(this);
}

REManagedObject* Resource::create_holder(sdk::RETypeDefinition* t) {
    if (t == nullptr) {
        return nullptr;
    }

    static const auto resource_holder_t = sdk::find_type_definition("via.ResourceHolder");

    if (resource_holder_t == nullptr || !t->is_a(resource_holder_t)) {
        return nullptr;
    }

    auto instance = t->create_instance_full();

    if (instance == nullptr) {
        return nullptr;
    }

    this->add_ref();
    *(sdk::Resource**)((uintptr_t)instance + sizeof(::REManagedObject)) = this;

    return instance;
}

ResourceManager* ResourceManager::get() {
    return (ResourceManager*)sdk::get_native_singleton("via.ResourceManager");
}

sdk::Resource* ResourceManager::create_resource(void* type_info, std::wstring_view name) {
    update_pointers();

    return s_create_resource_fn(this, type_info, name.data());
}

intrusive_ptr<sdk::ManagedObject> ResourceManager::create_userdata(void* type_info, std::wstring_view name) {
    update_pointers();

    intrusive_ptr<sdk::ManagedObject> out{};
    s_create_userdata_fn(this, &out, type_info, name.data());

    return out;
}

void ResourceManager::update_pointers() {
    if (s_create_resource_fn == nullptr) {
        spdlog::info("[ResourceManager::create_resource] Finding function...");

        const auto mod = utility::get_executable();
        const auto mod_size = *utility::get_module_size(mod);
        const auto mod_end = (uintptr_t)mod + mod_size;
        const auto string_ptr = utility::scan_string(mod, L"systems/rendering/AmbientBRDF.tex"); // common string that is used in all the games

        if (!string_ptr) {
            spdlog::error("[ResourceManager::create_resource] Failed to find string!");
            return;
        }

        // find a string reference that is preceded by lea r8, string_ptr
        const auto string_reference = utility::scan_relative_reference_strict(mod, *string_ptr, "4C 8D 05");

        if (!string_reference) {
            spdlog::error("[ResourceManager::create_resource] Failed to find string reference!");
            return;
        }

        // use HDE to disasm *string_reference - 3 and disasm forward a bit
        // to find a call instruction which is the function we want
        auto ip = *string_reference - 3;
        bool found = false;

        for (auto i = 0; i < 10; ++i) {
            hde64s hde{};
            auto len = hde64_disasm((void*)ip, &hde);

            if (hde.opcode == 0xE8) { // call
                found = true;
                break;
            }

            ip += len;
        }

        if (found) {
            s_create_resource_fn = (decltype(s_create_resource_fn))utility::calculate_absolute(ip + 1);
            s_create_resource_reference = ip;
            Resource::update_pointers();
            spdlog::info("[ResourceManager::create_resource] Found function at {:x}", (uintptr_t)s_create_resource_fn);
            
            // now find create_userdata, using the previous function as a reference to ignore
            // since they both have the same pattern at the start of the function
            const auto valid_patterns = {
                "66 83 F8 40 75 ? C6",
                "66 83 F8 40 75 ? 48"
            };

            bool found = false;
            bool exception_directory_maybe_removed = false;

            for (const auto& pat : valid_patterns) {
                for (auto ref = utility::scan(mod, pat); ref.has_value(); ref = utility::scan(*ref + 1, (mod_end - (*ref + 1)) - 100, pat)) {
                    auto func = utility::find_function_start_with_call(*ref);

                    if (func && *func != (uintptr_t)s_create_resource_fn) {
                        if (std::abs((ptrdiff_t)(*func - (uintptr_t)s_create_resource_fn)) < 0x50) {
                            spdlog::info("Exception directory may have been removed, falling back to int3 scan");
                            exception_directory_maybe_removed = true;
                            continue;
                        }

                        if (exception_directory_maybe_removed) {
                            func = utility::scan_reverse(*func, 0x100, "CC CC CC");

                            if (func) {
                                *func += 3;
                            } else {
                                func = utility::scan_reverse(*ref, 0x100, "4C 89 4C");
                            }
                        }

                        found = true;
                        s_create_userdata_fn = (decltype(s_create_userdata_fn))*func;
                        break;
                    }
                }

                if (found) {
                    break;
                }
            }

            if (found) {
                spdlog::info("[ResourceManager::create_userdata] Found function at {:x}", (uintptr_t)s_create_userdata_fn);
            } else {
                spdlog::error("[ResourceManager::create_userdata] Failed to find function!");
            }
        } else {
            spdlog::error("[ResourceManager::create_resource] Failed to find function!");
            return;
        }
    }
}

void Resource::update_pointers() {
    if (s_add_ref_fn != nullptr && s_release_fn != nullptr) {
        return;
    }

    if (ResourceManager::s_create_resource_reference == 0) {
        ResourceManager::update_pointers();

        if (ResourceManager::s_create_resource_reference == 0) {
            spdlog::error("[Resource::update_pointers] ResourceManager::s_create_resource_reference is 0; insufficient data to update pointers");
            return;
        }
    }

    spdlog::info("[Resource::update_pointers] Finding function...");

    static auto locate_add_ref_or_release = [](uintptr_t start) -> std::optional<uintptr_t> {
        // Find the first call, can either be add_ref or release
        // depending on compiler randomness
        const auto first_call = utility::scan_opcode(start, 30, 0xE8);

        if (!first_call) {
            spdlog::error("[Resource::update_pointers] Failed to find first call!");
            return std::nullopt;
        }

        const auto first_function = utility::calculate_absolute(*first_call + 1);
        
        // Find the jmp instruction inside the function
        const auto jmp_instruction = utility::scan_opcode(first_function, 5, 0xE9);
        const auto jmp_ptr = jmp_instruction.has_value() ? utility::calculate_absolute(*jmp_instruction + 1) : first_function;
        const auto jmp_disasm = utility::decode_one((uint8_t*)jmp_ptr);

        //spdlog::info("Flags: {:x}", jmp_disasm.flags);
        //spdlog::info("modrm: {:x}", jmp_disasm.modrm);

        std::string buffer{};
        buffer.resize(256);
        if (jmp_disasm) {
            NdToText(&*jmp_disasm, jmp_ptr, buffer.size(), buffer.data());
            std::transform(buffer.begin(), buffer.end(), buffer.begin(), [](char c) { return std::tolower(c); });

            // trim all repeating spaces into a single space
            buffer.erase(std::unique(buffer.begin(), buffer.end(), [](char a, char b) { return a == b && a == ' '; }), buffer.end());
        }

        // if the first instruction is lock xadd/lock inc or lea rcx, [rdx+whatever], it's the add_ref function
        if ((jmp_disasm && std::string_view{buffer.data()}.starts_with("lock") || std::string_view{buffer.data()}.starts_with("lea rcx, ["))) {
            s_add_ref_fn = (decltype(s_add_ref_fn))first_function;
            spdlog::info("[Resource::update_pointers] Found add_ref function at {:x}", (uintptr_t)s_add_ref_fn);
        } else {
            s_release_fn = (decltype(s_release_fn))first_function;
            spdlog::info("[Resource::update_pointers] Found release function at {:x}", (uintptr_t)s_release_fn);
        }

        return first_call;
    };

    constexpr size_t CALL_INSN_SIZE = 5;

    auto first_call = locate_add_ref_or_release(ResourceManager::s_create_resource_reference + CALL_INSN_SIZE); // first pass finds add_ref or release
    locate_add_ref_or_release(*first_call + CALL_INSN_SIZE); // second pass finds add_ref or release
}
}