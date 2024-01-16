#pragma once

#include <chrono>
#include <bitset>
#include <memory>
#include <shared_mutex>

#include <openvr.h>

#include <d3d11.h>
#include <d3d12.h>
#include <dxgi.h>
#include <wrl.h>

#include "utility/Patch.hpp"
#include "sdk/Math.hpp"
#include "sdk/helpers/NativeObject.hpp"
#include "sdk/Renderer.hpp"
#include "vr/D3D11Component.hpp"
#include "vr/D3D12Component.hpp"
#include "vr/OverlayComponent.hpp"
#include "vr/runtimes/OpenXR.hpp"
#include "vr/runtimes/OpenVR.hpp"

#include "Mod.hpp"

class REManagedObject;

class VR : public Mod {
public:
    static std::shared_ptr<VR>& get();

    std::string_view get_name() const override { return "VR"; }

    // Called when the mod is initialized
    std::optional<std::string> on_initialize_d3d_thread() override;

    void on_lua_state_created(sol::state& lua) override;

    void on_pre_imgui_frame() override;
    void on_present() override;
    void on_post_present() override;
    void on_update_transform(RETransform* transform) override;
    void on_update_camera_controller(RopewayPlayerCameraController* controller) override;
    bool on_pre_gui_draw_element(REComponent* gui_element, void* primitive_context) override;
    void on_gui_draw_element(REComponent* gui_element, void* primitive_context) override;
    void on_pre_update_before_lock_scene(void* ctx) override;
    void on_pre_lightshaft_draw(void* shaft, void* render_context) override;
    void on_lightshaft_draw(void* shaft, void* render_context) override;

    void on_pre_application_entry(void* entry, const char* name, size_t hash) override;
    void on_application_entry(void* entry, const char* name, size_t hash) override;

    void on_draw_ui() override;
    void on_device_reset() override;

    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    // Application entries
    void on_pre_update_hid(void* entry);
    void on_update_hid(void* entry);
    void on_pre_begin_rendering(void* entry);
    void on_begin_rendering(void* entry);
    void on_pre_end_rendering(void* entry);
    void on_end_rendering(void* entry);
    void on_pre_wait_rendering(void* entry);
    void on_wait_rendering(void* entry);

    template<typename T = VRRuntime>
    T* get_runtime() const {
        return (T*)m_runtime.get();
    }

    auto get_hmd() const {
        return m_openvr->hmd;
    }

    auto& get_openvr_poses() const {
        return m_openvr->render_poses;
    }

    auto get_hmd_width() const {
        return get_runtime()->get_width();
    }

    auto get_hmd_height() const {
        return get_runtime()->get_height();
    }

    auto get_last_controller_update() const {
        return m_last_controller_update;
    }

    int32_t get_frame_count() const;
    int32_t get_game_frame_count() const;

    bool is_using_afr() const {
        return m_use_afr->value();
    }

    // Functions that generally use a mutex or have more complex logic
    float get_standing_height();
    Vector4f get_standing_origin();
    void set_standing_origin(const Vector4f& origin);

    glm::quat get_rotation_offset();
    void set_rotation_offset(const glm::quat& offset);
    void recenter_view();

    glm::quat get_gui_rotation_offset();
    void set_gui_rotation_offset(const glm::quat& offset);
    void recenter_gui(const glm::quat& from);

    Vector4f get_current_offset();

    Matrix4x4f get_current_eye_transform(bool flip = false);
    Matrix4x4f get_current_projection_matrix(bool flip = false);

    auto& get_controllers() const {
        return m_controllers;
    }

    bool is_using_controllers() const {
        return !m_controllers.empty() && (std::chrono::steady_clock::now() - m_last_controller_update) <= std::chrono::seconds((int32_t)m_motion_controls_inactivity_timer->value());
    }

    bool is_hmd_active() const {
        return get_runtime()->ready();
    }
    
    bool is_openvr_loaded() const {
        return m_openvr != nullptr && m_openvr->loaded;
    }

    bool is_openxr_loaded() const {
        return m_openxr != nullptr && m_openxr->loaded;
    }

    bool is_using_hmd_oriented_audio() {
        return m_hmd_oriented_audio->value();
    }

    void toggle_hmd_oriented_audio() {
        m_hmd_oriented_audio->toggle();
    }

    const Matrix4x4f& get_last_render_matrix() {
        return m_render_camera_matrix;
    }

    Vector4f get_position(uint32_t index)  const;
    Vector4f get_velocity(uint32_t index)  const;
    Vector4f get_angular_velocity(uint32_t index)  const;
    Matrix4x4f get_rotation(uint32_t index)  const;
    Matrix4x4f get_transform(uint32_t index) const;
    vr::HmdMatrix34_t get_raw_transform(uint32_t index) const;

    const auto& get_eyes() const {
        return get_runtime()->eyes;
    }

    void apply_hmd_transform(glm::quat& rotation, Vector4f& position);
    void apply_hmd_transform(::REJoint* camera_joint);
    
    bool is_hand_behind_head(VRRuntime::Hand hand, float sensitivity = 0.2f) const;
    bool is_action_active(vr::VRActionHandle_t action, vr::VRInputValueHandle_t source = vr::k_ulInvalidInputValueHandle) const;
    Vector2f get_joystick_axis(vr::VRInputValueHandle_t handle) const;

    Vector2f get_left_stick_axis() const;
    Vector2f get_right_stick_axis() const;

    void trigger_haptic_vibration(float seconds_from_now, float duration, float frequency, float amplitude, vr::VRInputValueHandle_t source = vr::k_ulInvalidInputValueHandle);
    
    auto get_action_set() const { return m_action_set; }
    auto& get_active_action_set() const { return m_active_action_set; }
    auto get_action_trigger() const { return m_action_trigger; }
    auto get_action_grip() const { return m_action_grip; }
    auto get_action_joystick() const { return m_action_joystick; }
    auto get_action_joystick_click() const { return m_action_joystick_click; }
    auto get_action_a_button() const { return m_action_a_button; }
    auto get_action_b_button() const { return m_action_b_button; }
    auto get_action_weapon_dial() const { return m_action_weapon_dial; }
    auto get_action_minimap() const { return m_action_minimap; }
    auto get_action_block() const { return m_action_block; }
    auto get_action_dpad_up() const { return m_action_dpad_up; }
    auto get_action_dpad_down() const { return m_action_dpad_down; }
    auto get_action_dpad_left() const { return m_action_dpad_left; }
    auto get_action_dpad_right() const { return m_action_dpad_right; }
    auto get_action_heal() const { return m_action_heal; }
    auto get_left_joystick() const { return m_left_joystick; }
    auto get_right_joystick() const { return m_right_joystick; }

    const auto& get_action_handles() const { return m_action_handles;}

    auto get_ui_scale() const { return m_ui_scale_option->value(); }
    const auto& get_raw_projections() const { return get_runtime()->raw_projections; }

    void unhide_crosshair() {
        m_last_crosshair_hide = std::chrono::steady_clock::now();
    }

private:
    Vector4f get_position_unsafe(uint32_t index) const;
    Vector4f get_velocity_unsafe(uint32_t index) const;
    Vector4f get_angular_velocity_unsafe(uint32_t index) const;

private:
    // Hooks
    void on_view_get_size(REManagedObject* scene_view, float* result) override;
    static void inputsystem_update_hook(void* ctx, REManagedObject* input_system);
    void on_camera_get_projection_matrix(REManagedObject* camera, Matrix4x4f* result) override;
    static Matrix4x4f* gui_camera_get_projection_matrix_hook(REManagedObject* camera, Matrix4x4f* result);
    void on_camera_get_view_matrix(REManagedObject* camera, Matrix4x4f* result) override;

    bool on_pre_overlay_layer_update(sdk::renderer::layer::Overlay* layer, void* render_context) override;
    bool on_pre_overlay_layer_draw(sdk::renderer::layer::Overlay* layer, void* render_context) override;

    bool on_pre_post_effect_layer_update(sdk::renderer::layer::PostEffect* layer, void* render_context) override;
    bool on_pre_post_effect_layer_draw(sdk::renderer::layer::PostEffect* layer, void* render_context) override;
    void on_post_effect_layer_draw(sdk::renderer::layer::PostEffect* layer, void* render_context) override;
    uint32_t m_previous_distortion_type{};
    bool m_set_next_post_effect_distortion_type{false};

    bool on_pre_scene_layer_update(sdk::renderer::layer::Scene* layer, void* render_context) override;
    void on_scene_layer_update(sdk::renderer::layer::Scene* layer, void* render_context) override;
    bool on_pre_scene_layer_draw(sdk::renderer::layer::Scene* layer, void* render_context) override;
    bool m_set_next_scene_layer_data{false};

    struct SceneLayerData {
        SceneLayerData() = default;
        SceneLayerData(sdk::renderer::SceneInfo* info) 
            : scene_info(info)
        {
            if (scene_info != nullptr) {
                this->view_projection_matrix = scene_info->view_projection_matrix;
            }
        }

        sdk::renderer::SceneInfo* scene_info{};
        Matrix4x4f view_projection_matrix{};
    };

    std::array<SceneLayerData, 5> m_scene_layer_data {};

    static void wwise_listener_update_hook(void* listener);

    //static float get_sharpness_hook(void* tonemapping);

    // initialization functions
    std::optional<std::string> initialize_openvr();
    std::optional<std::string> initialize_openvr_input();
    std::optional<std::string> initialize_openxr();
    std::optional<std::string> initialize_openxr_input();
    std::optional<std::string> initialize_openxr_swapchains();
    std::optional<std::string> hijack_resolution();
    std::optional<std::string> hijack_input();
    std::optional<std::string> hijack_camera();
    std::optional<std::string> hijack_wwise_listeners(); // audio hook

    std::optional<std::string> reinitialize_openvr() {
        spdlog::info("Reinitializing OpenVR");
        std::scoped_lock _{m_openvr_mtx};

        m_runtime.reset();
        m_runtime = std::make_shared<VRRuntime>();
        m_openvr.reset();

        // Reinitialize openvr input, hopefully this fixes the issue
        m_controllers.clear();
        m_controllers_set.clear();

        auto e = initialize_openvr();

        if (e) {
            spdlog::error("Failed to reinitialize OpenVR: {}", *e);
        }

        return e;
    }

    std::optional<std::string> reinitialize_openxr() {
        spdlog::info("Reinitializing OpenXR");
        std::scoped_lock _{m_openvr_mtx};

        if (m_is_d3d12) {
            m_d3d12.openxr().destroy_swapchains();
        } else {
            m_d3d11.openxr().destroy_swapchains();
        }

        m_openxr.reset();
        m_runtime.reset();
        m_runtime = std::make_shared<VRRuntime>();
        
        m_controllers.clear();
        m_controllers_set.clear();

        auto e = initialize_openxr();

        if (e) {
            spdlog::error("Failed to reinitialize OpenXR: {}", *e);
        }

        return e;
    }

    bool detect_controllers();
    bool is_any_action_down();
    void update_hmd_state();
    void update_action_states();
    void update_camera(); // if not in firstperson mode
    void update_camera_origin(); // every frame
    void update_audio_camera();
    void update_render_matrix();
    void restore_audio_camera(); // after wwise listener update
    void restore_camera(); // After rendering
    void set_lens_distortion(bool value);
    void disable_bad_effects();
    void fix_temporal_effects();

    // input functions
    // Purpose: "Emulate" OpenVR input to the game
    // By setting things like input flags based on controller state
    void openvr_input_to_re2_re3(REManagedObject* input_system);
    void openvr_input_to_re_engine(); // generic, can be used on any game

    // Sets overlay layer to return instantly
    // causes world-space gui elements to render properly
    Patch::Ptr m_overlay_draw_patch{};
    
    mutable std::recursive_mutex m_openvr_mtx{};
    mutable std::recursive_mutex m_wwise_mtx{};
    mutable std::shared_mutex m_gui_mtx{};
    mutable std::shared_mutex m_rotation_mtx{};

    vr::VRTextureBounds_t m_right_bounds{ 0.0f, 0.0f, 1.0f, 1.0f };
    vr::VRTextureBounds_t m_left_bounds{ 0.0f, 0.0f, 1.0f, 1.0f };

    glm::vec3 m_overlay_rotation{-1.550f, 0.0f, -1.330f};
    glm::vec4 m_overlay_position{0.0f, 0.06f, -0.07f, 1.0f};

    float m_nearz{ 0.1f };
    float m_farz{ 3000.0f };

    std::shared_ptr<VRRuntime> m_runtime{std::make_shared<VRRuntime>()}; // will point to the real runtime if it exists
    std::shared_ptr<runtimes::OpenVR> m_openvr{std::make_shared<runtimes::OpenVR>()};
    std::shared_ptr<runtimes::OpenXR> m_openxr{std::make_shared<runtimes::OpenXR>()};

    Vector4f m_standing_origin{ 0.0f, 1.5f, 0.0f, 0.0f };
    glm::quat m_rotation_offset{ glm::identity<glm::quat>() };
    glm::quat m_gui_rotation_offset{ glm::identity<glm::quat>() };

    std::vector<int32_t> m_controllers{};
    std::unordered_set<int32_t> m_controllers_set{};

    // Action set handles
    vr::VRActionSetHandle_t m_action_set{};
    vr::VRActiveActionSet_t m_active_action_set{};

    // Action handles
    vr::VRActionHandle_t m_action_trigger{ };
    vr::VRActionHandle_t m_action_grip{ };
    vr::VRActionHandle_t m_action_joystick{};
    vr::VRActionHandle_t m_action_joystick_click{};
    vr::VRActionHandle_t m_action_a_button{};
    vr::VRActionHandle_t m_action_b_button{};
    vr::VRActionHandle_t m_action_dpad_up{};
    vr::VRActionHandle_t m_action_dpad_right{};
    vr::VRActionHandle_t m_action_dpad_down{};
    vr::VRActionHandle_t m_action_dpad_left{};
    vr::VRActionHandle_t m_action_system_button{};
    vr::VRActionHandle_t m_action_weapon_dial{};
    vr::VRActionHandle_t m_action_re3_dodge{};
    vr::VRActionHandle_t m_action_re2_quickturn{};
    vr::VRActionHandle_t m_action_re2_firstperson_toggle{};
    vr::VRActionHandle_t m_action_re2_reset_view{};
    vr::VRActionHandle_t m_action_re2_change_ammo{};
    vr::VRActionHandle_t m_action_re2_toggle_flashlight{};
    vr::VRActionHandle_t m_action_minimap{};
    vr::VRActionHandle_t m_action_block{};
    vr::VRActionHandle_t m_action_haptic{};
    vr::VRActionHandle_t m_action_heal{};

    bool m_was_firstperson_toggle_down{false};
    bool m_was_flashlight_toggle_down{false};
    
    
    std::unordered_map<std::string, std::reference_wrapper<vr::VRActionHandle_t>> m_action_handles {
        { "/actions/default/in/Trigger", m_action_trigger },
        { "/actions/default/in/Grip", m_action_grip },
        { "/actions/default/in/Joystick", m_action_joystick },
        { "/actions/default/in/JoystickClick", m_action_joystick_click },
        { "/actions/default/in/AButton", m_action_a_button },
        { "/actions/default/in/BButton", m_action_b_button },
        { "/actions/default/in/DPad_Up", m_action_dpad_up },
        { "/actions/default/in/DPad_Right", m_action_dpad_right },
        { "/actions/default/in/DPad_Down", m_action_dpad_down },
        { "/actions/default/in/DPad_Left", m_action_dpad_left },
        { "/actions/default/in/SystemButton", m_action_system_button },
        { "/actions/default/in/WeaponDial_Start", m_action_weapon_dial },
        { "/actions/default/in/RE3_Dodge", m_action_re3_dodge },
        { "/actions/default/in/RE2_Quickturn", m_action_re2_quickturn },
        { "/actions/default/in/RE2_FirstPerson_Toggle", m_action_re2_firstperson_toggle },
        { "/actions/default/in/RE2_Reset_View", m_action_re2_reset_view },
        { "/actions/default/in/RE2_Change_Ammo", m_action_re2_change_ammo },
        { "/actions/default/in/RE2_Toggle_Flashlight", m_action_re2_toggle_flashlight },
        { "/actions/default/in/MiniMap", m_action_minimap },
        { "/actions/default/in/Block", m_action_block },
        { "/actions/default/in/Heal", m_action_heal },

        // Out
        { "/actions/default/out/Haptic", m_action_haptic },
    };

    // Input sources
    vr::VRInputValueHandle_t m_left_joystick{};
    vr::VRInputValueHandle_t m_right_joystick{};

    // Input system history
    std::bitset<64> m_button_states_down{};
    std::bitset<64> m_button_states_on{};
    std::bitset<64> m_button_states_up{};
    std::chrono::steady_clock::time_point m_last_controller_update{};
    std::chrono::steady_clock::time_point m_last_interaction_display{};
    std::chrono::steady_clock::time_point m_last_crosshair_hide{};
    uint32_t m_backbuffer_inconsistency_start{};
    std::chrono::nanoseconds m_last_input_delay{};
    std::chrono::nanoseconds m_avg_input_delay{};

    HANDLE m_present_finished_event{CreateEvent(nullptr, TRUE, FALSE, nullptr)};

    Vector4f m_raw_projections[2]{};

    vrmod::D3D11Component m_d3d11{};
    vrmod::D3D12Component m_d3d12{};
    vrmod::OverlayComponent m_overlay_component{};

    Vector4f m_original_camera_position{ 0.0f, 0.0f, 0.0f, 0.0f };
    glm::quat m_original_camera_rotation{ glm::identity<glm::quat>() };

    Matrix4x4f m_original_camera_matrix{ glm::identity<Matrix4x4f>() };

    Vector4f m_original_audio_camera_position{ 0.0f, 0.0f, 0.0f, 0.0f };
    glm::quat m_original_audio_camera_rotation{ glm::identity<glm::quat>() };

    Matrix4x4f m_render_camera_matrix{ glm::identity<Matrix4x4f>() };

    sdk::helpers::NativeObject m_via_hid_gamepad{ "via.hid.GamePad" };

    // options
    int m_frame_count{};
    int m_render_frame_count{};
    int m_last_frame_count{-1};
    int m_left_eye_frame_count{0};
    int m_right_eye_frame_count{0};

    bool m_submitted{false};
    //bool m_disable_sharpening{true};

    bool m_needs_camera_restore{false};
    bool m_needs_audio_restore{false};
    bool m_in_render{false};
    bool m_in_lightshaft{false};
    bool m_positional_tracking{true};
    bool m_is_d3d12{false};
    bool m_backbuffer_inconsistency{false};
    bool m_init_finished{false};
    bool m_has_hw_scheduling{false}; // hardware accelerated GPU scheduling

    // on the backburner
    bool m_depth_aided_reprojection{false};

    // == 1 or == 0
    uint8_t m_left_eye_interval{0};
    uint8_t m_right_eye_interval{1};

    static std::string actions_json;
    static std::string binding_rift_json;
    static std::string bindings_oculus_touch_json;
    static std::string binding_vive;
    static std::string bindings_vive_controller;
    static std::string bindings_knuckles;

    const std::unordered_map<std::string, std::string> m_binding_files {
        { "actions.json", actions_json },
        { "binding_rift.json", binding_rift_json },
        { "bindings_oculus_touch.json", bindings_oculus_touch_json },
        { "binding_vive.json", binding_vive },
        { "bindings_vive_controller.json", bindings_vive_controller },
        { "bindings_knuckles.json", bindings_knuckles }
    };

    const ModKey::Ptr m_set_standing_key{ ModKey::create(generate_name("SetStandingOriginKey")) };
    const ModKey::Ptr m_recenter_view_key{ ModKey::create(generate_name("RecenterViewKey")) };
    const ModToggle::Ptr m_decoupled_pitch{ ModToggle::create(generate_name("DecoupledPitch"), false) };
    const ModToggle::Ptr m_use_afr{ ModToggle::create(generate_name("AlternateFrameRendering"), false) };
    const ModToggle::Ptr m_use_custom_view_distance{ ModToggle::create(generate_name("UseCustomViewDistance"), false) };
    const ModToggle::Ptr m_hmd_oriented_audio{ ModToggle::create(generate_name("HMDOrientedAudio"), true) };
    const ModSlider::Ptr m_view_distance{ ModSlider::create(generate_name("CustomViewDistance"), 10.0f, 3000.0f, 500.0f) };
    const ModSlider::Ptr m_motion_controls_inactivity_timer{ ModSlider::create(generate_name("MotionControlsInactivityTimer"), 30.0f, 100.0f, 30.0f) };
    const ModSlider::Ptr m_joystick_deadzone{ ModSlider::create(generate_name("JoystickDeadzone"), 0.01f, 0.9f, 0.15f) };
    const ModSlider::Ptr m_ui_scale_option{ ModSlider::create(generate_name("2DUIScale"), 1.0f, 100.0f, 12.0f) };
    const ModSlider::Ptr m_ui_distance_option{ ModSlider::create(generate_name("2DUIDistance"), 0.01f, 100.0f, 1.0f) };
    const ModSlider::Ptr m_world_ui_scale_option{ ModSlider::create(generate_name("WorldSpaceUIScale"), 1.0f, 100.0f, 15.0f) };
    const ModSlider::Ptr m_resolution_scale{ ModSlider::create(generate_name("OpenXRResolutionScale"), 0.1f, 5.0f, 1.0f) };

    const ModToggle::Ptr m_force_fps_settings{ ModToggle::create(generate_name("ForceFPS"), true) };
    const ModToggle::Ptr m_force_aa_settings{ ModToggle::create(generate_name("ForceAntiAliasing"), true) };
    const ModToggle::Ptr m_force_motionblur_settings{ ModToggle::create(generate_name("ForceMotionBlur"), true) };
    const ModToggle::Ptr m_force_vsync_settings{ ModToggle::create(generate_name("ForceVSync"), true) };
    const ModToggle::Ptr m_force_lensdistortion_settings{ ModToggle::create(generate_name("ForceLensDistortion"), true) };
    const ModToggle::Ptr m_force_volumetrics_settings{ ModToggle::create(generate_name("ForceVolumetrics"), true) };
    const ModToggle::Ptr m_force_lensflares_settings{ ModToggle::create(generate_name("ForceLensFlares"), true) };
    const ModToggle::Ptr m_force_dynamic_shadows_settings{ ModToggle::create(generate_name("ForceDynamicShadows"), true) };
    const ModToggle::Ptr m_allow_engine_overlays{ ModToggle::create(generate_name("AllowEngineOverlays"), true) };
    const ModToggle::Ptr m_desktop_fix{ ModToggle::create(generate_name("DesktopRecordingFix"), true) };
    const ModToggle::Ptr m_desktop_fix_skip_present{ ModToggle::create(generate_name("DesktopRecordingFixSkipPresent"), true) };
    const ModToggle::Ptr m_enable_asynchronous_rendering{ ModToggle::create(generate_name("AsyncRendering_V2"), true) };

    bool m_disable_projection_matrix_override{ false };
    bool m_disable_gui_camera_projection_matrix_override{ false };
    bool m_disable_view_matrix_override{false};
    bool m_disable_backbuffer_size_override{false};
    bool m_disable_temporal_fix{false};
    bool m_disable_post_effect_fix{false};

    ValueList m_options{
        *m_set_standing_key,
        *m_recenter_view_key,
        *m_decoupled_pitch,
        *m_use_afr,
        *m_use_custom_view_distance,
        *m_hmd_oriented_audio,
        *m_view_distance,
        *m_motion_controls_inactivity_timer,
        *m_joystick_deadzone,
        *m_force_fps_settings,
        *m_force_aa_settings,
        *m_force_motionblur_settings,
        *m_force_vsync_settings,
        *m_force_lensdistortion_settings,
        *m_force_volumetrics_settings,
        *m_force_lensflares_settings,
        *m_force_dynamic_shadows_settings,
        *m_ui_scale_option,
        *m_ui_distance_option,
        *m_world_ui_scale_option,
        *m_allow_engine_overlays,
        *m_resolution_scale,
        *m_desktop_fix,
        *m_desktop_fix_skip_present,
        *m_enable_asynchronous_rendering
    };

    bool m_use_rotation{true};

    friend class vrmod::D3D11Component;
    friend class vrmod::D3D12Component;
    friend class vrmod::OverlayComponent;
};
