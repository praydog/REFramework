#pragma once

#include "Mod.hpp"

#include "sdk/ReClass.hpp"
#include "sdk/helpers/NativeObject.hpp"

class FreeCam : public Mod {
public:
    FreeCam() = default;

    std::string_view get_name() const override { return "FreeCam"; }

    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    void on_frame() override;
    void on_draw_ui() override;
    void on_update_transform(RETransform* transform) override;
    void on_pre_application_entry(void* entry, const char* name, size_t hash) override;

private:
    bool update_pointers();

    const ModToggle::Ptr m_enabled{              ModToggle::create(generate_name("Enabled"), false) };
    const ModToggle::Ptr m_lock_camera{          ModToggle::create(generate_name("LockPosition"), false) };
    const ModToggle::Ptr m_disable_movement{     ModToggle::create(generate_name("DisableMovement"), false) };
    const ModKey::Ptr m_toggle_key{              ModKey::create(generate_name("ToggleKey_V2")) };
    const ModKey::Ptr m_lock_camera_key{         ModKey::create(generate_name("LockKey_V2")) };
    const ModKey::Ptr m_move_up_key{             ModKey::create(generate_name("MoveUpKey_V2"), VK_SPACE) };
    const ModKey::Ptr m_move_down_key{           ModKey::create(generate_name("MoveDownKey_V2"), VK_LCONTROL) };
    const ModKey::Ptr m_disable_movement_key{    ModKey::create(generate_name("DisableMoveKey_V2")) };
    const ModKey::Ptr m_speed_modifier_fast_key{ ModKey::create(generate_name("SpeedModifierFast_V2"), VK_LSHIFT)};
    const ModKey::Ptr m_speed_modifier_slow_key{ ModKey::create(generate_name("SpeedModifierSlow_V2"), VK_LMENU)};

    const ModSlider::Ptr m_speed{ ModSlider::create(generate_name("Speed"), 0.0f, 1.0f, 0.1f) };
    const ModSlider::Ptr m_speed_modifier{ ModSlider::create(generate_name("SpeedModifier"), 1.f, 50.f, 4.f) };

    const ModSlider::Ptr m_rotation_speed{ ModSlider::create(generate_name("RotationSpeed"), 0.0f, 1.0f, 0.1f) };

    ValueList m_options{
        *m_enabled,
        *m_lock_camera,
        *m_disable_movement,
        *m_toggle_key,
        *m_lock_camera_key,
        *m_move_up_key,
        *m_move_down_key,
        *m_disable_movement_key,
        *m_speed,
        *m_speed_modifier,
        *m_speed_modifier_fast_key,
        *m_speed_modifier_slow_key,
        *m_rotation_speed,
    };

#if defined(RE2) || defined(RE3)
    RopewaySurvivorManager* m_survivor_manager{ nullptr };
#endif

#ifdef RE8
    AppPropsManager* m_props_manager{ nullptr };
#endif

    sdk::helpers::NativeObject m_via_hid_gamepad{ "via.hid.GamePad" };
    sdk::helpers::NativeObject m_application{ "via.Application" };

    Matrix4x4f m_last_camera_matrix{ glm::identity<Matrix4x4f>() };

    bool m_first_time{ true };
    bool m_was_disabled{ false };

    Vector3f m_custom_angles{};
    float m_twist{0.0f};

    RECamera* m_camera{nullptr};

#ifdef RE4
    struct {
        bool attempted_hook{false};
        std::optional<size_t> get_past_move_frame_move_dir_vec_id{};
        std::optional<size_t> update_id{};
        std::optional<size_t> late_update_id{};
    } m_player_body_updater_hook{};

    struct {
        bool attempted_hook{false};
        std::optional<size_t> change_motion_internal_id{};
    } m_player_motion_controller_hook{};

    REManagedObject* m_re4_body{nullptr};
#endif
};