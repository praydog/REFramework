#include <utility/Scan.hpp>
#include <utility/Module.hpp>
#include <spdlog/spdlog.h>

#include "Memory.hpp"

namespace sdk {
namespace via {
namespace memory {
void* allocate(size_t size) {
    static decltype(via::memory::allocate)* allocate_fn = []() -> decltype(via::memory::allocate)* {
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

        auto fn = (decltype(via::memory::allocate)*)utility::calculate_absolute(*ref + 6);

        if (!fn) {
            spdlog::error("[via::memory::allocate] Failed to calculate allocate function!");
            return nullptr;
        }

        spdlog::info("[via::memory::allocate] Found allocate function at {:x}", (uintptr_t)fn);

        return fn;
    }();

    return allocate_fn(size);
}

void deallocate(void* ptr) {
    // In every RE Engine game, the deallocate function is the next function in the disassembly for some reason.
    static decltype(via::memory::deallocate)* deallocate_fn = []() -> decltype(via::memory::deallocate)* {
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

        // Scan until we hit a jmp.
        ref = utility::scan_opcode((uintptr_t)allocate_fn + 1, 50, 0xE9);

        if (!ref) {
            spdlog::error("[via::memory::deallocate] Failed to find deallocate function!");
            return nullptr;
        }

        auto fn = (decltype(via::memory::deallocate)*)*ref;

        spdlog::info("[via::memory::deallocate] Found deallocate function at {:x}", (uintptr_t)fn);

        return fn;
    }();

    deallocate_fn(ptr);
}
}
}
}