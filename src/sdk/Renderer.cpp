#include "../utility/Scan.hpp"
#include "../REFramework.hpp"

#include "Renderer.hpp"

namespace sdk {
namespace renderer {
void add_scene_view(void* scene_view) {
    static void (*add_scene_view_fn)(void*) = nullptr;

    if (add_scene_view_fn == nullptr) {
        spdlog::info("[Renderer] Finding add_scene_view_fn");

        /*
        .text:0000000142952B16 41 8B 86 B4 0A 00 00                          mov     eax, [r14+0AB4h]
        .text:0000000142952B1D 83 E0 01                                      and     eax, 1
        .text:0000000142952B20 48 83 C0 1C                                   add     rax, 1Ch
        .text:0000000142952B24 48 69 C0 88 00 00 00                          imul    rax, 88h
        .text:0000000142952B2B 49 03 C6                                      add     rax, r14
        .text:0000000142952B2E 49 89 86 F0 0F 00 00                          mov     [r14+0FF0h], rax
        .text:0000000142952B35 48 8B 3D 4C 0C 39 06                          mov     rdi, cs:g_scene_manager
        .text:0000000142952B3C 48 8B 5F 20                                   mov     rbx, [rdi+20h]
        .text:0000000142952B40 48 8B CB                                      mov     rcx, rbx        ; lpCriticalSection
        .text:0000000142952B43 FF 15 67 E7 F6 01                             call    cs:EnterCriticalSection
        .text:0000000142952B49 45 33 C9                                      xor     r9d, r9d
        .text:0000000142952B4C 4C 8D 05 8D 0F 00 00                          lea     r8, addSceneView(via::SceneView*)
        */
        // String refs in the function containing this pattern:
        // L"Renderer::DelayEndTask"
        // L"Renderer::DelayReleaseTask"
        const auto mod = g_framework->get_module().as<HMODULE>();
        auto ref = utility::scan(mod, "4C 8D 05 ? ? ? ? 48 8D ? ? 48 8D ? 08 E8 ? ? ? ? 48 ? ? FF 15");

        if (!ref) {
            spdlog::error("[Renderer] Failed to find add_scene_view_fn");
            return;
        }

        add_scene_view_fn = (decltype(add_scene_view_fn))utility::calculate_absolute(*ref + 3);

        spdlog::info("[Renderer] add_scene_view_fn: {:x}", (uintptr_t)add_scene_view_fn);
    }

    add_scene_view_fn(scene_view);
}

void remove_scene_view(void* scene_view) {
    static void (*remove_scene_view_fn)(void*) = nullptr;

    if (remove_scene_view_fn == nullptr) {
        spdlog::info("[Renderer] Finding remove_scene_view_fn");

        // Almost the same as add_scene_view pattern, is set up right after add_scene_view
        const auto mod = g_framework->get_module().as<HMODULE>();
        auto ref = utility::scan(mod, "4C 8D 05 ? ? ? ? 48 8D ? ? ? 48 8D ? 28 E8 ? ? ? ? 48 ? ? FF 15");

        if (!ref) {
            spdlog::error("[Renderer] Failed to find remove_scene_view_fn");
            return;
        }

        remove_scene_view_fn = (decltype(remove_scene_view_fn))utility::calculate_absolute(*ref + 3);

        spdlog::info("[Renderer] remove_scene_view_fn: {:x}", (uintptr_t)remove_scene_view_fn);
    }

    remove_scene_view_fn(scene_view);
}
}
}