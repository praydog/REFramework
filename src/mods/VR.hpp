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
#include "vr/D3D11Component.hpp"
#include "vr/D3D12Component.hpp"
#include "vr/OverlayComponent.hpp"

#include "Mod.hpp"

class REManagedObject;

class VR : public Mod {
public:
    static std::shared_ptr<VR>& get();

    std::string_view get_name() const override { return "VR"; }

    // Called when the mod is initialized
    std::optional<std::string> on_initialize() override;

    void on_pre_imgui_frame() override;
    void on_frame() override;
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

    // Application entries
    void on_pre_begin_rendering(void* entry);
    void on_begin_rendering(void* entry);
    void on_pre_end_rendering(void* entry);
    void on_end_rendering(void* entry);
    void on_pre_wait_rendering(void* entry);
    void on_wait_rendering(void* entry);

    void on_draw_ui() override;
    void on_device_reset() override;

    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    auto get_hmd() const {
        return m_hmd;
    }

    auto& get_poses() const {
        return m_render_poses;
    }

    auto get_hmd_width() const {
        return m_w;
    }

    auto get_hmd_height() const {
        return m_h;
    }

    auto get_last_controller_update() const {
        return m_last_controller_update;
    }

    int32_t get_frame_count() const;
    int32_t get_game_frame_count() const;

    bool is_using_afr() const {
        return m_use_afr;
    }

    // Functions that generally use a mutex or have more complex logic
    float get_standing_height();
    Vector4f get_standing_origin();

    Vector4f get_current_offset();

    Matrix4x4f get_current_eye_transform(bool flip = false);
    Matrix4x4f get_current_rotation_offset();
    Matrix4x4f get_current_projection_matrix(bool flip = false);

    auto& get_controllers() const {
        return m_controllers;
    }

    bool is_using_controllers() const {
        return (std::chrono::steady_clock::now() - m_last_controller_update) <= std::chrono::seconds(10);
    }

    bool is_hmd_active() const {
        return m_is_hmd_active && m_wgp_initialized;
    }

    Vector4f get_position(uint32_t index);
    Matrix4x4f get_rotation(uint32_t index);
    Matrix4x4f get_transform(uint32_t index);
    vr::HmdMatrix34_t get_raw_transform(uint32_t index);

    auto& get_pose_mutex() {
        return m_pose_mtx;
    }

    const auto& get_eyes() const {
        return m_eyes;
    }
    
    bool is_action_active(vr::VRActionHandle_t action, vr::VRInputValueHandle_t source = vr::k_ulInvalidInputValueHandle) const;
    Vector2f get_joystick_axis(vr::VRInputValueHandle_t handle) const;

    Vector2f get_left_stick_axis() const;
    Vector2f get_right_stick_axis() const;

    auto get_ui_scale() const { return m_ui_scale; }
    const auto& get_raw_projections() const { return m_raw_projections; }

private:
    Vector4f get_position_unsafe(uint32_t index);

private:
    // Hooks
    static float* get_size_hook(REManagedObject* scene_view, float* result);
    static void inputsystem_update_hook(void* ctx, REManagedObject* input_system);
    static Matrix4x4f* camera_get_projection_matrix_hook(REManagedObject* camera, Matrix4x4f* result);
    static Matrix4x4f* camera_get_view_matrix_hook(REManagedObject* camera, Matrix4x4f* result);
    static void overlay_draw_hook(void* layer, void* render_context);

    //static float get_sharpness_hook(void* tonemapping);

    // initialization functions
    std::optional<std::string> initialize_openvr();
    std::optional<std::string> initialize_openvr_input();
    std::optional<std::string> hijack_resolution();
    std::optional<std::string> hijack_input();
    std::optional<std::string> hijack_camera();
    std::optional<std::string> hijack_overlay_renderer();

    std::optional<std::string> reinitialize_openvr() {
        spdlog::info("Reinitializing openvr");

        vr::VR_Shutdown();

        // Reinitialize openvr input, hopefully this fixes the issue
        m_controllers.clear();
        m_controllers_set.clear();

        auto input_error = initialize_openvr();

        if (input_error) {
            spdlog::error("Failed to reinitialize openvr: {}", *input_error);
        }

        return input_error;
    }

    bool detect_controllers();
    void update_hmd_state();
    void update_camera(); // if not in firstperson mode
    void update_camera_origin(); // every frame
    void restore_camera(); // After rendering
    void set_lens_distortion(bool value);
    void disable_bad_effects();

    // input functions
    // Purpose: "Emulate" OpenVR input to the game
    // By setting things like input flags based on controller state
    void openvr_input_to_game(REManagedObject* input_system);

    // Sets overlay layer to return instantly
    // causes world-space gui elements to render properly
    Patch::Ptr m_overlay_draw_patch{};
    
    std::mutex m_afr_mtx{};
    std::shared_mutex m_pose_mtx{};
    std::shared_mutex m_eyes_mtx{};

    REManagedObject* m_main_view{nullptr};

    vr::VRTextureBounds_t m_right_bounds{ 0.0f, 0.0f, 1.0f, 1.0f };
    vr::VRTextureBounds_t m_left_bounds{ 0.0f, 0.0f, 1.0f, 1.0f };

    glm::vec3 m_overlay_rotation{-1.550f, 0.0f, -1.330f};
    glm::vec4 m_overlay_position{0.0f, 0.06f, -0.05f, 1.0f};

    float m_nearz{ 0.1f };
    float m_farz{ 3000.0f };

    vr::IVRSystem* m_hmd{nullptr};

    // Poses
    std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> m_real_render_poses;
    std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> m_real_game_poses;

    std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> m_render_poses;
    std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> m_game_poses;

    Vector4f m_standing_origin{ 0.0f, 1.5f, 0.0f, 0.0f };

    std::vector<int32_t> m_controllers{};
    std::unordered_set<int32_t> m_controllers_set{};

    std::array<Matrix4x4f, 2> m_eyes{};
    std::array<Matrix4x4f, 2> m_projections{};

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

    // Input sources
    vr::VRInputValueHandle_t m_left_joystick{};
    vr::VRInputValueHandle_t m_right_joystick{};

    // Input system history
    std::bitset<64> m_button_states_down{};
    std::bitset<64> m_button_states_on{};
    std::bitset<64> m_button_states_up{};
    std::chrono::steady_clock::time_point m_last_controller_update{};

    std::condition_variable m_present_finished_cv{};
    std::mutex m_present_finished_mtx{};
    
    uint32_t m_w{0}, m_h{0};
    Vector4f m_raw_projections[2]{};

    vrmod::D3D11Component m_d3d11{};
    vrmod::D3D12Component m_d3d12{};
    vrmod::OverlayComponent m_overlay_component{};

    Vector4f m_original_camera_position{ 0.0f, 0.0f, 0.0f, 0.0f };
    glm::quat m_original_camera_rotation{ glm::identity<glm::quat>() };

    sdk::helpers::NativeObject m_via_hid_gamepad{ "via.hid.GamePad" };

    // options
    float m_ui_scale{15.0f};
    int m_frame_count{};
    int m_last_frame_count{-1};
    int m_left_eye_frame_count{0};
    int m_right_eye_frame_count{0};
    bool m_use_afr{false};
    bool m_submitted{false};
    bool m_present_finished{false};
    // we always need at least one initial WaitGetPoses before the game will render
    // even if we don't have anything to submit yet, otherwise the compositor
    // will return VRCompositorError_DoNotHaveFocus
    bool m_needs_wgp_update{true};
    //bool m_disable_sharpening{true};

    bool m_is_hmd_active{true};
    bool m_wgp_initialized{false};
    bool m_needs_camera_restore{false};
    bool m_in_render{false};
    bool m_in_lightshaft{false};
    bool m_request_reinitialize_openvr{false};
    bool m_positional_tracking{true};

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

    bool m_use_rotation{true};

    friend class vrmod::D3D11Component;
    friend class vrmod::D3D12Component;
    friend class vrmod::OverlayComponent;
};