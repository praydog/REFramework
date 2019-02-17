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

    ModToggle::Ptr m_enabled{ ModToggle::create(false) };
    ModSlider::Ptr m_speed{ ModSlider::create(0.0f, 5.0f, 0.2f) };

    RopewayCameraSystem* m_cameraSystem{ nullptr };
    RopewayInputSystem* m_inputSystem{ nullptr };
    Matrix4x4f m_lastCameraMatrix{ glm::identity<Matrix4x4f>() };

    bool m_firstTime{ true };
};