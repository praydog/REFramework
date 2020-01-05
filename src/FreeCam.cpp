#include "FreeCam.hpp"

void FreeCam::onConfigLoad(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.configLoad(cfg);
    }
}

void FreeCam::onConfigSave(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.configSave(cfg);
    }
}

void FreeCam::onFrame() {
    if (m_toggleKey->isKeyDownOnce()) {
        m_enabled->toggle();
        m_firstTime = true;
    }

    if (m_lockCameraKey->isKeyDownOnce()) {
        m_lockCamera->toggle();
    }

    if (m_disableMovementKey->isKeyDownOnce()) {
        m_disableMovement->toggle();
    }
}

void FreeCam::onDrawUI() {
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);

    if (!ImGui::CollapsingHeader(getName().data())) {
        return;
    }

    if (m_enabled->draw("Enabled")) {
        m_firstTime = true;
    }

    ImGui::SameLine();
    m_lockCamera->draw("Lock Position");
    m_disableMovement->draw("Disable Character Movement");

    m_toggleKey->draw("Toggle Key");
    m_lockCameraKey->draw("Lock Position Toggle Key");
    m_disableMovementKey->draw("Disable Movement Toggle Key");

    m_speed->draw("Speed");
}

void FreeCam::onUpdateTransform(RETransform* transform) {
    if (!m_enabled->value() && !m_firstTime) {
        return;
    }

    if (!updatePointers()) {
        return;
    }

    if (m_cameraSystem->mainCamera == nullptr || transform != m_cameraSystem->mainCamera->ownerGameObject->transform) {
        return;
    }

    auto condition = m_survivorManager->playerCondition;

    if (m_firstTime || condition == nullptr) {
        m_lastCameraMatrix = transform->worldTransform;

        if (condition != nullptr && condition->actionOrderer != nullptr) {
            condition->actionOrderer->enabled = true;
            m_firstTime = false;
        }
        
        return;
    }

    auto orderer = condition->actionOrderer;

    if (orderer != nullptr) {
        orderer->enabled = !m_disableMovement->value();
    }

    auto controller = m_cameraSystem->cameraController;

    if (controller == nullptr) {
        return;
    }

    // despite the name, it works with the keyboard
    auto leftAnalog = utility::REManagedObject::getField<RopewayInputSystemAnalogStick*>(m_inputSystem, "_LStick");

    if (leftAnalog == nullptr) {
        return;
    }

    // Update wanted camera position
    if (!m_lockCamera->value()) {
        // Move direction
        // It's not a Vector2f because via.vec2 is not actually 8 bytes, we don't want stack corruption to occur.
        auto axis = utility::REManagedObject::getField<Vector3f>(leftAnalog, "Axis");
        auto dir = Vector4f{ axis.x, 0.0f, axis.y * -1.0f, 0.0f };

        auto delta = utility::REComponent::getDeltaTime(transform);

        // Use controller rotation instead of camera rotation as it's accurate, will work in cutscenes.
        auto newPos = m_lastCameraMatrix[3] + Matrix4x4f{ *(glm::quat*)&controller->worldRotation } * dir * m_speed->value() * delta;

        // Keep track of the rotation if we want to lock the camera
        m_lastCameraMatrix = transform->worldTransform;
        m_lastCameraMatrix[3] = newPos;
    }

    transform->worldTransform = m_lastCameraMatrix;
    transform->position = m_lastCameraMatrix[3];
    controller->worldPosition = m_lastCameraMatrix[3];
}

bool FreeCam::updatePointers() {
    if (m_cameraSystem == nullptr || m_inputSystem == nullptr || m_survivorManager == nullptr) {
        auto& globals = *g_framework->getGlobals();
        m_cameraSystem = globals.get<RopewayCameraSystem>("app.ropeway.camera.CameraSystem");
        m_inputSystem = globals.get<RopewayInputSystem>("app.ropeway.InputSystem");
        m_survivorManager = globals.get<RopewaySurvivorManager>("app.ropeway.SurvivorManager");
        return false;
    }

    return true;
}
