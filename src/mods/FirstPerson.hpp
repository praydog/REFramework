#pragma once

#include <chrono>
#include <mutex>

#include "Mod.hpp"
#include "utility/Patch.hpp"

#if defined(RE2) || defined(RE3)

class FirstPerson : public Mod {
public:
    static std::shared_ptr<FirstPerson>& get();

public:
    FirstPerson();

    bool is_enabled() const {
        return m_enabled->value();
    }

    bool will_be_used() const {
        return is_enabled() && is_first_person_allowed() && m_player_transform != nullptr;
    }

    void toggle();

    std::string_view get_name() const override { return "FirstPerson"; };
    std::optional<std::string> on_initialize() override;

    void on_frame() override;
    void on_draw_ui() override;
    void on_lua_state_created(sol::state& state) override;

    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    void on_pre_update_transform(RETransform* transform) override;
    void on_update_transform(RETransform* transform) override;
    void on_update_camera_controller(RopewayPlayerCameraController* controller) override;
    void on_update_camera_controller2(RopewayPlayerCameraController* controller) override;

    void on_pre_application_entry(void* entry, const char* name, size_t hash) override;
    void on_application_entry(void* entry, const char* name, size_t hash) override;

    // non-virtual callbacks
    void on_pre_update_behavior(void* entry);
    void on_pre_late_update_behavior(void* entry);
    void on_post_late_update_behavior(void* entry);
    void on_post_update_motion(void* entry, bool true_motion = false);

    // non-virtual callbacks called from lua
    bool on_pre_flashlight_apply_transform(::REManagedObject* flashlight_component);

protected:
    // gross
    bool list_box_handler_attach(void* data, int idx, const char** out_text) {
        *out_text = ((decltype(m_attach_names)*)data)->at(idx).data();
        return true;
    }

private:
    void reset();
    void set_vignette(via::render::ToneMapping::Vignetting value);
    bool update_pointers();
    bool update_pointers_from_camera_system(RopewayCameraSystem* camera_system);
    void update_player_vr(RETransform* transform, bool first = false);
    void update_player_arm_ik(RETransform* transform);
    void update_player_muzzle_behavior(RETransform* transform, bool restore = false);
    void update_player_body_ik(RETransform* transform, bool restore = false, bool first = false);
    void update_player_body_rotation(RETransform* transform);
    void update_camera_transform(RETransform* transform);
    void update_sweet_light_context(RopewaySweetLightManagerContext* ctx);
    void update_player_bones(RETransform* transform);
    void update_fov(RopewayPlayerCameraController* controller);
    void update_joint_names();
    float update_delta_time(REComponent* component);
    bool is_first_person_allowed() const;

    // Needs to be recursive for some reason. Otherwise freeze.
    std::recursive_mutex m_matrix_mutex{};
    std::mutex m_frame_mutex{};
    std::mutex m_delta_mutex{};

    std::string m_attach_bone_imgui{ "head" };
    std::wstring m_attach_bone{ L"head" };
    std::string m_player_name{ "pl1000" };

    // Different "configs" for each model
    std::unordered_map<std::string, Vector4f> m_attach_offsets;
    
    Matrix4x4f m_rotation_offset{ glm::identity<Matrix4x4f>() };
    Matrix4x4f m_interpolated_bone{ glm::identity<Matrix4x4f>() };
    Matrix4x4f m_last_bone_matrix{ glm::identity<Matrix4x4f>() };
    Matrix4x4f m_last_camera_matrix{ glm::identity<Matrix4x4f>() };
    Matrix4x4f m_last_camera_matrix_pre_vr{ glm::identity<Matrix4x4f>() };
    Matrix4x4f m_last_camera_matrix_pre_cutscene{ glm::identity<Matrix4x4f>() };
    Matrix4x4f m_last_headset_rotation_pre_cutscene{ glm::identity<Matrix4x4f>() };
    Matrix4x4f* m_cached_bone_matrix{ nullptr };
    Vector4f m_last_controller_pos{};
    glm::quat m_last_controller_rotation{};
    glm::quat m_last_controller_rotation_vr{};
    Vector3f m_last_controller_angles{};
    bool m_has_cutscene_rotation{ false };
    bool m_ignore_next_player_angles{ false };
    app::ropeway::camera::CameraControlType m_last_camera_type{};

    // Don't show first person when the camera is not one of these
    std::unordered_set<app::ropeway::camera::CameraControlType> m_allowed_camera_types{
        app::ropeway::camera::CameraControlType::PLAYER, // normal gameplay
        app::ropeway::camera::CameraControlType::EVENT, // cutscene
        app::ropeway::camera::CameraControlType::ACTION, // grabbed by zombie or something similar
        //app::ropeway::camera::CameraControlType::GIMMICK_MOTION, // traversal cutscene
    };

    float m_last_player_fov{ 0.0f };
    float m_last_fov_mult{ 0.0f };
    float m_interp_camera_speed{ 100.0f };
    float m_interp_bone_scale{ 1.0f };
    float m_vr_scale{ 1.0f };

    Vector4f m_scale_debug{ 1.0f, 1.0f, 1.0f, 1.0f };
    Vector4f m_scale_debug2{ 1.0f, 1.0f, 1.0f, 1.0f };
    Vector4f m_offset_debug{ 1.0f, 1.0f, 1.0f, 1.0f };
    Vector4f m_left_hand_position_offset{ -0.05f, 0.05f, 0.15f, 0.0f };
    Vector4f m_right_hand_position_offset{ 0.05f, 0.05f, 0.15f, 0.0f };
    Vector3f m_left_hand_rotation_offset{ 0.4f, 2.4f, 1.7f };
    Vector3f m_right_hand_rotation_offset{ 0.2f, -2.5f, -1.7f };

    Vector3f m_last_controller_euler[2]{};

    RETransform* m_player_transform{ nullptr };
    RECamera* m_camera{ nullptr };
    RopewayPlayerCameraController* m_player_camera_controller{ nullptr };
    RopewayCameraSystem* m_camera_system{ nullptr };
    RopewaySweetLightManager* m_sweet_light_manager{ nullptr };
    RopewayPostEffectController* m_post_effect_controller{ nullptr };
    RopewayPostEffectControllerBase* m_tone_mapping_controller{ nullptr };

    // app::ropeway::gui::GuiMaster
    REBehavior* m_gui_master{ nullptr };

    std::vector<std::string> m_attach_names;
    int32_t m_attach_selected{ 0 };
    
    //std::unique_ptr<Patch> m_disableVignettePatch{};

    const ModToggle::Ptr m_enabled{ ModToggle::create(generate_name("Enabled")) };
    const ModKey::Ptr m_toggle_key{ ModKey::create(generate_name("ToggleKey")) };
    void on_disabled();
    bool m_wants_disable{false};
    bool m_was_hmd_active{false};


    const ModToggle::Ptr m_smooth_xz_movement{ ModToggle::create(generate_name("SmoothXZMovementVR"), true) };
    const ModToggle::Ptr m_smooth_y_movement{ ModToggle::create(generate_name("SmoothYMovementVR"), true) };
    const ModToggle::Ptr m_roomscale{ ModToggle::create(generate_name("RoomScale"), false) };
    const ModToggle::Ptr m_disable_vignette{ ModToggle::create(generate_name("DisableVignette"), true) };
    const ModToggle::Ptr m_hide_mesh{ ModToggle::create(generate_name("HideJointMesh"), true) };
    const ModToggle::Ptr m_rotate_mesh{ ModToggle::create(generate_name("ForceRotateMesh"), true) };
    const ModToggle::Ptr m_disable_light_source{ ModToggle::create(generate_name("DisableLightSource"), true) };
    const ModToggle::Ptr m_show_in_cutscenes{ ModToggle::create(generate_name("ShowInCutscenes"), false) };
    const ModToggle::Ptr m_rotate_body{ ModToggle::create(generate_name("RotateBody"), true) };
    const ModSlider::Ptr m_body_rotate_speed{ ModSlider::create(generate_name("BodyRotateSpeed"), 0.01f, 5.0f, 0.3f) };

    const ModSlider::Ptr m_fov_offset{ ModSlider::create(generate_name("FOVOffset"), -100.0f, 100.0f, 10.0f) };
    const ModSlider::Ptr m_fov_mult{ ModSlider::create(generate_name("FOVMultiplier"), 0.0f, 2.0f, 1.0f) };

    const ModSlider::Ptr m_camera_scale{ ModSlider::create(generate_name("CameraSpeed"), 0.0f, 100.0f, 40.0f) };
    const ModSlider::Ptr m_bone_scale{ ModSlider::create(generate_name("CameraShake"), 0.0f, 100.0f, 15.0f) };

    // just used to draw. not actually stored in config
    const ModFloat::Ptr m_current_fov{ ModFloat::create("") };

    ValueList m_options{
        *m_enabled,
        *m_toggle_key,
        *m_smooth_xz_movement,
        *m_smooth_y_movement,
        *m_disable_vignette,
        *m_hide_mesh,
        *m_rotate_mesh,
        *m_rotate_body,
        *m_body_rotate_speed,
        *m_disable_light_source,
        *m_show_in_cutscenes,
        *m_fov_offset,
        *m_fov_mult,
        *m_camera_scale,
        *m_bone_scale,
        *m_current_fov
    };
};

#endif