#pragma once

#include <chrono>
#include <mutex>

#include "Mod.hpp"
#include "Patch.hpp"

class FirstPerson : public Mod {
public:
    FirstPerson();

    std::string_view getName() const override { return "FirstPerson"; };

    void onInitialize() override;
    void onFrame() override;
    void onDrawUI() override;

    void onComponent(REComponent* component) override;
    void onPreUpdateTransform(RETransform* transform) override;
    void onUpdateTransform(RETransform* transform) override;
    void onUpdateCameraController(RopewayPlayerCameraController* controller) override;
    void onUpdateCameraController2(RopewayPlayerCameraController* controller) override;

protected:
    // gross
    bool listBoxHandlerAttach(void* data, int idx, const char** outText) {
        *outText = ((decltype(m_attachNames)*)data)->at(idx).data();
        return true;
    }

private:
    void reset();
    bool updatePointersFromCameraSystem(RopewayCameraSystem* cameraSystem);
    void updateCameraTransform(RETransform* transform);
    void updatePlayerBones(RETransform* transform);
    void updateFOV(RopewayPlayerCameraController* controller);
    void updateJointNames();
    float updateDeltaTime(REComponent* component);

    std::mutex m_matrixMutex{};
    std::mutex m_frameMutex{};
    std::mutex m_deltaMutex{};

    std::string m_attachBoneImgui{ "head" };
    std::wstring m_attachBone{ L"head" };
    std::string m_playerName{ "pl1000" };

    // Different "configs" for each model
    std::unordered_map<std::string, Vector4f> m_attachOffsets;
    Matrix4x4f m_rotationOffset{ glm::identity<Matrix4x4f>() };
    Matrix4x4f m_interpolatedBone{ glm::identity<Matrix4x4f>() };
    Matrix4x4f m_lastBoneMatrix{ glm::identity<Matrix4x4f>() };
    Matrix4x4f m_lastCameraMatrix{ glm::identity<Matrix4x4f>() };
    Vector4f m_lastControllerPos{};
    glm::quat m_lastControllerRotation{};

    float m_cameraScale{ 40.0f };
    float m_boneScale{ 15.0f };
    float m_lastFovMult{ 0.0f };

    RETransform* m_playerTransform{ nullptr };
    RECamera* m_camera{ nullptr };
    RECamera* m_playerCameraController{ nullptr };
    RopewayCameraSystem* m_cameraSystem{ nullptr };

    bool m_enabled{ true };
    bool m_hideMesh{ true };
    bool m_disableVignette{ false };
    bool m_wasDifferentCamera{ false };

    std::unordered_map<REComponent*, std::chrono::high_resolution_clock::time_point> m_updateTimes;
    std::unordered_map<REComponent*, float> m_deltaTimes;

    std::vector<std::string> m_attachNames;
    int32_t m_attachSelected{ 0 };
    
    std::unique_ptr<Patch> m_disableVignettePatch{};

    ModFloat::Ptr m_currentFov{};
};