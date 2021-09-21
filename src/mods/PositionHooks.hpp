#pragma once

#include "Mod.hpp"
#include "utility/FunctionHook.hpp"

class PositionHooks : public Mod {
public:
    PositionHooks();

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

    std::unique_ptr<FunctionHook> m_update_transform_hook;
    std::unique_ptr<FunctionHook> m_update_camera_controller_hook;
    std::unique_ptr<FunctionHook> m_update_camera_controller2_hook;
    std::unique_ptr<FunctionHook> m_gui_draw_hook;
    std::unique_ptr<FunctionHook> m_update_before_lock_scene_hook;
    std::unique_ptr<FunctionHook> m_lock_scene_hook;
    std::unique_ptr<FunctionHook> m_begin_rendering_hook;
    std::unique_ptr<FunctionHook> m_end_rendering_hook;
    std::unique_ptr<FunctionHook> m_wait_rendering_hook;
};