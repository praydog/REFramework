#include <codecvt>

#include <imgui/imgui.h>

#include "Address.hpp"
#include "REFramework.hpp"

#include "ComponentHook.hpp"

ComponentHook* g_hook = nullptr;

ComponentHook::ComponentHook() {
    g_hook = this;
}

void ComponentHook::onInitialize() {
    m_getMainCameraHook = std::make_unique<FunctionHook>(g_framework->getModule().get(0xEE2C740), &getMainCameraHook);
}

RECamera* ComponentHook::getMainCameraHook_Internal(void* a1) {
    auto camera = m_getMainCameraHook->getOriginal<decltype(getMainCameraHook)>()(a1);

    for (auto& mod : g_framework->getMods().getMods()) {
        mod->onGetMainCamera(camera);
    }

    return camera;
}

RECamera* ComponentHook::getMainCameraHook(void* a1) {
    return g_hook->getMainCameraHook_Internal(a1);
}

