#include <codecvt>

#include <imgui/imgui.h>

#include "Address.hpp"
#include "REFramework.hpp"

#include "ComponentHook.hpp"

ComponentHook* g_hook = nullptr;

ComponentHook::ComponentHook() {
    g_hook = this;

    m_componentName.reserve(256);
    m_getMainCameraHook = std::make_unique<FunctionHook>(Address(GetModuleHandle(0)).get(0xEE2C740), &getMainCameraHook);

    m_sliders["x"] = ModSlider::create(-100.0f, 100.0f);
    m_sliders["y"] = ModSlider::create(-100.0f, 100.0f);
    m_sliders["z"] = ModSlider::create(-100.0f, 100.0f);
    m_toggles["world"] = ModToggle::create();

    spdlog::info("ComponentHook");
}

void ComponentHook::onDrawUI() {
    ImGui::Begin("ComponentHook");

    auto& xSlider = m_sliders["x"];
    auto& ySlider = m_sliders["y"];
    auto& zSlider = m_sliders["z"];

    for (auto& i : m_sliders) {
        auto& name = i.first;
        auto& slider = i.second;
        ImGui::SliderFloat(name.c_str(), &slider->value, slider->minValue, slider->maxValue);
    }

    for (auto& i : m_toggles) {
        auto& name = i.first;
        auto& toggle = i.second;
        ImGui::Checkbox(name.c_str(), &toggle->value);
    }

    ImGui::Text("Camera: 0x%p", m_camera);
    ImGui::InputText("Component Name", m_componentName.data(), m_componentName.capacity());

    ImGui::End();
}

void ComponentHook::onUpdateTransform(RETransform* transform) {
    if (utility::REString::getString(transform->ownerGameObject->name) != m_componentName) {
        return;
    }

    auto offset = Vector3{ *m_sliders["x"], *m_sliders["y"], *m_sliders["z"] };

    if (!*m_toggles["world"]) {
        for (int32_t i = 0; i < transform->joints.size; ++i) {
            auto joint = transform->joints.data->joints[i];

            if (joint == nullptr) {
                continue;
            }

            joint->localMatrix[0] += offset;
        }
    }
    else {
        auto& pos = *(Vector3f*)&transform->worldTransform[3];
        pos += offset;
    }
}

RECamera* ComponentHook::getMainCameraHook_Internal(void* a1) {
    m_camera = m_getMainCameraHook->getOriginal<decltype(getMainCameraHook)>()(a1);

    for (auto& mod : g_framework->getMods().getMods()) {
        mod->onGetMainCamera(m_camera);
    }

    return m_camera;
}

RECamera* ComponentHook::getMainCameraHook(void* a1) {
    return g_hook->getMainCameraHook_Internal(a1);
}

