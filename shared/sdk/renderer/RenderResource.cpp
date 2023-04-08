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
            return nullptr;
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