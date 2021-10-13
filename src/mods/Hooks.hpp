#pragma once

#include "Mod.hpp"
#include "utility/FunctionHook.hpp"

class Hooks : public Mod {
public:
    Hooks();

    std::string_view get_name() const override { return "PositionHooks"; };
    std::optional<std::string> on_initialize() override;

protected:
    void* update_transform_hook_internal(RETransform* t, uint8_t a2, uint32_t a3);
    static void* update_transform_hook(RETransform* t, uint8_t a2, uint32_t a3);

    void* update_camera_controller_hook_internal(void* ctx, RopewayPlayerCameraController* camera_controller);
    static void* update_camera_controller_hook(void* ctx, RopewayPlayerCameraController* camera_controller);

    void* update_camera_controller2_hook_internal(void* ctx, RopewayPlayerCameraController* camera_controller);
    static void* update_camera_controller2_hook(void* ctx, RopewayPlayerCameraController* camera_controller);

    void* gui_draw_hook_internal(REComponent* gui_element, void* primitive_context);
    static void* gui_draw_hook(REComponent* gui_element, void* primitive_context);

    void update_before_lock_scene_hook_internal(void* ctx);
    static void update_before_lock_scene_hook(void* ctx);

    void lock_scene_hook_internal(void* entry);
    static void lock_scene_hook(void* entry);

    void begin_rendering_hook_internal(void* entry);
    static void begin_rendering_hook(void* entry);

    void end_rendering_hook_internal(void* entry);
    static void end_rendering_hook(void* entry);

    void wait_rendering_hook_internal(void* entry);
    static void wait_rendering_hook(void* entry);

    void lightshaft_draw_hook_internal(void* shaft, void* render_context);
    static void lightshaft_draw_hook(void* shaft, void* render_context);
    
    void global_application_entry_hook_internal(void* entry, const char* name, size_t hash);
    static void global_application_entry_hook(void* entry, const char* name, size_t hash);

private:
    std::optional<std::string> hook_update_transform();
    std::optional<std::string> hook_update_camera_controller();
    std::optional<std::string> hook_update_camera_controller2();
    std::optional<std::string> hook_gui_draw();
    std::optional<std::string> hook_update_before_lock_scene();
    std::optional<std::string> hook_lightshaft_draw();

    // Utility function for hooking function entries in via.Application
    std::optional<std::string> hook_application_entry(std::string name, std::unique_ptr<FunctionHook>& hook, void (*hook_fn)(void*));
    std::optional<std::string> hook_all_application_entries();

    #define HOOK_LAMBDA(func) [&]() -> std::optional<std::string> { return this->func(); }

    std::vector<std::function<std::optional<std::string>()>> m_hook_list{
        HOOK_LAMBDA(hook_update_transform),
        HOOK_LAMBDA(hook_update_camera_controller),
        HOOK_LAMBDA(hook_update_camera_controller2),
        HOOK_LAMBDA(hook_gui_draw),
#ifndef RE7
#ifndef MHRISE
        HOOK_LAMBDA(hook_update_before_lock_scene),
        HOOK_LAMBDA(hook_lightshaft_draw),
#endif
#endif
        HOOK_LAMBDA(hook_all_application_entries),
    };

protected:
    std::unique_ptr<FunctionHook> m_update_transform_hook;
    std::unique_ptr<FunctionHook> m_update_camera_controller_hook;
    std::unique_ptr<FunctionHook> m_update_camera_controller2_hook;
    std::unique_ptr<FunctionHook> m_gui_draw_hook;
    std::unique_ptr<FunctionHook> m_update_before_lock_scene_hook;
    std::unique_ptr<FunctionHook> m_lightshaft_draw_hook;

    std::unordered_map<const char*, void (*)(void*)> m_application_entry_hooks;
};