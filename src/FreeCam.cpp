#include "FreeCam.hpp"

void FreeCam::onConfigLoad(const utility::Config& cfg) {
    m_enabled->configLoad(cfg, generateName("Enabled"));
    m_speed->configLoad(cfg, generateName("Speed"));
}

void FreeCam::onConfigSave(utility::Config& cfg) {
    m_enabled->configSave(cfg, generateName("Enabled"));
    m_speed->configSave(cfg, generateName("Speed"));
}

void FreeCam::onFrame() {
    
}

void FreeCam::onDrawUI() {
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);

    if (!ImGui::CollapsingHeader(getName().data())) {
        return;
    }

    m_enabled->draw("Enabled");
    m_speed->draw("Speed");
}

void FreeCam::onUpdateTransform(RETransform* transform) {
    if (!updatePointers()) {
        return;
    }

    if (m_cameraSystem->mainCamera == nullptr || transform != m_cameraSystem->mainCamera->ownerGameObject->transform) {
        return;
    }

    if (!m_enabled->value || m_firstTime) {
        m_lastCameraMatrix = transform->worldTransform;
        m_firstTime = false;
        return;
    }

    auto& kb = g_framework->getKeyboardState();
    auto delta = utility::REManagedObject::getField<float>(transform, "DeltaTime");
    auto leftAnalog = utility::REManagedObject::getField<RopewayInputSystemAnalogStick*>(m_inputSystem, "_LStick");

    if (leftAnalog == nullptr) {
        return;
    }

    // Move direction
    auto dir = utility::REManagedObject::getField<Vector4f>(leftAnalog, "Direction") * -1.0f;

    // Update wanted camera position
    m_lastCameraMatrix[3] += glm::extractMatrixRotation(transform->worldTransform) * dir * m_speed->value * delta;

    transform->worldTransform[3] = m_lastCameraMatrix[3];
    transform->position = m_lastCameraMatrix[3];

    auto controller = m_cameraSystem->cameraController;

    if (controller != nullptr) {
        controller->worldPosition = m_lastCameraMatrix[3];
    }
}

bool FreeCam::updatePointers() {
    if (m_cameraSystem == nullptr || m_inputSystem == nullptr) {
        auto& globals = *g_framework->getGlobals();
        m_cameraSystem = globals.get<RopewayCameraSystem>("app.ropeway.camera.CameraSystem");
        m_inputSystem = globals.get<RopewayInputSystem>("app.ropeway.InputSystem");
        return false;
    }

    return true;
}
