#include "REFramework.hpp"

#include "ManualFlashlight.hpp"

ManualFlashlight::ManualFlashlight() {

}

void ManualFlashlight::onFrame() {
    // TODO: Add a way to change this button, and a config.
    auto holdingButton = g_framework->getKeyboardState()[DIK_F] != 0;

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
}

void ManualFlashlight::onUpdateTransform(RETransform* transform) {
    if (m_illuminationManager == nullptr) {
        m_illuminationManager = g_framework->getGlobals()->get<RopewayIlluminationManager>("app.ropeway.IlluminationManager");
        return;
    }

    if (!m_enabled) {
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
