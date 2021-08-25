#include "sdk/Math.hpp"

#include "VR.hpp"

std::shared_ptr<VR>& VR::get() {
    static std::shared_ptr<VR> inst{};

    if (inst == nullptr) {
        inst = std::make_shared<VR>();
    }

    return inst;
}

// Called when the mod is initialized
std::optional<std::string> VR::on_initialize() {
    auto error = vr::VRInitError_None;
	m_hmd = vr::VR_Init(&error, vr::VRApplication_Scene);

    // check if error
    if (error != vr::VRInitError_None) {
        return "VR_Init failed: " + std::string{vr::VR_GetVRInitErrorAsEnglishDescription(error)};
    }

    // set m_hmd to module context vrsystem
    m_hmd = vr::OpenVRInternal_ModuleContext().VRSystem();

    // check if error
    if (!m_hmd || error != vr::VRInitError_None) {
        return "VR_Init failed: " + std::string{vr::VR_GetVRInitErrorAsEnglishDescription(error)};
    }

    // get render target size
    m_hmd->GetRecommendedRenderTargetSize(&m_w, &m_h);

    if (!vr::VRCompositor()) {
        return "VRCompositor failed to initialize.";
    }

    // all OK
    return Mod::on_initialize();
}

void VR::on_post_frame() {
    const auto renderer = g_framework->get_renderer_type();

    if (renderer == REFramework::RendererType::D3D11) {
        on_frame_d3d11();
    } else if (renderer == REFramework::RendererType::D3D12) {
        on_frame_d3d12();
    }

    vr::VRCompositor()->WaitGetPoses(m_poses, vr::k_unMaxTrackedDeviceCount, NULL, 0);
}

void VR::on_frame_d3d11() {
    auto& hook = g_framework->get_d3d11_hook();
    
    // get device
    auto device = hook->get_device();

    // get swapchain
    auto swapchain = hook->get_swap_chain();

    // get back buffer
    ID3D11Texture2D* backbuffer = nullptr;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);

    if (backbuffer == nullptr) {
        spdlog::error("[VR] Failed to get back buffer.");
        return;
    }

    auto compositor = vr::VRCompositor();

    vr::Texture_t texture{(void*)backbuffer, vr::TextureType_DirectX, vr::ColorSpace_Auto};

    auto e = compositor->Submit(vr::Eye_Left, &texture, &m_left_bounds);

    if (e != vr::VRCompositorError_None) {
        spdlog::error("[VR] VRCompositor failed to submit left eye: {}", (int)e);
    }

    e = compositor->Submit(vr::Eye_Right, &texture, &m_right_bounds);

    if (e != vr::VRCompositorError_None) {
        spdlog::error("[VR] VRCompositor failed to submit right eye: {}", (int)e);
    }
}

void VR::on_update_camera_controller(RopewayPlayerCameraController* controller) {
    // get headset rotation
    const auto& headset_pose = m_poses[0];

    if (!headset_pose.bPoseIsValid) {
        return;
    }

    auto headset_matrix = Matrix4x4f{ *(Matrix3x4f*)&headset_pose.mDeviceToAbsoluteTracking };
    auto headset_rotation = glm::extractMatrixRotation(glm::rowMajor4(headset_matrix));

    *(glm::quat*)&controller->worldRotation = glm::quat{ headset_rotation  };
}

void VR::on_frame_d3d12() {
    auto& hook = g_framework->get_d3d12_hook();
}

void VR::on_draw_ui() {
    // create VR tree entry in menu (imgui)
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);

    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    // draw VR tree entry in menu (imgui)
    ImGui::Text("VR");
    ImGui::Separator();
    ImGui::Text("Recommended render target size: %d x %d", m_w, m_h);
    ImGui::Separator();
    ImGui::DragFloat4("Right", (float*)&m_right_bounds, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat4("Left", (float*)&m_left_bounds, 0.01f, 0.0f, 1.0f);
}

void VR::on_config_load(const utility::Config& cfg) {
    
}

void VR::on_config_save(utility::Config& cfg) {
    
}

Matrix4x4f VR::get_rotation(uint32_t index) {
    if (index >= vr::k_unMaxTrackedDeviceCount) {
        return Matrix4x4f{};
    }

    auto& pose = m_poses[index];
    auto matrix = Matrix4x4f{ *(Matrix3x4f*)&pose.mDeviceToAbsoluteTracking };
    return glm::extractMatrixRotation(glm::rowMajor4(matrix));
}