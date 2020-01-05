#include <unordered_set>

#include <spdlog/spdlog.h>
#include <imgui/imgui.h>

#include "utility/Scan.hpp"
#include "REFramework.hpp"

#include "FirstPerson.hpp"

FirstPerson* g_firstPerson = nullptr;

FirstPerson::FirstPerson() {
    // thanks imgui
    g_firstPerson = this;
    m_attachBoneImgui.reserve(256);

    // Specific player model configs
    // Leon
    m_attachOffsets["pl0000"] = Vector4f{ -0.26f, 0.435f, 1.0f, 0.0f };
    // Claire
    m_attachOffsets["pl1000"] = Vector4f{ -0.23f, 0.4f, 1.0f, 0.0f };
    // Sherry
    m_attachOffsets["pl3000"] = Vector4f{ -0.278f, 0.435f, 0.945f, 0.0f };
    // Hunk
    m_attachOffsets["pl4000"] = Vector4f{ -0.26f, 0.435f, 1.0f, 0.0f };
    // Kendo
    m_attachOffsets["pl5000"] = Vector4f{ -0.24f, 0.4f, 1.0f, 0.0f };
    // Forgotten Soldier
    m_attachOffsets["pl5600"] = Vector4f{ -0.316, 0.556f, 1.02f, 0.0f };
    // Elizabeth
    m_attachOffsets["pl6400"] = Vector4f{ -0.316, 0.466f, 0.79f, 0.0f };
}

std::optional<std::string> FirstPerson::onInitialize() {
    auto vignetteCode = utility::scan(g_framework->getModule().as<HMODULE>(), "8B 87 3C 01 00 00 89 83 DC 00 00 00");

    if (!vignetteCode) {
        return "Failed to find Disable Vignette pattern";
    }

    // xor eax, eax
    m_disableVignettePatch = Patch::create(*vignetteCode, { 0x31, 0xC0, 0x90, 0x90, 0x90, 0x90 }, false);

    return Mod::onInitialize();
}

void FirstPerson::onFrame() {
    if (m_toggleKey->isKeyDownOnce() && !m_enabled->toggle()) {
        onDisabled();
    }

    if (!m_enabled->value()) {
        return;
    }

    // Update our global pointers
    if (m_cameraSystem == nullptr || m_cameraSystem->ownerGameObject == nullptr || m_sweetLightManager == nullptr || m_sweetLightManager->ownerGameObject == nullptr) {
        auto& globals = *g_framework->getGlobals();
        m_sweetLightManager = globals.get<RopewaySweetLightManager>("app.ropeway.SweetLightManager");
        m_cameraSystem = globals.get<RopewayCameraSystem>("app.ropeway.camera.CameraSystem");

        reset();
        return;
    }
}

void FirstPerson::onDrawUI() {
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);

    if (!ImGui::CollapsingHeader(getName().data())) {
        return;
    }

    std::lock_guard _{ m_frameMutex };

    if (m_enabled->draw("Enabled") && !m_enabled->value()) {
        // Disable fov and camera light changes
        onDisabled();
    }

    ImGui::SameLine();
    m_disableLightSource->draw("Disable Camera Light");

    m_hideMesh->draw("Hide Joint Mesh");

    ImGui::SameLine();
    m_rotateMesh->draw("Force Rotate Joint");

    if (m_disableVignette->draw("Disable Vignette")) {
        m_disableVignettePatch->toggle(m_disableVignette->value());
    }

    m_toggleKey->draw("Change Toggle Key");

    ImGui::SliderFloat3("CameraOffset", (float*)&m_attachOffsets[m_playerName], -2.0f, 2.0f, "%.3f", 1.0f);

    m_cameraScale->draw("CameraSpeed");
    m_boneScale->draw("CameraShake");

    if (m_playerCameraController != nullptr) {
        if (m_fovOffset->draw("FOVOffset")) {
            updateFOV(m_cameraSystem->cameraController);
        }

        if (m_fovMult->draw("FOVMultiplier")) {
            updateFOV(m_cameraSystem->cameraController);
            m_lastFovMult = m_fovMult->value();
        }

        m_currentFov->value() = m_playerCameraController->activeCamera->fov;
        m_currentFov->draw("CurrentFOV");
    }

    if (ImGui::InputText("Joint", m_attachBoneImgui.data(), 256)) {
        m_attachBone = std::wstring{ std::begin(m_attachBoneImgui), std::end(m_attachBoneImgui) };
    }

    static auto listBoxHandler = [](void* data, int idx, const char** outText) -> bool {
        return g_firstPerson->listBoxHandlerAttach(data, idx, outText);
    };

    if (ImGui::ListBox("Joints", &m_attachSelected, listBoxHandler, &m_attachNames, (int32_t)m_attachNames.size())) {
        m_attachBoneImgui = m_attachNames[m_attachSelected];
        m_attachBone = std::wstring{ std::begin(m_attachNames[m_attachSelected]), std::end(m_attachNames[m_attachSelected]) };
    }
}

void FirstPerson::onConfigLoad(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.configLoad(cfg);
    }

    m_lastFovMult = m_fovMult->value();

    // turn the patch on
    if (m_disableVignette->value()) {
        m_disableVignettePatch->toggle(m_disableVignette->value());
    }
}

void FirstPerson::onConfigSave(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.configSave(cfg);
    }
}

thread_local bool g_inPlayerTransform = false;
thread_local bool g_firstTime = true;

void FirstPerson::onPreUpdateTransform(RETransform* transform) {
    if (!m_enabled->value() || m_camera == nullptr || m_camera->ownerGameObject == nullptr) {
        return;
    }

    if (m_playerCameraController == nullptr || m_playerTransform == nullptr || m_cameraSystem == nullptr || m_cameraSystem->cameraController == nullptr) {
        return;
    }

    // can change to action camera
    if (m_cameraSystem->cameraController != m_playerCameraController) {
        return;
    }

    // We need to lock a mutex because these UpdateTransform functions
    // are called from multiple threads
    if (transform == m_playerTransform) {
        g_inPlayerTransform = true;
        g_firstTime = true;
        m_matrixMutex.lock();
    }
    // This is because UpdateTransform recursively calls UpdateTransform on its children,
    // and the player transform (topmost) is the one that actually updates the bone matrix,
    // and all the child transforms operate on the bones that it updated
    else if (g_inPlayerTransform) {
        updatePlayerBones(m_playerTransform);
    }
}

void FirstPerson::onUpdateTransform(RETransform* transform) {
    // Do this first before anything else.
    if (g_inPlayerTransform && transform == m_playerTransform) {
        updateJointNames();

        g_inPlayerTransform = false;
        m_matrixMutex.unlock();
    }

    if (!m_enabled->value()) {
        return;
    }

    if (m_cameraSystem != nullptr && m_cameraSystem->ownerGameObject != nullptr && transform == m_cameraSystem->ownerGameObject->transform) {
        if (!updatePointersFromCameraSystem(m_cameraSystem)) {
            reset();
            return;
        }
    }

    if (m_sweetLightManager == nullptr) {
        return;
    }

    if (m_camera == nullptr || m_camera->ownerGameObject == nullptr) {
        return;
    }

    if (m_playerCameraController == nullptr || m_playerTransform == nullptr || m_cameraSystem == nullptr || m_cameraSystem->cameraController == nullptr) {
        return;
    }

    // Remove the camera light if specified
    if (transform == m_sweetLightManager->ownerGameObject->transform) {
        // SweetLight
        updateSweetLightContext(utility::RopewaySweetLightManager::getContext(m_sweetLightManager, 0));
        // EnvironmentSweetLight
        updateSweetLightContext(utility::RopewaySweetLightManager::getContext(m_sweetLightManager, 1));
    }

    // can change to action camera
    if (m_cameraSystem->cameraController != m_playerCameraController) {
        return;
    }

    if (transform == m_camera->ownerGameObject->transform) {
        updateCameraTransform(transform);
    }
}

void FirstPerson::onUpdateCameraController(RopewayPlayerCameraController* controller) {
    if (!m_enabled->value() || controller != m_playerCameraController || m_playerTransform == nullptr) {
        return;
    }

    // The following code fixes inaccuracies between the rotation set by the game and what's set in updateCameraTransform
    controller->worldPosition = m_lastCameraMatrix[3];
    *(glm::quat*)&controller->worldRotation = glm::quat{ m_lastCameraMatrix };

    m_camera->ownerGameObject->transform->worldTransform = m_lastCameraMatrix;
    m_camera->ownerGameObject->transform->angles = *(Vector4f*)&controller->worldRotation;
}

void FirstPerson::onUpdateCameraController2(RopewayPlayerCameraController* controller) {
    if (!m_enabled->value() || controller != m_playerCameraController || m_playerTransform == nullptr) {
        return;
    }

    // Just update the FOV in here. Whatever.
    updateFOV(controller);

    // Save the original position and rotation before our modifications.
    // If we don't, the camera rotation will freeze up, because it keeps getting overwritten.
    m_lastControllerPos = controller->worldPosition;
    m_lastControllerRotation = *(glm::quat*)&controller->worldRotation;
}

void FirstPerson::reset() {
    m_rotationOffset = glm::identity<decltype(m_rotationOffset)>();
    m_interpolatedBone = glm::identity<decltype(m_interpolatedBone)>();
    m_lastCameraMatrix = glm::identity<decltype(m_lastCameraMatrix)>();
    m_lastBoneMatrix = glm::identity<decltype(m_lastBoneMatrix)>();
    m_lastControllerPos = Vector4f{};
    m_lastControllerRotation = glm::quat{};

    std::lock_guard _{ m_frameMutex };
    m_attachNames.clear();
}

bool FirstPerson::updatePointersFromCameraSystem(RopewayCameraSystem* cameraSystem) {
    if (cameraSystem == nullptr) {
        return false;
    }

    if (m_camera = cameraSystem->mainCamera; m_camera == nullptr) {
        m_playerTransform = nullptr;
        return false;
    }

    auto joint = cameraSystem->playerJoint;
    
    if (joint == nullptr) {
        m_playerTransform = nullptr;
        return false;
    }

    // Update player name and log it
    if (m_playerTransform != joint->parentTransform && joint->parentTransform != nullptr) {
        if (joint->parentTransform->ownerGameObject == nullptr) {
            return false;
        }

        m_playerName = utility::REString::getString(joint->parentTransform->ownerGameObject->name);

        if (m_playerName.empty()) {
            return false;
        }

        spdlog::info("Found Player {:s} {:p}", m_playerName.data(), (void*)joint->parentTransform);
    }

    // Update player transform pointer
    if (m_playerTransform = joint->parentTransform; m_playerTransform == nullptr) {
        return false;
    }

    // Update PlayerCameraController camera pointer
    if (m_playerCameraController == nullptr) {
        auto controller = cameraSystem->cameraController;

        if (controller == nullptr || controller->ownerGameObject == nullptr || controller->activeCamera == nullptr || controller->activeCamera->ownerGameObject == nullptr) {
            return false;
        }

        if (utility::REString::equals(controller->activeCamera->ownerGameObject->name, L"PlayerCameraController")) {
            m_playerCameraController = controller;
            spdlog::info("Found PlayerCameraController {:p}", (void*)m_playerCameraController);
        }

        return m_playerCameraController != nullptr;
    }

    return true;
}

void FirstPerson::updateCameraTransform(RETransform* transform) {
    std::lock_guard _{ m_matrixMutex };

    auto deltaTime = updateDeltaTime(transform);

    auto& mtx = transform->worldTransform;
    auto& cameraPos = mtx[3];

    auto camPos3 = Vector3f{ m_lastControllerPos };

    auto boneMatrix = m_lastCameraMatrix * Matrix4x4f{
        -1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, -1, 0,
        0, 0, 0, 1
    };

    boneMatrix[3] = m_lastBoneMatrix[3];
    auto& bonePos = boneMatrix[3];

    auto camRotMat = glm::extractMatrixRotation(Matrix4x4f{ m_lastControllerRotation });
    auto headRotMat = glm::extractMatrixRotation(m_lastBoneMatrix);

    auto& camForward3 = *(Vector3f*)&camRotMat[2];

    auto offset = glm::extractMatrixRotation(boneMatrix) * (m_attachOffsets[m_playerName] * Vector4f{ -0.1f, 0.1f, 0.1f, 0.0f });
    auto finalPos = Vector3f{ bonePos + offset };

    // Average the distance to the wanted rotation
    auto dist = (glm::distance(m_interpolatedBone[0], headRotMat[0])
               + glm::distance(m_interpolatedBone[1], headRotMat[1])
               + glm::distance(m_interpolatedBone[2], headRotMat[2])) / 3.0f;

    // interpolate the bone rotation (it's snappy otherwise)
    m_interpolatedBone = glm::interpolate(m_interpolatedBone, headRotMat, deltaTime * (m_boneScale->value() * 0.01f) * dist);

    // Look at where the camera is pointing from the head position
    camRotMat = glm::extractMatrixRotation(glm::rowMajor4(glm::lookAtLH(finalPos, camPos3 + (camForward3 * 8192.0f), { 0.0f, 1.0f, 0.0f })));
    // Follow the bone rotation, but rotate towards where the camera is looking.
    auto wantedMat = glm::inverse(m_interpolatedBone) * camRotMat;

    // Average the distance to the wanted rotation
    dist = (glm::distance(m_rotationOffset[0], wantedMat[0])
          + glm::distance(m_rotationOffset[1], wantedMat[1])
          + glm::distance(m_rotationOffset[2], wantedMat[2])) / 3.0f;

    m_rotationOffset = glm::interpolate(m_rotationOffset, wantedMat, deltaTime * (m_cameraScale->value() * 0.01f) * dist);
    auto finalMat = m_interpolatedBone * m_rotationOffset;
    auto finalQuat = glm::quat{ finalMat };

    // Apply the same matrix data to other things stored in-game (positions/quaternions)
    cameraPos = Vector4f{ finalPos, 1.0f };
    m_cameraSystem->cameraController->worldPosition = *(Vector4f*)&cameraPos;
    m_cameraSystem->cameraController->worldRotation = *(Vector4f*)&finalQuat;
    transform->position = *(Vector4f*)&cameraPos;
    transform->angles = *(Vector4f*)&finalQuat;

    // Apply the new matrix
    *(Matrix3x4f*)&mtx = finalMat;
    m_lastCameraMatrix = mtx;

    if (transform->joints.size >= 1 && transform->joints.matrices != nullptr) {
        transform->joints.matrices->data[0].worldMatrix = m_lastCameraMatrix;
    }
}

void FirstPerson::updateSweetLightContext(RopewaySweetLightManagerContext* ctx) {
    if (ctx->controller == nullptr || ctx->controller->ownerGameObject == nullptr) {
        return;
    }


    // Disable the camera light source.
    ctx->controller->ownerGameObject->shouldDraw = !(m_enabled->value() &&
                                                     m_disableLightSource->value() &&
                                                     m_cameraSystem->cameraController == m_playerCameraController);
}

void FirstPerson::updatePlayerBones(RETransform* transform) {
    auto& boneMatrix = utility::RETransform::getJointMatrix(*m_playerTransform, m_attachBone);
    
    if (g_firstTime) {
        m_lastBoneMatrix = boneMatrix;
        g_firstTime = false;
    }

    // Forcefully rotate the bone to match the camera direction
    if (m_cameraSystem->cameraController == m_playerCameraController && m_rotateMesh->value()) {
        auto wantedMat = m_lastCameraMatrix * Matrix4x4f{
            -1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, -1, 0,
            0, 0, 0, 1
        };

        *(Matrix3x4f*)&boneMatrix = wantedMat;
    }

    // Hide the head model by moving it out of view of the camera (and hopefully shadows...)
    if (m_hideMesh->value()) {
        boneMatrix[0] = Vector4f{ 0.0f, 0.0f, 0.0f, 0.0f };
        boneMatrix[1] = Vector4f{ 0.0f, 0.0f, 0.0f, 0.0f };
        boneMatrix[2] = Vector4f{ 0.0f, 0.0f, 0.0f, 0.0f };
    }
}

void FirstPerson::updateFOV(RopewayPlayerCameraController* controller) {
    if (controller == nullptr) {
        return;
    }

    auto isActiveCamera = m_cameraSystem != nullptr
        && m_cameraSystem->cameraController != nullptr
        && m_cameraSystem->cameraController->cameraParam != nullptr
        && m_cameraSystem->cameraController == m_playerCameraController;

    if (!isActiveCamera) { 
        return; 
    }

    if (auto param = controller->cameraParam; param != nullptr) {
        auto newValue = (param->fov * m_fovMult->value()) + m_fovOffset->value();

        if (m_fovMult->value() != m_lastFovMult) {
            auto prevValue = (param->fov * m_lastFovMult) + m_fovOffset->value();
            auto delta = prevValue - newValue;

            m_fovOffset->value() += delta;
            m_playerCameraController->activeCamera->fov = (param->fov * m_fovMult->value()) + m_fovOffset->value();
        }
        else {
            m_playerCameraController->activeCamera->fov = newValue;
        }
        
        // Causes the camera to ignore the FOV inside the param
        param->useParam = !m_enabled->value();
    }
}

void FirstPerson::updateJointNames() {
    if (m_playerTransform == nullptr || !m_attachNames.empty()) {
        return;
    }

    auto& joints = m_playerTransform->joints;

    for (int32_t i = 0; joints.data != nullptr && i < joints.size; ++i) {
        auto joint = joints.data->joints[i];

        if (joint == nullptr || joint->info == nullptr || joint->info->name == nullptr) {
            continue;
        }

        auto name = std::wstring{ joint->info->name };
        m_attachNames.push_back(std::string{ std::begin(name), std::end(name) }.c_str());
    }
}

float FirstPerson::updateDeltaTime(REComponent* component) {
    return utility::REComponent::getDeltaTime(component);
}

void FirstPerson::onDisabled() {
    // Disable fov and camera light changes
    if (m_cameraSystem != nullptr && m_sweetLightManager != nullptr) {
        updateFOV(m_cameraSystem->cameraController);
        updateSweetLightContext(utility::RopewaySweetLightManager::getContext(m_sweetLightManager, 0));
        updateSweetLightContext(utility::RopewaySweetLightManager::getContext(m_sweetLightManager, 1));
    }
}

