#include <windows.h>

#include <spdlog/spdlog.h>

#include <utility/Scan.hpp>
#include <utility/Module.hpp>

#include "../RETypeDB.hpp"

#include "RenderResource.hpp"

namespace sdk::renderer {
void RenderResource::add_ref() {
    _InterlockedIncrement((long*)&m_ref_count);
}

void RenderResource::release() {
    get_release_fn()(this);
}

RenderResource::ReleaseFn RenderResource::get_release_fn() {
    static ReleaseFn release_fn = []() -> ReleaseFn {
        spdlog::info("[RenderResource] Scanning for release function...");

        const auto capture_plane_t = sdk::find_type_definition("via.render.CapturePlane");

        if (capture_plane_t == nullptr) {
            spdlog::error("[RenderResource] Failed to find via.render.CapturePlane type definition!");
            return nullptr;
        }

        const auto reset_method = capture_plane_t->get_method("reset");

        if (reset_method == nullptr) {
            spdlog::error("[RenderResource] Failed to find via.render.CapturePlane.reset method!");

            const auto game = utility::get_executable();
            std::vector<std::string> landmark_patterns {
                "FF 50 ?", // call qword ptr [rax + ?]
                "F0 FF ? 08", // lock dec dword ptr [reg + 8]
                "BA 01 00 00 00", // mov edx, 1
                "E8 ? ? ? ?", // call ?
            };

            // Initial scan for a lock cmpxchg [reg + 8], reg instruction
            auto landmark = utility::find_landmark_sequence(game, "F0 0F B1 ? 08 75 ?", landmark_patterns, false);

            if (!landmark) {
                spdlog::error("[RenderResource] Failed to find landmark sequence!");
                return nullptr;
            }

            auto fn_start = utility::find_function_start_with_call(landmark->addr);
            
            if (!fn_start) {
                spdlog::error("[RenderResource] Failed to find function start!");
                return nullptr;
            }

            spdlog::info("[RenderResource] Found function start at {:x}", *fn_start);

            return (ReleaseFn)*fn_start;
        }

        const auto reset_method_addr = (uintptr_t)reset_method->get_function();

        // The first call instruction in the reset method is the release function.
        const auto ref = utility::scan_disasm(reset_method_addr, 50, "E8 ? ? ? ?");

        if (!ref) {
            spdlog::error("[RenderResource] Failed to find release function!");
            return nullptr;
        }

        const auto result = (ReleaseFn)utility::calculate_absolute(*ref + 1);

        spdlog::info("[RenderResource] Found release function at {:x}", (uintptr_t)result);

        return result;
    }();

    return release_fn;
}
}