#include <openvr/headers/openvr.h>

#include "../VR.hpp"

#include "D3D11Component.hpp"

namespace vrmod {
void D3D11Component::on_frame(VR* vr) {
    if (m_left_eye_tex == nullptr) {
        setup();
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
    if (vr->m_frame_count % 2 == 0) {
        // Copy the back buffer to the left eye texture (m_left_eye_tex0 holds the intermediate frame).
        context->CopyResource(m_left_eye_tex0.Get(), backbuffer.Get());
    } else {
        // Copy the back buffer to the right eye texture.
        context->CopyResource(m_right_eye_tex.Get(), backbuffer.Get());

        // Copy the intermediate left eye texture to the actual left eye texture.
        context->CopyResource(m_left_eye_tex.Get(), m_left_eye_tex0.Get());
    }

    if (vr->m_frame_count % 2 == 1) {
        // Submit the eye textures to the compositor at this point. It must be done every frame for both eyes otherwise
        // FPS will dive off the deep end.
        auto compositor = vr::VRCompositor();

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

void D3D11Component::on_reset(VR* vr) {
    m_left_eye_tex0.Reset();
    m_left_eye_tex.Reset();
    m_right_eye_tex.Reset();
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
    device->CreateTexture2D(&backbuffer_desc, nullptr, &m_left_eye_tex0);
    device->CreateTexture2D(&backbuffer_desc, nullptr, &m_left_eye_tex);
    device->CreateTexture2D(&backbuffer_desc, nullptr, &m_right_eye_tex);

    // copy backbuffer into right eye
    // Get the context.
    ComPtr<ID3D11DeviceContext> context{};

    device->GetImmediateContext(&context);
    context->CopyResource(m_right_eye_tex.Get(), backbuffer.Get());

    spdlog::info("[VR] d3d11 textures have been setup");
}
} // namespace vr

