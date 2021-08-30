#pragma once

#include <memory>
#include <shared_mutex>
#include <openvr/headers/openvr.h>

#include <d3d11.h>
#include <d3d12.h>
#include <dxgi.h>
#include <wrl.h>

#include "sdk/Math.hpp"

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

    auto get_current_offset() const {
        if (!m_use_afr) {
            return Vector4f{};
        }

        if (get_frame_count() % 2 == 0) {
            return Vector4f{m_eye_distance * -1.0f, 0.0f, 0.0f, 0.0f};
        }
        
        return Vector4f{m_eye_distance, 0.0f, 0.0f, 0.0f};
    }

    auto get_current_yaw_offset() const {
        if (!m_use_afr) {
            return 0.0f;
        }

        if (get_frame_count() % 2 == 0) {
            return m_eye_rotation * -1;
        }
        
        return m_eye_rotation;
    }

    auto get_current_rotation_offset() const {
        if (!m_use_afr) {
            return glm::identity<Matrix4x4f>();
        }

        return Matrix4x4f{ glm::quat{ Vector3f { 0.0f, get_current_yaw_offset(), 0.0f } } };
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
    
    bool is_action_active(vr::VRActionHandle_t action, vr::VRInputValueHandle_t source = vr::k_ulInvalidInputValueHandle) const;
    Vector2f get_joystick_axis(vr::VRInputValueHandle_t handle) const;

    Vector2f get_left_stick_axis() const;
    Vector2f get_right_stick_axis() const;

private:
    // Hooks
    static float* get_size_hook(float* result, void* ctx, REManagedObject* scene_view);
    static void inputsystem_update_hook(void* ctx, REManagedObject* input_system);

    // initialization functions
    std::optional<std::string> initialize_openvr();
    std::optional<std::string> hijack_resolution();
    std::optional<std::string> hijack_input();

    // rendering functions
    void on_frame_d3d11();
    void on_frame_d3d12();

    // input functions
    // Purpose: "Emulate" OpenVR input to the game
    // By setting things like input flags based on controller state
    void openvr_input_to_game();
    
    std::recursive_mutex m_camera_mtx{};
    std::shared_mutex m_pose_mtx{};

    REManagedObject* m_main_view{nullptr};

    vr::VRTextureBounds_t m_right_bounds{ 0.0f, 0.0f, 1.0f, 1.0f };
    vr::VRTextureBounds_t m_left_bounds{ 0.0f, 0.0f, 1.0f, 1.0f };

    float m_eye_distance{ -0.025f };
    float m_eye_rotation{ 0.135f };

    vr::IVRSystem* m_hmd{nullptr};
    vr::TrackedDevicePose_t m_render_poses[vr::k_unMaxTrackedDeviceCount];
    vr::TrackedDevicePose_t m_game_poses[vr::k_unMaxTrackedDeviceCount];
    std::vector<int32_t> m_controllers{};

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
    
    uint32_t m_w{0}, m_h{0};

    template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    struct D3D11Resources {
        ComPtr<ID3D11Texture2D> left_eye_tex0{}; // Holds an intermediate frame for the left eye.
        ComPtr<ID3D11Texture2D> left_eye_tex{};
        ComPtr<ID3D11Texture2D> right_eye_tex{};
    } m_d3d11{};

    struct D3D12Resources {
        ComPtr<ID3D12CommandAllocator> cmd_allocator{};
        ComPtr<ID3D12GraphicsCommandList> cmd_list{};
        ComPtr<ID3D12Fence> fence{};
        UINT64 fence_value{};
        HANDLE fence_event{};

        ComPtr<ID3D12Resource> left_eye_tex0{}; // Holds an intermediate frame for the left eye.
        ComPtr<ID3D12Resource> left_eye_tex{};
        ComPtr<ID3D12Resource> right_eye_tex{}; 
    } m_d3d12{};

    int m_frame_count{};
    int m_last_frame_count{-1};
    bool m_use_afr{true};
    bool m_use_predicted_poses{false};
    bool m_submitted{false};

    void setup_d3d11();
    void setup_d3d12();

    static std::string actions_json;
    static std::string binding_rift_json;
    static std::string bindings_oculus_touch_json;
};