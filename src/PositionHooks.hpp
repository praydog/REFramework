#pragma once

#include "Mod.hpp"
#include "FunctionHook.hpp"

class PositionHooks : public Mod {
public:
    PositionHooks();

protected:
    void* updateTransformHook_Internal(RETransform* t, uint8_t a2, uint32_t a3);
    static void* updateTransformHook(RETransform* t, uint8_t a2, uint32_t a3);

    void* updateCameraControllerHook_Internal(void* a1, RopewayPlayerCameraController* cameraController);
    static void* updateCameraControllerHook(void* a1, RopewayPlayerCameraController* cameraController);

    void* updateCameraController2Hook_Internal(void* a1, RopewayPlayerCameraController* cameraController);
    static void* updateCameraController2Hook(void* a1, RopewayPlayerCameraController* cameraController);

    std::unique_ptr<FunctionHook> m_updateTransformHook;
    std::unique_ptr<FunctionHook> m_updateCameraControllerHook;
    std::unique_ptr<FunctionHook> m_updateCameraController2Hook;
};