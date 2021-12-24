#include <imgui.h>
#include <imgui_internal.h>
#include <openvr.h>

#include "../VR.hpp"

#include "D3D11Component.hpp"

namespace vrmod {
void D3D11Component::on_frame(VR* vr) {
    if (m_left_eye_tex == nullptr) {
        setup();
    }

    if (vr->m_frame_count == vr->m_last_frame_count) {
        return;
    }

    auto& hook = g_framework->get_d3d11_hook();

    // get device
    auto device = hook->get_device();

    // Get the context.
    ComPtr<ID3D11DeviceContext> context{};

    device->GetImmediateContext(&context);

    // get swapchain
    auto swapchain = hook->get_swap_chain();

    // get back buffer
    ComPtr<ID3D11Texture2D> backbuffer{};

    swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));

    if (backbuffer == nullptr) {
        spdlog::error("[VR] Failed to get back buffer.");
        return;
    }

    // If m_frame_count is even, we're rendering the left eye.
    if (vr->m_frame_count % 2 == vr->m_left_eye_interval) {
        // Copy the back buffer to the left eye texture (m_left_eye_tex0 holds the intermediate frame).
        context->CopyResource(m_left_eye_tex.Get(), backbuffer.Get());

        if (vr->m_depth_aided_reprojection) {
            context->CopyResource(m_left_eye_depthstencil.Get(), hook->get_last_depthstencil_used().Get());
            m_left_eye_proj = vr->m_hmd->GetProjectionMatrix(vr::Eye_Left, vr->m_nearz, vr->m_farz);
        }
    } else {
        // Copy the back buffer to the right eye texture.
        context->CopyResource(m_right_eye_tex.Get(), backbuffer.Get());

        if (vr->m_depth_aided_reprojection) {
            context->CopyResource(m_right_eye_depthstencil.Get(), hook->get_last_depthstencil_used().Get());
            m_right_eye_proj = vr->m_hmd->GetProjectionMatrix(vr::Eye_Right, vr->m_nearz, vr->m_farz);
        }
    }

    if (vr->m_frame_count % 2 == vr->m_right_eye_interval) {
        // Submit the eye textures to the compositor at this point. It must be done every frame for both eyes otherwise
        // FPS will dive off the deep end.
        auto compositor = vr::VRCompositor();

        if (vr->m_depth_aided_reprojection) {
            auto view_mat = Matrix4x4f{};

            vr::VRTextureWithPoseAndDepth_t left_eye{
                (void*)m_left_eye_tex.Get(),
                vr::TextureType_DirectX,
                vr::ColorSpace_Auto,
                *(vr::HmdMatrix34_t*)&view_mat,
                (void*)m_left_eye_depthstencil.Get(),
                m_left_eye_proj,
                {0.0f, 1.0f},
            };
            vr::VRTextureWithPoseAndDepth_t right_eye{
                (void*)m_right_eye_tex.Get(),
                vr::TextureType_DirectX,
                vr::ColorSpace_Auto,
                *(vr::HmdMatrix34_t*)&view_mat,
                (void*)m_right_eye_depthstencil.Get(),
                m_right_eye_proj,
                {0.0f, 1.0f},
            };

            const auto flags = vr::Submit_TextureWithDepth | vr::Submit_TextureWithPose;
            auto e = compositor->Submit(vr::Eye_Left, &left_eye, &vr->m_left_bounds, (vr::EVRSubmitFlags)flags);

            bool submitted = true;

            if (e != vr::VRCompositorError_None) {
                spdlog::error("[VR] VRCompositor failed to submit left eye: {}", (int)e);
                submitted = false;
            }

            e = compositor->Submit(vr::Eye_Right, &right_eye, &vr->m_right_bounds, (vr::EVRSubmitFlags)flags);

            if (e != vr::VRCompositorError_None) {
                spdlog::error("[VR] VRCompositor failed to submit right eye: {}", (int)e);
                submitted = false;
            }

            vr->m_submitted = submitted;
        } else {
            vr::Texture_t left_eye{(void*)m_left_eye_tex.Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto};
            vr::Texture_t right_eye{(void*)m_right_eye_tex.Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto};

            auto e = compositor->Submit(vr::Eye_Left, &left_eye, &vr->m_left_bounds);

            bool submitted = true;

            if (e != vr::VRCompositorError_None) {
                spdlog::error("[VR] VRCompositor failed to submit left eye: {}", (int)e);
                submitted = false;
            }

            e = compositor->Submit(vr::Eye_Right, &right_eye, &vr->m_right_bounds);

            if (e != vr::VRCompositorError_None) {
                spdlog::error("[VR] VRCompositor failed to submit right eye: {}", (int)e);
                submitted = false;
            }

            vr->m_submitted = submitted;
        }
    }
}

void D3D11Component::on_reset(VR* vr) {
    m_left_eye_tex.Reset();
    m_right_eye_tex.Reset();
    m_left_eye_depthstencil.Reset();
    m_right_eye_depthstencil.Reset();
}

void D3D11Component::setup() {
    // Get device and swapchain.
    auto& hook = g_framework->get_d3d11_hook();
    auto device = hook->get_device();
    auto swapchain = hook->get_swap_chain();

    // Get back buffer.
    ComPtr<ID3D11Texture2D> backbuffer{};

    swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));

    // Get backbuffer description.
    D3D11_TEXTURE2D_DESC backbuffer_desc{};

    backbuffer->GetDesc(&backbuffer_desc);

    backbuffer_desc.BindFlags |= D3D11_BIND_RENDER_TARGET;

    // Create eye textures.
    device->CreateTexture2D(&backbuffer_desc, nullptr, &m_left_eye_tex);
    device->CreateTexture2D(&backbuffer_desc, nullptr, &m_right_eye_tex);

    // copy backbuffer into right eye
    // Get the context.
    ComPtr<ID3D11DeviceContext> context{};

    device->GetImmediateContext(&context);
    context->CopyResource(m_right_eye_tex.Get(), backbuffer.Get());

    // Make depth stencils for both eyes.
    auto depthstencil = hook->get_last_depthstencil_used();
    D3D11_TEXTURE2D_DESC depthstencil_desc{};

    depthstencil->GetDesc(&depthstencil_desc);

    // Create eye depthstencils.
    device->CreateTexture2D(&depthstencil_desc, nullptr, &m_left_eye_depthstencil);
    device->CreateTexture2D(&depthstencil_desc, nullptr, &m_right_eye_depthstencil);

    // Copy the current depthstencil into the right eye.
    context->CopyResource(m_right_eye_depthstencil.Get(), depthstencil.Get());

    spdlog::info("[VR] d3d11 textures have been setup");
}
} // namespace vrmod
