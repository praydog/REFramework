#pragma once

#include <chrono>
#include <mutex>

#include "Mod.hpp"
#include "utility/Patch.hpp"

class FirstPerson : public Mod {
public:
    FirstPerson();

    std::string_view get_name() const override { return "FirstPerson"; };
    std::optional<std::string> on_initialize() override;

    void on_frame() override;
    void on_draw_ui() override;

    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    void on_pre_update_transform(RETransform* transform) override;
    void on_update_transform(RETransform* transform) override;
    void on_update_camera_controller(RopewayPlayerCameraController* controller) override;
    void on_update_camera_controller2(RopewayPlayerCameraController* controller) override;

protected:
    // gross
    bool list_box_handler_attach(void* data, int idx, const char** out_text) {
        *out_text = ((decltype(m_attach_names)*)data)->at(idx).data();
        return true;
    }

private:
    void reset();
    void set_vignette(via::render::ToneMapping::Vignetting value);
    bool update_pointers_from_camera_system(RopewayCameraSystem* camera_system);
    void update_camera_transform(RETransform* transform);
    void update_sweet_light_context(RopewaySweetLightManagerContext* ctx);
    void update_player_bones(RETransform* transform);
    void update_fov(RopewayPlayerCameraController* controller);
    void update_joint_names();
    float update_delta_time(REComponent* component);

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
    Vector4f m_last_controller_pos{};
    glm::quat m_last_controller_rotation{};

    float m_last_fov_mult{ 0.0f };

    RETransform* m_player_transform{ nullptr };
    RECamera* m_camera{ nullptr };
    RopewayPlayerCameraController* m_player_camera_controller{ nullptr };
    RopewayCameraSystem* m_camera_system{ nullptr };
    RopewaySweetLightManager* m_sweet_light_manager{ nullptr };
    RopewayPostEffectController* m_post_effect_controller{ nullptr };
    RopewayPostEffectControllerBase* m_tone_mapping_controller{ nullptr };

    std::vector<std::string> m_attach_names;
    int32_t m_attach_selected{ 0 };
    
    //std::unique_ptr<Patch> m_disableVignettePatch{};

    const ModToggle::Ptr m_enabled{ ModToggle::create(generate_name("Enabled")) };
    const ModKey::Ptr m_toggle_key{ ModKey::create(generate_name("ToggleKey")) };
    void on_disabled();

    const ModToggle::Ptr m_disable_vignette{ ModToggle::create(generate_name("DisableVignette")) };
    const ModToggle::Ptr m_hide_mesh{ ModToggle::create(generate_name("HideJointMesh"), true) };
    const ModToggle::Ptr m_rotate_mesh{ ModToggle::create(generate_name("ForceRotateMesh"), true) };
    const ModToggle::Ptr m_disable_light_source{ ModToggle::create(generate_name("DisableLightSource"), true) };
    const ModToggle::Ptr m_show_in_cutscenes{ ModToggle::create(generate_name("ShowInCutscenes"), false) };

    const ModSlider::Ptr m_fov_offset{ ModSlider::create(generate_name("FOVOffset"), -100.0f, 100.0f, 10.0f) };
    const ModSlider::Ptr m_fov_mult{ ModSlider::create(generate_name("FOVMultiplier"), 0.0f, 2.0f, 1.0f) };

    const ModSlider::Ptr m_camera_scale{ ModSlider::create(generate_name("CameraSpeed"), 0.0f, 100.0f, 40.0f) };
    const ModSlider::Ptr m_bone_scale{ ModSlider::create(generate_name("CameraShake"), 0.0f, 100.0f, 15.0f) };

    // just used to draw. not actually stored in config
    const ModFloat::Ptr m_current_fov{ ModFloat::create("") };

    ValueList m_options{
        *m_enabled,
        *m_toggle_key,
        *m_disable_vignette,
        *m_hide_mesh,
        *m_rotate_mesh,
        *m_disable_light_source,
        *m_show_in_cutscenes,
        *m_fov_offset,
        *m_fov_mult,
        *m_camera_scale,
        *m_bone_scale,
        *m_current_fov
    };
};