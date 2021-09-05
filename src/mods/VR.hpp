#pragma once

#include <bitset>
#include <memory>
#include <shared_mutex>
#include <openvr/headers/openvr.h>

#include <d3d11.h>
#include <d3d12.h>
#include <dxgi.h>
#include <wrl.h>

#include "sdk/Math.hpp"
#include "vr/D3D11Component.hpp"
#include "vr/D3D12Component.hpp"

#include "Mod.hpp"

class REManagedObject;

class VR : public Mod {
public:
    static std::shared_ptr<VR>& get();

    std::string_view get_name() const override { return "VR"; }

    // Called when the mod is initialized
    std::optional<std::string> on_initialize() override;

    void on_post_frame() override;
    void on_update_transform(RETransform* transform) override;
    void on_update_camera_controller(RopewayPlayerCameraController* controller) override;
    void on_draw_ui() override;
    void on_device_reset() override;

    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    auto get_hmd() const {
        return m_hmd;
    }

    auto& get_poses() const {
        if (m_use_predicted_poses) {
            return m_game_poses;
        }

        return m_render_poses;
    }

    auto get_hmd_width() const {
        return m_w;
    }

    auto get_hmd_height() const {
        return m_h;
    }

    int32_t get_frame_count() const;

    bool is_using_afr() const {
        return m_use_afr;
    }

    Vector4f get_current_offset();

    float get_current_yaw_offset();

    Matrix4x4f get_current_rotation_offset();
    Matrix4x4f get_current_projection_matrix(bool flip = false);

    auto get_focus_distance() const {
        return m_focus_distance;
    }

    auto& get_controllers() const {
        return m_controllers;
    }

    Vector4f get_position(uint32_t index);
    Matrix4x4f get_rotation(uint32_t index);

    auto& get_camera_mutex() {
        return m_camera_mtx;
    }

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

    auto get_ui_offset() const { return m_ui_offset; }
    auto get_ui_scale() const { return m_ui_scale; }

private:
    // Hooks
    static float* get_size_hook(float* result, void* ctx, REManagedObject* scene_view);
    static void inputsystem_update_hook(void* ctx, REManagedObject* input_system);
    static Matrix4x4f* camera_get_projection_matrix_hook(REManagedObject* camera, Matrix4x4f* result);

    // initialization functions
    std::optional<std::string> initialize_openvr();
    std::optional<std::string> hijack_resolution();
    std::optional<std::string> hijack_input();
    std::optional<std::string> hijack_camera();

    // input functions
    // Purpose: "Emulate" OpenVR input to the game
    // By setting things like input flags based on controller state
    void openvr_input_to_game(REManagedObject* input_system);
    
    std::recursive_mutex m_camera_mtx{};
    std::shared_mutex m_pose_mtx{};
    std::shared_mutex m_eyes_mtx{};

    REManagedObject* m_main_view{nullptr};

    vr::VRTextureBounds_t m_right_bounds{ 0.0f, 0.0f, 1.0f, 1.0f };
    vr::VRTextureBounds_t m_left_bounds{ 0.0f, 0.0f, 1.0f, 1.0f };

    float m_eye_distance{ -0.025f };
    float m_eye_rotation{ 0.135f };
    float m_focus_distance{ 8192.0f };
    float m_nearz{ 0.1f };
    float m_farz{ 3000.0f };

    vr::IVRSystem* m_hmd{nullptr};
    std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> m_real_render_poses;
    std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> m_real_game_poses;

    std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> m_render_poses;
    std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> m_game_poses;
    std::vector<int32_t> m_controllers{};

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
    
    uint32_t m_w{0}, m_h{0};

    vrmod::D3D11Component m_d3d11{};
    vrmod::D3D12Component m_d3d12{};

    float m_ui_offset{};
    float m_ui_scale{1.0f};

    int m_frame_count{};
    int m_last_frame_count{-1};
    bool m_use_afr{true};
    bool m_use_predicted_poses{false};
    bool m_submitted{false};

    static std::string actions_json;
    static std::string binding_rift_json;
    static std::string bindings_oculus_touch_json;

    friend class vrmod::D3D11Component;
    friend class vrmod::D3D12Component;
};