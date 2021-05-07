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

    const ModToggle::Ptr m_enabled{              ModToggle::create(generate_name("Enabled"), false) };
    const ModToggle::Ptr m_lock_camera{          ModToggle::create(generate_name("LockPosition"), false) };
    const ModToggle::Ptr m_disable_movement{     ModToggle::create(generate_name("DisableMovement"), false) };
    const ModKey::Ptr m_toggle_key{              ModKey::create(generate_name("ToggleKey")) };
    const ModKey::Ptr m_lock_camera_key{         ModKey::create(generate_name("LockKey")) };
    const ModKey::Ptr m_disable_movement_key{    ModKey::create(generate_name("DisableMoveKey")) };
    const ModKey::Ptr m_speed_modifier_fast_key{ ModKey::create(generate_name("SpeedModifierFast"), DIK_LSHIFT)};
    const ModKey::Ptr m_speed_modifier_slow_key{ ModKey::create(generate_name("SpeedModifierSlow"), DIK_LMENU)};

    const ModSlider::Ptr m_speed{ ModSlider::create(generate_name("Speed"), 0.0f, 1.0f, 0.1f) };
    const ModSlider::Ptr m_speed_modifier{ ModSlider::create(generate_name("SpeedModifier"), 1.f, 50.f, 4.f) };

#ifdef RE8
    const ModSlider::Ptr m_rotation_speed{ ModSlider::create(generate_name("RotationSpeed"), 0.0f, 1.0f, 0.1f) };
#endif

    ValueList m_options{
        *m_enabled,
        *m_lock_camera,
        *m_disable_movement,
        *m_toggle_key,
        *m_lock_camera_key,
        *m_disable_movement_key,
        *m_speed,
        *m_speed_modifier,
        *m_speed_modifier_fast_key,
        *m_speed_modifier_slow_key,
#ifdef RE8
        *m_rotation_speed,
#endif
    };

#ifndef RE8
    RopewayCameraSystem* m_camera_system{ nullptr };
    RopewayInputSystem* m_input_system{ nullptr };
    RopewaySurvivorManager* m_survivor_manager{ nullptr };
#else
    AppHIDPadManager* m_pad_manager{ nullptr };
    AppPropsManager* m_props_manager{ nullptr };
#endif

    Matrix4x4f m_last_camera_matrix{ glm::identity<Matrix4x4f>() };

    bool m_first_time{ true };

#ifdef RE8
    Vector3f m_custom_angles{};
#endif
};