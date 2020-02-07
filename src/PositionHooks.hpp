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

    void* update_camera_controller_hook_internal(void* a1, RopewayPlayerCameraController* camera_controller);
    static void* update_camera_controller_hook(void* a1, RopewayPlayerCameraController* camera_controller);

    void* update_camera_controller2_hook_internal(void* a1, RopewayPlayerCameraController* camera_controller);
    static void* update_camera_controller2_hook(void* a1, RopewayPlayerCameraController* camera_controller);

    std::unique_ptr<FunctionHook> m_update_transform_hook;
    std::unique_ptr<FunctionHook> m_update_camera_controller_hook;
    std::unique_ptr<FunctionHook> m_update_camera_controller2_hook;
};