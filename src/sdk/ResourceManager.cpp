#include <spdlog/spdlog.h>
#include <hde64.h>

#include "../utility/Scan.hpp"

#include "../REFramework.hpp"
#include "RETypeDB.hpp"
#include "ResourceManager.hpp"

namespace sdk {
// static definitions
decltype(ResourceManager::s_create_resource_fn) ResourceManager::s_create_resource_fn = nullptr;
decltype(ResourceManager::s_create_resource_reference) ResourceManager::s_create_resource_reference = 0;
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

ResourceManager* ResourceManager::get() {
    return (ResourceManager*)sdk::get_native_singleton("via.ResourceManager");
}

sdk::Resource* ResourceManager::create_resource(void* type_info, std::wstring_view name) {
    update_pointers();

    return s_create_resource_fn(this, type_info, name.data());
}

void ResourceManager::update_pointers() {
    if (s_create_resource_fn == nullptr) {
        spdlog::info("[ResourceManager::create_resource] Finding function...");

        const auto mod = g_framework->get_module().as<HMODULE>();
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

    static auto find_opcode = [](uintptr_t ip, size_t num_instructions, uint8_t opcode) -> std::optional<uintptr_t> {
        for (size_t i = 0; i < num_instructions; ++i) {
            hde64s hde{};
            auto len = hde64_disasm((void*)ip, &hde);

            if (hde.opcode == opcode) { // call
                return ip;
            }

            ip += len;
        }

        return std::nullopt;
    };

    static auto disasm = [](uintptr_t ip) -> hde64s {
        hde64s hde{};
        hde64_disasm((void*)ip, &hde);
        return hde;
    };

    static auto locate_add_ref_or_release = [](uintptr_t start) -> std::optional<uintptr_t> {
        // Find the first call, can either be add_ref or release
        // depending on compiler randomness
        auto first_call = find_opcode(start, 30, 0xE8);

        if (!first_call) {
            spdlog::error("[Resource::update_pointers] Failed to find first call!");
            return std::nullopt;
        }

        auto first_function = utility::calculate_absolute(*first_call + 1);
        
        // Find the jmp instruction inside the function
        auto jmp_instruction = find_opcode(first_function, 5, 0xE9);

        if (!jmp_instruction) {
            spdlog::error("[Resource::update_pointers] Failed to find jmp instruction!");
            return std::nullopt;
        }

        auto jmp_ptr = utility::calculate_absolute(*jmp_instruction + 1);

        auto jmp_disasm = disasm(jmp_ptr);

        spdlog::info("Flags: {:x}", jmp_disasm.flags);
        spdlog::info("modrm: {:x}", jmp_disasm.modrm);

        // if the first instruction is lock xadd/lock inc or lea rcx, [rdx+whatever], it's the add_ref function
        if (jmp_disasm.p_lock == 0xF0 || memcmp((void*)jmp_ptr, "\x48\x8D", 2) == 0) {
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