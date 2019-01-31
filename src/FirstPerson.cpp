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

    m_toggles["enabled"] = ModToggle::create();
    m_toggles["enabled"]->value = true;
}

void FirstPerson::onFrame() {
    auto enabled = m_toggles["enabled"]->value;

    if (!enabled) {
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

    ImGui::Checkbox("Enabled", &m_toggles["enabled"]->value);
    ImGui::SliderFloat3("offset", (float*)&m_attachOffset, -2.0f, 2.0f, "%.3f", 1.0f);
    ImGui::SliderFloat("scale", &m_scale, 0.0f, 250.0f);

    if (ImGui::InputText("joint", m_attachBoneImgui.data(), 256)) {
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

    if (!m_toggles["enabled"]->value) {
        return;
    }

    auto deltaDuration = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - m_lastFrame);
    auto deltaTime = deltaDuration.count();

    m_lastFrame = std::chrono::high_resolution_clock::now();

    constexpr auto a = offsetof(RETransform, worldTransform);

    auto& mtx = transform->worldTransform;
    auto& cameraPos = mtx[3];

    auto& boneMatrix = utility::RETransform::getJointMatrix(*m_playerTransform, m_attachBone);
    auto& bonePos = boneMatrix[3];

    constexpr float speed = 0.01f;
    
    auto camRotMat = glm::extractMatrixRotation(mtx);
    auto headRotMat = glm::extractMatrixRotation(boneMatrix);

    auto offset = headRotMat * (m_attachOffset * Vector4f{ -0.1f, 0.1f, 0.1f, 0.0f });

    if (m_inEventCamera) {

    }
    else {
        /*auto camDir = camForward;
        auto camLookPosition = pos + (camForward * 8192.0f);
        auto newCamDir = normalize(camLookPosition - (bonePos + offset));
        auto finalDir = normalize(newCamDir);

        camRotMat = Matrix3x3f{ Quat::rotation(finalDir, Vector3f::yAxis()) };*/

        // Follow the bone rotation, but rotate towards where the camera is looking.
        auto wantedMat = glm::inverse(headRotMat) * camRotMat;

        // Average the distance to the wanted rotation
        auto dist = glm::distance(m_rotationOffset[0], wantedMat[0]) + glm::distance(m_rotationOffset[1], wantedMat[1]) + glm::distance(wantedMat[2], m_rotationOffset[2]);
        dist /= 3.0f;
        m_rotationOffset = glm::interpolate(m_rotationOffset, wantedMat, m_scale * deltaTime * dist);
        
        camRotMat = headRotMat * m_rotationOffset;

        *(Matrix3x4f*)&mtx = camRotMat;
    }

    if (m_resetView) {

    }

    if (!m_inEventCamera) {
        cameraPos = bonePos + offset;
        transform->position = *(Vector4f*)&cameraPos;
        *(glm::quat*)&transform->angles = glm::quat{ camRotMat };
        
        if (m_playerCameraController->ownerGameObject->transform->joints.matrices != nullptr) {
            m_playerCameraController->ownerGameObject->transform->joints.matrices->data[0].worldMatrix = mtx;
        }
    }
}

void FirstPerson::reset() {
    m_rotationOffset = glm::identity<decltype(m_rotationOffset)>();
}

