#include <utility/Scan.hpp>
#include <utility/Module.hpp>
#include <spdlog/spdlog.h>

#include "Memory.hpp"

namespace sdk {
namespace memory {
void* allocate(size_t size, bool zero_memory) {
    using allocate_fn_t = void* (*)(size_t);
    static allocate_fn_t allocate_fn = []() -> allocate_fn_t {
        spdlog::info("[via::memory::allocate] Finding allocate function...");

        // this pattern literally works back to the very first version of the RE Engine!
        // it is within the startup function that creates the window/application
        // Relevant string references:
        // "RE ENGINE [%ls] %ls port:%3d"
        auto ref = utility::scan(utility::get_executable(), "B9 ? ? ? ? E8 ? ? ? ? 45 33 F6 48 85 C0");

        if (!ref) {
            spdlog::error("[via::memory::allocate] Failed to find allocate function!");
            return nullptr;
        }

        spdlog::info("[via::memory::allocate] Ref {:x}", (uintptr_t)*ref);

        auto fn = (allocate_fn_t)utility::calculate_absolute(*ref + 6);

        if (!fn) {
            spdlog::error("[via::memory::allocate] Failed to calculate allocate function!");
            return nullptr;
        }

        spdlog::info("[via::memory::allocate] Found allocate function at {:x}", (uintptr_t)fn);

        return fn;
    }();

    auto result = allocate_fn(size);

    if (zero_memory && result != nullptr) {
        memset(result, 0, size);
    }

    return result;
}

void deallocate(void* ptr) {
    // In every RE Engine game, the deallocate function is the next function in the disassembly for some reason.
    static decltype(sdk::memory::deallocate)* deallocate_fn = []() -> decltype(sdk::memory::deallocate)* {
        spdlog::info("[via::memory::deallocate] Finding deallocate function...");

        // this pattern literally works back to the very first version of the RE Engine!
        // it is within the startup function that creates the window/application
        // Relevant string references:
        // "RE ENGINE [%ls] %ls port:%3d"
        auto ref = utility::scan(utility::get_executable(), "B9 ? ? ? ? E8 ? ? ? ? 45 33 F6 48 85 C0");

        if (!ref) {
            spdlog::error("[via::memory::deallocate] Failed to find allocate function!");
            return nullptr;
        }

        auto allocate_fn = utility::calculate_absolute(*ref + 6);

        if (!allocate_fn) {
            spdlog::error("[via::memory::deallocate] Failed to calculate allocate function!");
            return nullptr;
        }

        spdlog::info("[via::memory::deallocate] Found allocate function at {:x}", (uintptr_t)allocate_fn);

        const auto decoded_insn = utility::decode_one((uint8_t*)allocate_fn);
        const auto first_insn_size = decoded_insn.has_value() ? decoded_insn->Length : 1;

        // Scan until we hit a jmp.
        ref = utility::scan_opcode((uintptr_t)allocate_fn + first_insn_size, 50, 0xE9);

        if (!ref) {
            spdlog::error("[via::memory::deallocate] Failed to find deallocate function!");
            return nullptr;
        }

        auto fn = (decltype(sdk::memory::deallocate)*)*ref;

        spdlog::info("[via::memory::deallocate] Found deallocate function at {:x}", (uintptr_t)fn);

        return fn;
    }();

    deallocate_fn(ptr);
}

// so this is a bit strange that we need the old size
// but its because we dont know the size of the memory block as we havent mapped out the memory allocator
void* reallocate(void* ptr, size_t old_size, size_t size) {
    if (ptr == nullptr) {
        return allocate(size);
    }

    if (old_size == size) {
        return ptr;
    }

    // There is no function for this so we have to do it manually
    auto new_mem = allocate(size);

    const auto final_size = std::min<size_t>(old_size, size);
    memcpy(new_mem, ptr, final_size);

    deallocate(ptr);

    return new_mem;
}
}
}
