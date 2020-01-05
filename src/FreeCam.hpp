#pragma once

#include "Mod.hpp"

#include "sdk/ReClass.hpp"

class FreeCam : public Mod {
public:
    FreeCam() = default;

    std::string_view getName() const override { return "FreeCam"; }

    void onConfigLoad(const utility::Config& cfg) override;
    void onConfigSave(utility::Config& cfg) override;

    void onFrame() override;
    void onDrawUI() override;
    void onUpdateTransform(RETransform* transform) override;

private:
    bool updatePointers();

    const ModToggle::Ptr m_enabled{         ModToggle::create(generateName("Enabled"), false) };
    const ModToggle::Ptr m_lockCamera{      ModToggle::create(generateName("LockPosition"), false) };
    const ModToggle::Ptr m_disableMovement{ ModToggle::create(generateName("DisableMovement"), false) };
    const ModKey::Ptr m_toggleKey{          ModKey::create(generateName("ToggleKey")) };
    const ModKey::Ptr m_lockCameraKey{      ModKey::create(generateName("LockKey")) };
    const ModKey::Ptr m_disableMovementKey{ ModKey::create(generateName("DisableMoveKey")) };

    const ModSlider::Ptr m_speed{ ModSlider::create(generateName("Speed"), 0.0f, 1.0f, 0.1f) };

    ValueList m_options{
        *m_enabled,
        *m_lockCamera,
        *m_disableMovement,
        *m_toggleKey,
        *m_lockCameraKey,
        *m_disableMovement,
        *m_speed
    };

    RopewayCameraSystem* m_cameraSystem{ nullptr };
    RopewayInputSystem* m_inputSystem{ nullptr };
    RopewaySurvivorManager* m_survivorManager{ nullptr };

    Matrix4x4f m_lastCameraMatrix{ glm::identity<Matrix4x4f>() };

    bool m_firstTime{ true };
};