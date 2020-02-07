#pragma once

#include "Mod.hpp"

#include "sdk/ReClass.hpp"

class FreeCam : public Mod {
public:
    FreeCam() = default;

    std::string_view get_name() const override { return "FreeCam"; }

    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    void on_frame() override;
    void on_draw_ui() override;
    void on_update_transform(RETransform* transform) override;

private:
    bool update_pointers();

    const ModToggle::Ptr m_enabled{         ModToggle::create(generate_name("Enabled"), false) };
    const ModToggle::Ptr m_lock_camera{      ModToggle::create(generate_name("LockPosition"), false) };
    const ModToggle::Ptr m_disable_movement{ ModToggle::create(generate_name("DisableMovement"), false) };
    const ModKey::Ptr m_toggle_key{          ModKey::create(generate_name("ToggleKey")) };
    const ModKey::Ptr m_lock_camera_key{      ModKey::create(generate_name("LockKey")) };
    const ModKey::Ptr m_disable_movement_key{ ModKey::create(generate_name("DisableMoveKey")) };

    const ModSlider::Ptr m_speed{ ModSlider::create(generate_name("Speed"), 0.0f, 1.0f, 0.1f) };

    ValueList m_options{
        *m_enabled,
        *m_lock_camera,
        *m_disable_movement,
        *m_toggle_key,
        *m_lock_camera_key,
        *m_disable_movement_key,
        *m_speed
    };

    RopewayCameraSystem* m_camera_system{ nullptr };
    RopewayInputSystem* m_input_system{ nullptr };
    RopewaySurvivorManager* m_survivor_manager{ nullptr };

    Matrix4x4f m_last_camera_matrix{ glm::identity<Matrix4x4f>() };

    bool m_first_time{ true };
};