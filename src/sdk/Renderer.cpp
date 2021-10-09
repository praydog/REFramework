#include "../utility/Scan.hpp"
#include "../REFramework.hpp"

#include "Application.hpp"
#include "RETypeDB.hpp"

#include "Renderer.hpp"

namespace sdk {
namespace renderer {
RenderLayer* RenderLayer::add_layer(::REType* layer_type, uint32_t priority, uint8_t offset) {
    // can be found inside addSceneView
    static RenderLayer* (*add_layer_fn)(RenderLayer*, ::REType*, uint32_t, uint8_t) = nullptr;
    
    if (add_layer_fn == nullptr) {
        spdlog::info("[Renderer] Finding RenderLayer::AddLayer");

        const auto mod = g_framework->get_module().as<HMODULE>();
        
        auto ref = utility::scan(mod, "41 B8 00 00 00 05 48 8B F8 E8 ? ? ? ?"); // mov r8d, 5000000h; call add_layer

        if (!ref) {
            // Fallback pattern
            ref = utility::scan(mod, "41 B8 00 00 00 05 48 89 C7 E8 ? ? ? ?"); // mov r8d, 5000000h; call add_layer

            if (!ref) {
                spdlog::error("[Renderer] Failed to find add_layer");
                return nullptr;
            }
        }

        add_layer_fn = (decltype(add_layer_fn))utility::calculate_absolute(*ref + 10);

        if (add_layer_fn == nullptr || IsBadReadPtr(add_layer_fn, sizeof(add_layer_fn))) {
            spdlog::error("[Renderer] Failed to calculate add_layer");
            return nullptr;
        }

        spdlog::info("[Renderer] RenderLayer::AddLayer: {:x}", (uintptr_t)add_layer_fn);
    }

    return add_layer_fn(this, layer_type, priority, offset);
}

sdk::NativeArray<RenderLayer*>& RenderLayer::get_layers() {
    static uint32_t layers_offset = 0;

    if (layers_offset == 0) {
        spdlog::info("[Renderer] Finding RenderLayer::layers");

        const auto root_layer = sdk::renderer::get_root_layer();
        
        if (root_layer == nullptr) {
            spdlog::error("[Renderer] Failed to find root layer");
            throw std::runtime_error("[Renderer] Failed to find root layer");
        }
        
        // Scan through the root layer for a pointer to a RenderLayer object
        for (auto i = 0; i < 0x500; i += sizeof(void*)) {
            auto ptr = *(RenderLayer***)((uintptr_t)root_layer + i);

            if (ptr == nullptr || IsBadReadPtr(ptr, sizeof(ptr))) {
                continue;
            }

            const auto potential_layer = *ptr;

            if (potential_layer == nullptr || IsBadReadPtr(potential_layer, sizeof(potential_layer))) {
                continue;
            }

            if (!utility::re_managed_object::is_managed_object(potential_layer)) {
                continue;
            }

            if (utility::re_managed_object::is_a(potential_layer, "via.render.RenderLayer")) {
                layers_offset = i;
                break;
            }
        }

        spdlog::info("[Renderer] RenderLayer::layers: {:x}", layers_offset);
    }

    return *(sdk::NativeArray<RenderLayer*>*)((uintptr_t)this + layers_offset);
}

RenderLayer* RenderLayer::find_layer(::REType* layer_type) {
    const auto& layers = get_layers();

    for (auto layer : layers) {
        if (layer->info == nullptr || layer->info->classInfo == nullptr) {
            continue;
        }

        const auto t = utility::re_managed_object::get_type(layer);

        if (t == layer_type) {
            return layer;
        }
    }

    return nullptr;
}

RenderLayer* RenderLayer::get_parent() {
    return sdk::call_object_func<RenderLayer*>(this, "get_Parent", sdk::get_thread_context(), this);
}

RenderLayer* RenderLayer::find_parent(::REType* layer_type) {
    for (auto parent = get_parent(); parent != nullptr; parent = parent->get_parent()) {
        if (parent->info == nullptr || parent->info->classInfo == nullptr) {
            break;
        }

        const auto t = utility::re_managed_object::get_type(parent);

        if (t == layer_type) {
            return parent;
        }
    }

    return nullptr;
}

void* get_renderer() {
    return sdk::get_native_singleton("via.render.Renderer");
}

void wait_rendering() {
    static auto wait_rendering_entry = sdk::Application::get()->get_function("WaitRendering");

    return wait_rendering_entry->func(wait_rendering_entry->entry);
}

void begin_rendering() {
    static auto begin_rendering_entry = sdk::Application::get()->get_function("BeginRendering");

    return begin_rendering_entry->func(begin_rendering_entry->entry);
}

void end_rendering() {
    static auto end_rendering_entry = sdk::Application::get()->get_function("EndRendering");

    return end_rendering_entry->func(end_rendering_entry->entry);
}

void begin_update_primitive() {
    static auto begin_update_primitive_entry = sdk::Application::get()->get_function("BeginUpdatePrimitive");

    return begin_update_primitive_entry->func(begin_update_primitive_entry->entry);
}

void update_primitive() {
    static auto update_primitive_entry = sdk::Application::get()->get_function("UpdatePrimitive");

    return update_primitive_entry->func(update_primitive_entry->entry);
}

void end_update_primitive() {
    static auto end_update_primitive_entry = sdk::Application::get()->get_function("EndUpdatePrimitive");

    return end_update_primitive_entry->func(end_update_primitive_entry->entry);
}

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

RenderLayer* get_root_layer() {
    auto renderer = sdk::get_native_singleton("via.render.Renderer");

    if (renderer == nullptr) {
        spdlog::error("[Renderer] Failed to find renderer");
        return nullptr;
    }

    static uint32_t root_layer_offset = 0;

    if (root_layer_offset == 0) {
        spdlog::info("[Renderer] Finding root_layer_offset");

        auto get_output_layer_fn = sdk::find_native_method("via.render.Renderer", "getOutputLayer");

        if (get_output_layer_fn == nullptr) {
            spdlog::error("[Renderer] Failed to find getOutputLayer");
            return nullptr;
        }

        // Resolve the jmp to the real function
        if (((uint8_t*)get_output_layer_fn)[0] == 0xE9) {
            get_output_layer_fn = (decltype(get_output_layer_fn))utility::calculate_absolute((uintptr_t)get_output_layer_fn + 1);
        }

        // Find the offset to the root layer (RE3, RE8)
        auto ref = utility::scan((uintptr_t)get_output_layer_fn, 0x100, "48 8B 81 ? ? ? ?");

        if (!ref) {
            // Fallback pattern to scan for (RE2)
            ref = utility::scan((uintptr_t)get_output_layer_fn, 0x100, "4C 8B 80 ? ? ? ?");

            if (!ref) {
                spdlog::error("[Renderer] Failed to find root_layer_offset");
                return nullptr;
            }
        }

        root_layer_offset = *(uint32_t*)(*ref + 3);

        spdlog::info("[Renderer] root_layer_offset: {:x}", root_layer_offset);
    }

    return *(RenderLayer**)((uintptr_t)renderer + root_layer_offset);
}

RenderLayer* find_layer(::REType* layer_type) {
    auto renderer = sdk::get_native_singleton("via.render.Renderer");

    if (renderer == nullptr) {
        spdlog::error("[Renderer] Failed to find renderer");
        return nullptr;
    }

    static uint32_t layers_offset = 0;

    // Scan through the renderer object to find a RenderLayer pointer
    if (layers_offset == 0) {
        spdlog::info("[Renderer] Finding layers_offset");

        for (uint32_t i = 0; i < 0x2000; i += sizeof(void*)) {
            const auto ptr = *(REManagedObject**)((uintptr_t)renderer + i);

            if (ptr == nullptr) {
                continue;
            }

            if (!utility::re_managed_object::is_managed_object(ptr)) {
                continue;
            }

            if (utility::re_managed_object::is_a(ptr, "via.render.RenderLayer")) {
                layers_offset = i;
                break;
            }
        }

        if (layers_offset == 0) {
            spdlog::error("[Renderer] Failed to find layers_offset");
            return nullptr;
        }

        spdlog::info("[Renderer] layers_offset: {:x}", layers_offset);
    }

    const auto& layers = *(std::array<RenderLayer*, 256>*)((uintptr_t)renderer + layers_offset);

    for (auto& layer : layers) {
        if (layer->info == nullptr || layer->info->classInfo == nullptr) {
            continue;
        }

        const auto t = utility::re_managed_object::get_type(layer);

        if (t == layer_type) {
            return layer;
        }
    }

    return nullptr;
}

sdk::renderer::layer::Output* get_output_layer() {
    auto renderer_t = sdk::RETypeDB::get()->find_type("via.render.Renderer");

    if (renderer_t == nullptr) {
        spdlog::error("[Renderer] Failed to find via.render.Renderer type");
        return nullptr;
    }

    return sdk::call_object_func<sdk::renderer::layer::Output*>(nullptr, renderer_t, "getOutputLayer", sdk::get_thread_context(), nullptr);
}

void*& layer::Output::get_present_state() {
    static uint32_t output_target_offset = 0;

    if (output_target_offset == 0) {
        spdlog::info("[Renderer] Finding output_target_offset");

        auto get_scene_view_fn = sdk::find_native_method("via.render.Renderer", "get_SceneView");

        if (get_scene_view_fn == nullptr) {
            spdlog::error("[Renderer] Failed to find get_SceneView");
            return *(void**)((uintptr_t)this + 0);
        }

        // Resolve the jmp to the real function
        if (((uint8_t*)get_scene_view_fn)[0] == 0xE9) {
            get_scene_view_fn = (decltype(get_scene_view_fn))utility::calculate_absolute((uintptr_t)get_scene_view_fn + 1);
        }

        // Find the offset to the output target
        // First instruction is a mov, so we don't need to pattern scan for it
        output_target_offset = *(uint8_t*)((uintptr_t)get_scene_view_fn + 3);

        spdlog::info("[Renderer] output_target_offset: {:x}", output_target_offset);
    }

    return *(void**)((uintptr_t)this + output_target_offset);
}

REManagedObject*& layer::Output::get_scene_view() {
    static uint32_t scene_view_offset = 0;

    if (scene_view_offset == 0) {
        spdlog::info("[Renderer] Finding scene_view_offset");

        // because if this is a manually created output layer,
        // we might not have the scene view and output state set up yet
        auto top_output_layer = sdk::renderer::get_output_layer();

        if (top_output_layer == nullptr) {
            spdlog::error("[Renderer] Failed to find top_output_layer");
            return *(REManagedObject**)((uintptr_t)this + 0);
        }

        // Call get_SceneView so we can get a scene view
        // to scan the object for
        const auto scene_view = sdk::call_object_func<void*>(top_output_layer, "get_SceneView", sdk::get_thread_context(), top_output_layer);
        const auto output_target = top_output_layer->get_present_state();

        if (scene_view == nullptr) {
            spdlog::error("[Renderer] Failed to find scene_view");
            return *(REManagedObject**)((uintptr_t)this + 0);
        }

        if (output_target == nullptr) {
            spdlog::error("[Renderer] Failed to find output_target");
            return *(REManagedObject**)((uintptr_t)this + 0);
        }

        // Find the offset to the scene view
        for (auto i = 0; i < 0x1000; i += sizeof(void*)) {
            if (*(void**)((uintptr_t)output_target + i) == scene_view) {
                scene_view_offset = i;
                break;
            }
        }

        spdlog::info("[Renderer] scene_view_offset: {:x}", scene_view_offset);
    }

    return *(REManagedObject**)((uintptr_t)get_present_state() + scene_view_offset);
}
}
}