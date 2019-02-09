#include "REFramework.hpp"

#include "ManualFlashlight.hpp"

ManualFlashlight::ManualFlashlight() {

}

void ManualFlashlight::onFrame() {
    // TODO: Add a config, and controller support.
    auto holdingButton = g_framework->getKeyboardState()[m_key] != 0;

    if (holdingButton && !m_lastButton) {
        m_shouldPullOut = !m_shouldPullOut;
        m_lastButton = true;
    }
    else if (!holdingButton) {
        m_lastButton = false;
    }
}

void ManualFlashlight::onDrawUI() {
    if (!ImGui::CollapsingHeader(getName().data())) {
        return;
    }

    ImGui::Checkbox("Enabled", &m_enabled);
    ImGui::Button(m_keyButtonName.c_str());

    if (ImGui::IsItemHovered()) {
        m_keyButtonName = "Press any key";
        
        auto& keys = g_framework->getKeyboardState();

        for (auto k = 0; k < keys.size(); ++k) {
            if (keys[k]) {
                m_key = k;
                break;
            }
        }
    }
    else {
        m_keyButtonName = "Change Key";
    }

    ImGui::Text("Current Key: 0x%X", m_key);
}

void ManualFlashlight::onUpdateTransform(RETransform* transform) {
    if (!m_enabled) {
        return;
    }

    if (m_illuminationManager == nullptr) {
        m_illuminationManager = g_framework->getGlobals()->get<RopewayIlluminationManager>("app.ropeway.IlluminationManager");
        return;
    }

    // No patch is needed if we are modifying the variables after the transform is updated
    if (transform != m_illuminationManager->ownerGameObject->transform) {
        return;
    }

    m_illuminationManager->shouldUseFlashlight = (int)m_shouldPullOut;
    m_illuminationManager->someCounter = (int)m_shouldPullOut;
    m_illuminationManager->shouldUseFlashlight2 = m_shouldPullOut;
}
