#include "REFramework.hpp"

#include "PositionHooks.hpp"

PositionHooks* g_hook = nullptr;

PositionHooks::PositionHooks() {
    g_hook = this;

    // Can be found by breakpointing RETransform's worldTransform
    m_updateTransformHook = std::make_unique<FunctionHook>(Address(GetModuleHandle(0)).get(0xEE0DDE0), &updateTransformHook);

    // Can be found by breakpointing camera controller's worldPosition
    m_updateCameraControllerHook = std::make_unique<FunctionHook>(Address(GetModuleHandle(0)).get(0xB4685A0), &updateCameraControllerHook);

    spdlog::info("PositionHooks");
}

void* PositionHooks::updateTransformHook_Internal(RETransform* t) {
    auto ret = m_updateTransformHook->getOriginal<decltype(updateTransformHook)>()(t);

    for (auto& mod : g_framework->getMods().getMods()) {
        mod->onUpdateTransform(t);
    }

    return ret;
}

void* PositionHooks::updateTransformHook(RETransform* t) {
    return g_hook->updateTransformHook_Internal(t);
}

void* PositionHooks::updateCameraControllerHook_Internal(void* a1, RopewayPlayerCameraController* cameraController) {
    auto ret = m_updateCameraControllerHook->getOriginal<decltype(updateCameraControllerHook)>()(a1, cameraController);

    for (auto& mod : g_framework->getMods().getMods()) {
        mod->onUpdateCameraController(cameraController);
    }

    return ret;
}

void* PositionHooks::updateCameraControllerHook(void* a1, RopewayPlayerCameraController* cameraController) {
    return g_hook->updateCameraControllerHook_Internal(a1, cameraController);
}

