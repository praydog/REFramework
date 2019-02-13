#pragma once

#include <chrono>
#include <mutex>

#include "Mod.hpp"
#include "utility/Patch.hpp"

class FirstPerson : public Mod {
public:
    FirstPerson();

    std::string_view getName() const override { return "FirstPerson"; };

    bool onInitialize() override;
    void onFrame() override;
    void onDrawUI() override;

    void onConfigLoad(const utility::Config& cfg) override;
    void onConfigSave(utility::Config& cfg) override;

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
    void updateSweetLightContext(RopewaySweetLightManagerContext* ctx);
    void updateSweetLightTransform(RETransform* transform);
    void updatePlayerBones(RETransform* transform);
    void updateFOV(RopewayPlayerCameraController* controller);
    void updateJointNames();
    float updateDeltaTime(REComponent* component);

    // Needs to be recursive for some reason. Otherwise freeze.
    std::recursive_mutex m_matrixMutex{};
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

    float m_lastFovMult{ 0.0f };

    RETransform* m_playerTransform{ nullptr };
    RECamera* m_camera{ nullptr };
    RECamera* m_playerCameraController{ nullptr };
    RopewayCameraSystem* m_cameraSystem{ nullptr };
    RopewaySweetLightManager* m_sweetLightManager{ nullptr };

    std::unordered_map<REComponent*, std::chrono::high_resolution_clock::time_point> m_updateTimes;
    std::unordered_map<REComponent*, float> m_deltaTimes;

    std::vector<std::string> m_attachNames;
    int32_t m_attachSelected{ 0 };
    
    std::unique_ptr<Patch> m_disableVignettePatch{};

    ModToggle::Ptr m_enabled{ ModToggle::create() };
    ModToggle::Ptr m_disableVignette{ ModToggle::create() };
    ModToggle::Ptr m_hideMesh{ ModToggle::create(true) };
    ModToggle::Ptr m_disableLightSource{ ModToggle::create(true) };

    ModSlider::Ptr m_fovOffset{ ModSlider::create(-100.0f, 100.0f, 10.0f) };
    ModSlider::Ptr m_fovMult{ ModSlider::create(0.0f, 2.0f, 1.0f) };

    ModSlider::Ptr m_cameraScale{ ModSlider::create(0.0f, 100.0f, 40.0f) };
    ModSlider::Ptr m_boneScale{ ModSlider::create(0.0f, 100.0f, 15.0f) };

    // just used to draw. not actually stored in config
    ModFloat::Ptr m_currentFov{ ModFloat::create() };
};