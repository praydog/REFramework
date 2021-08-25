#pragma once

#include <memory>
#include <openvr/headers/openvr.h>

#include <d3d11.h>
#include <dxgi.h>
#include <wrl.h>

#include "sdk/Math.hpp"

#include "Mod.hpp"

class VR : public Mod {
public:
    static std::shared_ptr<VR>& get();

    std::string_view get_name() const override { return "VR"; }

    // Called when the mod is initialized
    std::optional<std::string> on_initialize() override;

    void on_post_frame() override;
    void on_update_camera_controller(RopewayPlayerCameraController* controller) override;
    void on_draw_ui() override;

    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    auto get_hmd() const {
        return m_hmd;
    }

    auto& get_poses() const {
        return m_poses;
    }

    Matrix4x4f get_rotation(uint32_t index);

private:
    void on_frame_d3d11();
    void on_frame_d3d12();

    vr::VRTextureBounds_t m_right_bounds{ 0.0f, 0.0f, 0.5f, 1.0f };
    vr::VRTextureBounds_t m_left_bounds{ 0.5f, 0.0f, 1.0f, 1.0f };

    vr::IVRSystem* m_hmd{nullptr};
    vr::TrackedDevicePose_t m_poses[vr::k_unMaxTrackedDeviceCount];
    
    uint32_t m_w{0}, m_h{0};

    template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    ComPtr<ID3D11Texture2D> m_left_eye_tex0{}; // Holds an intermediate frame for the left eye.
    ComPtr<ID3D11Texture2D> m_left_eye_tex{};
    ComPtr<ID3D11Texture2D> m_right_eye_tex{};

    int m_frame_count{};
    bool m_use_afr{true};

    void setup_d3d11();
};