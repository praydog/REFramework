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
    // Increment frame count.
    ++m_frame_count;

    if (m_left_eye_tex == nullptr) {
        setup_d3d11();
    }

    auto& hook = g_framework->get_d3d11_hook();
    
    // get device
    auto device = hook->get_device();

    // Get the context.
    ID3D11DeviceContext* context{};
    device->GetImmediateContext(&context);

    // get swapchain
    auto swapchain = hook->get_swap_chain();

    // get back buffer
    ID3D11Texture2D* backbuffer = nullptr;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);

    if (backbuffer == nullptr) {
        spdlog::error("[VR] Failed to get back buffer.");
        return;
    }

    if (m_use_afr) {
        // If m_frame_count is even, we're rendering the left eye.
        if (m_frame_count % 2 == 0) {
            // Copy the back buffer to the left eye texture (m_left_eye_tex0 holds the intermediate frame).
            context->CopyResource(m_left_eye_tex0.Get(), backbuffer);
        } else {
            // Copy the back buffer to the right eye texture.
            context->CopyResource(m_right_eye_tex.Get(), backbuffer);

            // Copy the intermediate left eye texture to the actual left eye texture.
            context->CopyResource(m_left_eye_tex.Get(), m_left_eye_tex0.Get());
        }

        // Submit the eye textures to the compositor at this point. It must be done every frame for both eyes otherwise
        // FPS will dive off the deep end.
        auto compositor = vr::VRCompositor();

        vr::Texture_t left_eye{(void*)m_left_eye_tex.Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto};
        vr::Texture_t right_eye{(void*)m_right_eye_tex.Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto};

        auto e = compositor->Submit(vr::Eye_Left, &left_eye, &m_left_bounds);

        if (e != vr::VRCompositorError_None) {
            spdlog::error("[VR] VRCompositor failed to submit left eye: {}", (int)e);
        }

        e = compositor->Submit(vr::Eye_Right, &right_eye, &m_right_bounds);

        if (e != vr::VRCompositorError_None) {
            spdlog::error("[VR] VRCompositor failed to submit right eye: {}", (int)e);
        }
    } else {
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

    // Release the context.
    context->Release();
}

void VR::setup_d3d11() {
    // Get device and swapchain.
    auto& hook = g_framework->get_d3d11_hook();
    auto device = hook->get_device();
    auto swapchain = hook->get_swap_chain();

    // Get back buffer.
    ID3D11Texture2D* backbuffer{};
    swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));

    // Get backbuffer description.
    D3D11_TEXTURE2D_DESC backbuffer_desc{};
    backbuffer->GetDesc(&backbuffer_desc);

    // Create eye textures.
    device->CreateTexture2D(&backbuffer_desc, nullptr, &m_left_eye_tex0);
    device->CreateTexture2D(&backbuffer_desc, nullptr, &m_left_eye_tex);
    device->CreateTexture2D(&backbuffer_desc, nullptr, &m_right_eye_tex);

    spdlog::info("[VR] d3d11 textures have been setup");
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

    if (m_use_afr) {
        if (m_frame_count % 2 == 0) {
            // adjust the camera position using left eye offset
            controller->worldPosition += m_left_offset;
        } else {
            controller->worldPosition += m_right_offset;
        }
    }
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
    ImGui::DragFloat4("Right Bounds", (float*)&m_right_bounds, 0.01f, 0.0f, 5.0f);
    ImGui::DragFloat4("Left Bounds", (float*)&m_left_bounds, 0.01f, 0.0f, 5.0f);
    ImGui::DragFloat3("Right Offset", (float*)&m_left_offset, 0.01f, 0.0f, 5.0f);
    ImGui::DragFloat3("Left Offset", (float*)&m_right_offset, 0.01f, 0.0f, 5.0f);
    ImGui::Checkbox("Use AFR", &m_use_afr);
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