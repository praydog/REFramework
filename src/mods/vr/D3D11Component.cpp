#include <openvr.h>
#include <imgui.h>
#include <imgui_internal.h>

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
    } else {
        // Copy the back buffer to the right eye texture.
        context->CopyResource(m_right_eye_tex.Get(), backbuffer.Get());
    }

    if (vr->m_frame_count % 2 == vr->m_right_eye_interval) {
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

        this->update_overlay();
    }
}

void D3D11Component::on_reset(VR* vr) {
    m_left_eye_tex.Reset();
    m_right_eye_tex.Reset();

    m_overlay_data = {};
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

    m_overlay_data = {};

    spdlog::info("[VR] d3d11 textures have been setup");
}

void D3D11Component::update_overlay() {
    auto vr = VR::get();

    // update REFramework menu overlay
    const auto last_window_pos = g_framework->get_last_window_pos();
    const auto last_window_size = g_framework->get_last_window_size();
    const auto render_target_width = g_framework->get_rendertarget_width_d3d11();
    const auto render_target_height = g_framework->get_rendertarget_height_d3d11();

    if (m_overlay_data.last_x != last_window_pos.x || m_overlay_data.last_y != last_window_pos.y ||
        m_overlay_data.last_width != last_window_size.x || m_overlay_data.last_height != last_window_size.y ||
        m_overlay_data.last_render_target_width != render_target_width || m_overlay_data.last_render_target_height != render_target_height) 
    {
        vr::VRTextureBounds_t bounds{};
        bounds.uMin = last_window_pos.x / render_target_width;
        bounds.uMax = (last_window_pos.x + last_window_size.x) / render_target_width;
        bounds.vMin = last_window_pos.y / render_target_height;
        bounds.vMax = (last_window_pos.y + last_window_size.y) / render_target_height;

        vr::VROverlay()->SetOverlayTextureBounds(vr->m_overlay_handle, &bounds);

        // necessary, fixes all sorts of issues with ray intersection
        const auto mouse_scale = vr::HmdVector2_t{(float)render_target_width, (float)render_target_height};
        vr::VROverlay()->SetOverlayMouseScale(vr->m_overlay_handle, &mouse_scale);

        vr::VROverlayIntersectionMaskPrimitive_t intersection_mask{};

        intersection_mask.m_nPrimitiveType = vr::EVROverlayIntersectionMaskPrimitiveType::OverlayIntersectionPrimitiveType_Rectangle;
        intersection_mask.m_Primitive.m_Rectangle.m_flTopLeftX = last_window_pos.x;
        intersection_mask.m_Primitive.m_Rectangle.m_flTopLeftY = last_window_pos.y;
        intersection_mask.m_Primitive.m_Rectangle.m_flWidth = last_window_size.x;
        intersection_mask.m_Primitive.m_Rectangle.m_flHeight = last_window_size.y;

        vr::VROverlay()->SetOverlayIntersectionMask(vr->m_overlay_handle, &intersection_mask, 1);

        // and now set the last known values
        m_overlay_data.last_x = last_window_pos.x;
        m_overlay_data.last_y = last_window_pos.y;
        m_overlay_data.last_width = last_window_size.x;
        m_overlay_data.last_height = last_window_size.y;
        m_overlay_data.last_render_target_width = render_target_width;
        m_overlay_data.last_render_target_height = render_target_height;
    }

    // Fire an intersection test and enable the laser pointer if we're intersecting
    const auto& controllers = vr->get_controllers();

    bool should_show_overlay = true;

    if (controllers.size() >= 2) {
        // Attach the overlay to the left controller
        if (controllers[0] != vr::k_unTrackedDeviceIndexInvalid) {
            const auto position_offset = vr->m_overlay_position;
            const auto rotation_offset = vr->m_overlay_rotation;
 
            auto left_controller_world_transform = vr->get_transform(controllers[0]) * Matrix4x4f{glm::quat{rotation_offset}};
            left_controller_world_transform[3] -= glm::extractMatrixRotation(left_controller_world_transform) * position_offset;
            left_controller_world_transform[3].w = 1.0f;

            const auto steamvr_transform = Matrix3x4f{ glm::rowMajor4(left_controller_world_transform) };
            
            vr::VROverlay()->SetOverlayTransformAbsolute(vr->m_overlay_handle, vr::ETrackingUniverseOrigin::TrackingUniverseStanding, (vr::HmdMatrix34_t*)&steamvr_transform);
        }

        bool any_intersected = false;

        for (const auto& controller_index : controllers) {
            const auto is_left = controller_index == controllers[0];

            vr::VRInputValueHandle_t controller_handle{};

            if (is_left) {
                controller_handle = vr->m_left_joystick;
            } else {
                controller_handle = vr->m_right_joystick;
            }

            char render_name[vr::k_unMaxPropertyStringSize]{};
            vr::VRSystem()->GetStringTrackedDeviceProperty(controller_index, vr::Prop_RenderModelName_String, render_name, vr::k_unMaxPropertyStringSize);

            vr::RenderModel_ControllerMode_State_t controller_state{};
            vr::RenderModel_ComponentState_t component_state{};

            // get tip component state
            if (!vr::VRRenderModels()->GetComponentStateForDevicePath(render_name, vr::k_pch_Controller_Component_Tip, controller_handle, &controller_state, &component_state)) {
                continue;
            }

            // obtain tip world transform
            const auto controller_world_transform = vr->get_transform(controller_index);
            const auto tip_local_transform = glm::rowMajor4(Matrix4x4f{*(Matrix3x4f*)&component_state.mTrackingToComponentLocal});
            const auto tip_world_transform = controller_world_transform * tip_local_transform;

            // Set up intersection data
            vr::VROverlayIntersectionResults_t intersection_results{};
            vr::VROverlayIntersectionParams_t intersection_params{};
            intersection_params.eOrigin = vr::TrackingUniverseOrigin::TrackingUniverseStanding;
            intersection_params.vSource.v[0] = tip_world_transform[3][0];
            intersection_params.vSource.v[1] = tip_world_transform[3][1];
            intersection_params.vSource.v[2] = tip_world_transform[3][2];
            intersection_params.vDirection.v[0] = -tip_world_transform[2][0];
            intersection_params.vDirection.v[1] = -tip_world_transform[2][1];
            intersection_params.vDirection.v[2] = -tip_world_transform[2][2];

            // Do the intersection test
            if (vr::VROverlay()->ComputeOverlayIntersection(vr->m_overlay_handle, &intersection_params, &intersection_results)) {
                // Make sure the intersection hit the front of the overlay, not the back
                any_intersected = intersection_results.vNormal.v[2] > 0.0f;
            }
        }

        // Do the same intersection test with the user's view/head gaze
        // Both the head and controller needs to be aimed at the overlay for it to appear
        if (any_intersected) {
            auto head_world_transform = vr->get_transform(vr::k_unTrackedDeviceIndex_Hmd);
            vr::VROverlayIntersectionResults_t intersection_results{};
            vr::VROverlayIntersectionParams_t intersection_params{};
            intersection_params.eOrigin = vr::TrackingUniverseOrigin::TrackingUniverseStanding;
            intersection_params.vSource.v[0] = head_world_transform[3][0];
            intersection_params.vSource.v[1] = head_world_transform[3][1];
            intersection_params.vSource.v[2] = head_world_transform[3][2];
            intersection_params.vDirection.v[0] = -head_world_transform[2][0];
            intersection_params.vDirection.v[1] = -head_world_transform[2][1];
            intersection_params.vDirection.v[2] = -head_world_transform[2][2];

            if (vr::VROverlay()->ComputeOverlayIntersection(vr->m_overlay_handle, &intersection_params, &intersection_results)) {
                // Make sure the intersection hit the front of the overlay, not the back
                any_intersected = intersection_results.vNormal.v[2] > 0.0f;
            } else {
                any_intersected = false;
            }
        }

        // set overlay flag
        if (any_intersected) {
            should_show_overlay = true;
            vr::VROverlay()->SetOverlayFlag(vr->m_overlay_handle, vr::VROverlayFlags::VROverlayFlags_MakeOverlaysInteractiveIfVisible, true);
        } else {
            should_show_overlay = false;
            vr::VROverlay()->SetOverlayFlag(vr->m_overlay_handle, vr::VROverlayFlags::VROverlayFlags_MakeOverlaysInteractiveIfVisible, false);
        }
    }
    
    if (should_show_overlay) {
        // finally set the texture
        vr::Texture_t imgui_tex{(void*)g_framework->get_rendertarget_d3d11().Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto};
        vr::VROverlay()->SetOverlayTexture(vr->m_overlay_handle, &imgui_tex);
    } else {
        // draw a blank texture (don't just call HideOverlay, we'll no longer be able to use intersection tests)
        vr::Texture_t imgui_tex{(void*)g_framework->get_blank_rendertarget_d3d11().Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto};
        vr::VROverlay()->SetOverlayTexture(vr->m_overlay_handle, &imgui_tex);
    }
}
} // namespace vr

