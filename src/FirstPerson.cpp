#include <unordered_set>

#include <spdlog/spdlog.h>
#include <imgui/imgui.h>

#include "REFramework.hpp"

#include "FirstPerson.hpp"

FirstPerson* g_firstPerson = nullptr;

FirstPerson::FirstPerson() {
    // thanks imgui
    g_firstPerson = this;

    m_attachBoneImgui.reserve(256);

    // RopewayPlayerCameraController::worldPosition
    /*
    +B46960F - F3 0F11 86 80000000   - movss [rsi+00000080],xmm0
    +B469617 - F3 0F10 44 24 48      - movss xmm0,[rsp+48]
    +B46961D - F3 0F11 86 88000000   - movss [rsi+00000088],xmm0
    +B469625 - F3 0F11 8E 84000000   - movss [rsi+00000084],xmm1
    */
    // NOP the whole thing away so we can control it
    m_cameraControllerPosPatch = Patch::createNOP((uintptr_t)GetModuleHandle(0) + 0xB46960F, 34);
}

void FirstPerson::onFrame() {
    if (!m_enabled) {
        m_camera = nullptr;
        return;
    }

    if (m_camera == nullptr || m_cameraSystem == nullptr || m_playerCameraController == nullptr || m_playerTransform == nullptr) {
        ComponentTraverser::refreshComponents();
        reset();
        return;
    }

    if (m_camera->ownerGameObject == nullptr || m_cameraSystem->ownerGameObject == nullptr || m_playerTransform->ownerGameObject == nullptr) {
        ComponentTraverser::refreshComponents();
        reset();
        return;
    }

    if (m_attachNames.empty()) {
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
}

void FirstPerson::onDrawUI() {
    ImGui::Begin("FirstPerson");

    if (ImGui::Checkbox("Enabled", &m_enabled)) {
        m_cameraControllerPosPatch->toggle(m_enabled);
    }

    ImGui::SliderFloat3("offset", (float*)&m_attachOffset, -2.0f, 2.0f, "%.3f", 1.0f);
    ImGui::SliderFloat("CameraScale", &m_scale, 0.0f, 250.0f);
    ImGui::SliderFloat("BoneScale", &m_boneScale, 0.0f, 250.0f);

    if (ImGui::InputText("Joint", m_attachBoneImgui.data(), 256)) {
        m_attachBone = std::wstring{ std::begin(m_attachBoneImgui), std::end(m_attachBoneImgui) };
        reset();
    }

    if (ImGui::Button("Refresh Joints")) {
        m_attachNames.clear();
    }

    static auto listBoxHandler = [](void* data, int idx, const char** outText) -> bool {
        return g_firstPerson->listBoxHandlerAttach(data, idx, outText);
    };

    if (ImGui::ListBox("Joints", &m_attachSelected, listBoxHandler, &m_attachNames, (int32_t)m_attachNames.size())) {
        m_attachBoneImgui = m_attachNames[m_attachSelected];
        m_attachBone = std::wstring{ std::begin(m_attachNames[m_attachSelected]), std::end(m_attachNames[m_attachSelected]) };
        reset();
    }

    ImGui::End();
}

void FirstPerson::onComponent(REComponent* component) {
    auto gameObject = component->ownerGameObject;

    if (gameObject == nullptr || component->info == nullptr || component->info->classInfo == nullptr || component->info->classInfo->type == nullptr) {
        return;
    }

    if (component->info->classInfo->type->name == nullptr) {
        return;
    }

    auto typeName = std::string_view{ component->info->classInfo->type->name };

    //spdlog::info("{:p} {} {}", (void*)component, utility::REString::getString(gameObject->name).c_str(), component->info->classInfo->type->name);

    if (typeName == "via.Transform") {
        // pl1000 = claire, pl0000 = leon
        if (utility::REString::equals(gameObject->name, L"pl1000") || utility::REString::equals(gameObject->name, L"pl0000")) {
            if (m_playerTransform != (RETransform*)component) {
                spdlog::info("Found player {:p}", (void*)component);
            }

            m_playerTransform = (RETransform*)component;
        }
    }
    else if (typeName == "app.ropeway.camera.CameraSystem") {
        if (utility::REString::equals(gameObject->name, L"Main Camera")) {

            if (m_cameraSystem != (RopewayCameraSystem*)component) {
                spdlog::info("Found CameraSystem {:p}", (void*)component);
            }

            m_cameraSystem = (RopewayCameraSystem*)component;
        }
    }
    else if (typeName == "via.Camera") {
        if (utility::REString::equals(gameObject->name, L"PlayerCameraController")) {
            if (m_playerCameraController != (RECamera*)component) {
                spdlog::info("Found PlayercameraController {:p}", (void*)component);
            }

            m_playerCameraController = (RECamera*)component;
        }
        else if (utility::REString::equals(gameObject->name, L"Main Camera")) {
            if (m_camera != (RECamera*)component) {
                spdlog::info("Found camera {:p}", (void*)component);
            }

            m_camera = (RECamera*)component;
        }
    }
}

void FirstPerson::onUpdateTransform(RETransform* transform) {
    if (m_camera == nullptr || m_camera->ownerGameObject == nullptr) {
        return;
    }

    if (m_playerCameraController == nullptr || m_playerTransform == nullptr || m_cameraSystem == nullptr || m_cameraSystem->cameraController == nullptr) {
        return;
    }

    if (m_inEventCamera) {
        if (transform != m_cameraSystem->cameraController->activeCamera->ownerGameObject->transform) {
            return;
        }
    }
    else if (transform != m_camera->ownerGameObject->transform) {
        return;
    }

    // action camera
    if (m_cameraSystem->cameraController->activeCamera != m_playerCameraController) {
        return;
    }

    if (!m_enabled) {
        return;
    }

    auto deltaDuration = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - m_lastFrame);
    auto deltaTime = deltaDuration.count();

    m_lastFrame = std::chrono::high_resolution_clock::now();

    auto& mtx = transform->worldTransform;
    auto& cameraPos = mtx[3];

    auto& camPos3 = *(Vector3f*)&mtx[3];
    auto& camForward3 = *(Vector3f*)&mtx[2];

    auto& boneMatrix = utility::RETransform::getJointMatrix(*m_playerTransform, m_attachBone);
    auto& bonePos = boneMatrix[3];
    
    auto camRotMat = glm::extractMatrixRotation(mtx);
    auto headRotMat = glm::extractMatrixRotation(boneMatrix);

    auto offset = headRotMat * (m_attachOffset * Vector4f{ -0.1f, 0.1f, 0.1f, 0.0f });
    auto finalPos = Vector3f{ bonePos + offset };

    if (m_inEventCamera) {

    }
    else {
        auto dist = (glm::distance(m_lastBoneRotation[0], headRotMat[0])
            + glm::distance(m_lastBoneRotation[1], headRotMat[1])
            + glm::distance(m_lastBoneRotation[2], headRotMat[2])) / 3.0f;

        // interpolate the bone rotation (it's snappy otherwise)
        m_lastBoneRotation = glm::interpolate(m_lastBoneRotation, headRotMat, deltaTime * m_boneScale * dist);

        // Look at where the camera is pointing from the head position
        camRotMat = glm::extractMatrixRotation(glm::rowMajor4(glm::lookAtRH(finalPos, camPos3 - (camForward3 * 100.0f), { 0.0f, 1.0f, 0.0f })));
        // Follow the bone rotation, but rotate towards where the camera is looking.
        auto wantedMat = glm::inverse(m_lastBoneRotation) * camRotMat;

        // Average the distance to the wanted rotation
        dist = (glm::distance(m_rotationOffset[0], wantedMat[0]) 
                   + glm::distance(m_rotationOffset[1], wantedMat[1]) 
                   + glm::distance(m_rotationOffset[2], wantedMat[2])) / 3.0f;

        //m_lastDist = glm::lerp(m_lastDist, dist, deltaTime);
        m_rotationOffset = glm::interpolate(m_rotationOffset, wantedMat, m_scale * deltaTime * dist);
        
        // Apply the new matrix
        *(Matrix3x4f*)&mtx = m_lastBoneRotation * m_rotationOffset;
    }

    if (m_resetView) {

    }

    if (!m_inEventCamera) {
        cameraPos = Vector4f{ finalPos, 1.0f };
        transform->position = *(Vector4f*)&cameraPos;
        *(glm::quat*)&transform->angles = glm::quat{ glm::colMajor4(camRotMat) };
        m_cameraSystem->cameraController->worldPosition = cameraPos;
        
        if (m_playerCameraController->ownerGameObject->transform->joints.matrices != nullptr) {
            m_playerCameraController->ownerGameObject->transform->joints.matrices->data[0].worldMatrix = mtx;
        }
    }
}

void FirstPerson::reset() {
    m_rotationOffset = glm::identity<decltype(m_rotationOffset)>();
    m_lastBoneRotation = glm::identity<decltype(m_lastBoneRotation)>();
}

