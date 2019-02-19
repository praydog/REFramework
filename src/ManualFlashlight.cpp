#include "REFramework.hpp"

#include "ManualFlashlight.hpp"

void ManualFlashlight::onFrame() {
    // TODO: Add controller support.
    auto holdingButton = m_key->isKeyDown();

    if (holdingButton && !m_lastButton) {
        m_shouldPullOut = !m_shouldPullOut;
        m_lastButton = true;
    }
    else if (!holdingButton) {
        m_lastButton = false;
    }
}

void ManualFlashlight::onDrawUI() {
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);
    if (!ImGui::CollapsingHeader(getName().data())) {
        return;
    }

    m_enabled->draw("Enabled");
    m_key->draw("Change Key");
}

void ManualFlashlight::onConfigLoad(const utility::Config& cfg) {
    m_enabled->configLoad(cfg);
    m_key->configLoad(cfg);
}

void ManualFlashlight::onConfigSave(utility::Config& cfg) {
    m_enabled->configSave(cfg);
    m_key->configSave(cfg);
}

void ManualFlashlight::onUpdateTransform(RETransform* transform) {
    if (!m_enabled->value()) {
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
