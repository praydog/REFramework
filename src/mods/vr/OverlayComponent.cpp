#include "../VR.hpp"

#include "OverlayComponent.hpp"

namespace vrmod {
void OverlayComponent::on_reset() {
    m_overlay_data = {};
}

std::optional<std::string> OverlayComponent::on_initialize_openvr() {
    m_overlay_data = {};

    // create vr overlay
    auto overlay_error = vr::VROverlay()->CreateOverlay("REFramework", "REFramework", &m_overlay_handle);

    if (overlay_error != vr::VROverlayError_None) {
        return "VROverlay failed to create overlay: " + std::string{vr::VROverlay()->GetOverlayErrorNameFromEnum(overlay_error)};
    }

    // set overlay to visible
    vr::VROverlay()->ShowOverlay(m_overlay_handle);

    // set overlay to high quality
    overlay_error = vr::VROverlay()->SetOverlayWidthInMeters(m_overlay_handle, 0.25f);

    if (overlay_error != vr::VROverlayError_None) {
        return "VROverlay failed to set overlay width: " + std::string{vr::VROverlay()->GetOverlayErrorNameFromEnum(overlay_error)};
    }

    overlay_error = vr::VROverlay()->SetOverlayInputMethod(m_overlay_handle, vr::VROverlayInputMethod_Mouse);

    if (overlay_error != vr::VROverlayError_None) {
        return "VROverlay failed to set overlay input method: " + std::string{vr::VROverlay()->GetOverlayErrorNameFromEnum(overlay_error)};
    }

    // same thing as above but absolute instead
    // get absolute tracking pose of hmd with GetDeviceToAbsoluteTrackingPose
    // then get the matrix from that
    // then set it as the overlay transform
    vr::TrackedDevicePose_t pose{};
    vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0f, &pose, 1);
    vr::VROverlay()->SetOverlayTransformAbsolute(m_overlay_handle, vr::TrackingUniverseStanding, &pose.mDeviceToAbsoluteTracking);

    // set overlay flag to receive discrete scroll events
    overlay_error = vr::VROverlay()->SetOverlayFlag(m_overlay_handle, vr::VROverlayFlags::VROverlayFlags_SendVRDiscreteScrollEvents, true);

    if (overlay_error != vr::VROverlayError_None) {
        return "VROverlay failed to set overlay flag: " + std::string{vr::VROverlay()->GetOverlayErrorNameFromEnum(overlay_error)};
    }

    spdlog::info("Made overlay with handle {}", m_overlay_handle);

    return std::nullopt;
}

void OverlayComponent::on_pre_imgui_frame() {
    this->update_input();
}

void OverlayComponent::on_post_compositor_submit() {
    this->update_overlay();
}

void OverlayComponent::update_input() {
    auto vr = VR::get();
    auto& io = ImGui::GetIO();
    const auto is_initial_frame = vr->get_game_frame_count() % 2 == vr->m_left_eye_interval || vr->m_use_afr;

    if (!is_initial_frame) {
        // Restore the previous frame's input state
        memcpy(io.KeysDown, m_initial_imgui_input_state.KeysDown, sizeof(io.KeysDown));
        memcpy(io.MouseDown, m_initial_imgui_input_state.MouseDown, sizeof(io.MouseDown));
        io.MousePos = m_initial_imgui_input_state.MousePos;
        io.MouseWheel = m_initial_imgui_input_state.MouseWheel;
        io.MouseWheelH = m_initial_imgui_input_state.MouseWheelH;
        io.KeyCtrl = m_initial_imgui_input_state.KeyCtrl;
        io.KeyShift = m_initial_imgui_input_state.KeyShift;
        io.KeyAlt = m_initial_imgui_input_state.KeyAlt;
        io.KeySuper = m_initial_imgui_input_state.KeySuper;

        return;
    }

    const auto last_window_pos = g_framework->get_last_window_pos();
    const auto last_window_size = g_framework->get_last_window_size();
    const auto rendertarget_width = g_framework->get_renderer_type() == REFramework::RendererType::D3D11 ? g_framework->get_rendertarget_width_d3d11() : 0;
    const auto rendertarget_height = g_framework->get_renderer_type() == REFramework::RendererType::D3D11 ? g_framework->get_rendertarget_height_d3d11() : 0;

    // Poll overlay events
    vr::VREvent_t event{};
    const auto hwnd = g_framework->get_window();

    while (vr::VROverlay()->PollNextOverlayEvent(m_overlay_handle, &event, sizeof(event))) {
        switch (event.eventType) {
            case vr::VREvent_MouseButtonDown:
                SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                io.MouseDown[0] = true;
                break;
            case vr::VREvent_MouseButtonUp:
                SendMessage(hwnd, WM_LBUTTONUP, 0, 0);
                io.MouseDown[0] = false;
                break;
            case vr::VREvent_MouseMove: {
                const std::array<float, 2> raw_coords { event.data.mouse.x, event.data.mouse.y };

                // Convert from GL space (bottom left is 0,0) to window space (top left is 0,0)
                const auto mouse_point = ImVec2{
                    raw_coords[0],
                    (rendertarget_height - raw_coords[1])
                };

                // make lparam
                const auto lparam = MAKELPARAM((int32_t)mouse_point[0], (int32_t)mouse_point[1]);
                SendMessage(hwnd, WM_MOUSEMOVE, 0, lparam);

                // override imgui mouse position
                io.MousePos = mouse_point;
                //SetCursorPos((int32_t)mouse_point[0], (int32_t)mouse_point[1]);
            } break;
            case vr::VREvent_ScrollDiscrete: {
                // WM_MOUSEWHEEL
                const auto wparam = MAKEWPARAM(0, event.data.scroll.ydelta);
                SendMessage(hwnd, WM_MOUSEWHEEL, wparam, 0);
            } break;
            default:
                break;
        }
    }

    // Store the current frame's input state
    memcpy(m_initial_imgui_input_state.KeysDown, io.KeysDown, sizeof(io.KeysDown));
    memcpy(m_initial_imgui_input_state.MouseDown, io.MouseDown, sizeof(io.MouseDown));
    m_initial_imgui_input_state.MousePos = io.MousePos;
    m_initial_imgui_input_state.MouseWheel = io.MouseWheel;
    m_initial_imgui_input_state.MouseWheelH = io.MouseWheelH;
    m_initial_imgui_input_state.KeyCtrl = io.KeyCtrl;
    m_initial_imgui_input_state.KeyShift = io.KeyShift;
    m_initial_imgui_input_state.KeyAlt = io.KeyAlt;
    m_initial_imgui_input_state.KeySuper = io.KeySuper;
}

void OverlayComponent::update_overlay() {
    auto vr = VR::get();

    const auto is_d3d11 = g_framework->get_renderer_type() == REFramework::RendererType::D3D11;

    // update REFramework menu overlay
    const auto last_window_pos = g_framework->get_last_window_pos();
    const auto last_window_size = g_framework->get_last_window_size();
    const auto render_target_width = is_d3d11 ? g_framework->get_rendertarget_width_d3d11() : g_framework->get_rendertarget_width_d3d12();
    const auto render_target_height = is_d3d11 ? g_framework->get_rendertarget_height_d3d11() : g_framework->get_rendertarget_height_d3d12();

    // only update certain parts of the overlay
    // if things like the width or position of the window change
    if (m_overlay_data.last_x != last_window_pos.x || m_overlay_data.last_y != last_window_pos.y ||
        m_overlay_data.last_width != last_window_size.x || m_overlay_data.last_height != last_window_size.y ||
        m_overlay_data.last_render_target_width != render_target_width || m_overlay_data.last_render_target_height != render_target_height) 
    {
        vr::VRTextureBounds_t bounds{};
        bounds.uMin = last_window_pos.x / render_target_width;
        bounds.uMax = (last_window_pos.x + last_window_size.x) / render_target_width;
        bounds.vMin = last_window_pos.y / render_target_height;
        bounds.vMax = (last_window_pos.y + last_window_size.y) / render_target_height;

        vr::VROverlay()->SetOverlayTextureBounds(m_overlay_handle, &bounds);

        // necessary, fixes all sorts of issues with ray intersection
        const auto mouse_scale = vr::HmdVector2_t{(float)render_target_width, (float)render_target_height};
        vr::VROverlay()->SetOverlayMouseScale(m_overlay_handle, &mouse_scale);

        vr::VROverlayIntersectionMaskPrimitive_t intersection_mask{};

        intersection_mask.m_nPrimitiveType = vr::EVROverlayIntersectionMaskPrimitiveType::OverlayIntersectionPrimitiveType_Rectangle;
        intersection_mask.m_Primitive.m_Rectangle.m_flTopLeftX = last_window_pos.x;
        intersection_mask.m_Primitive.m_Rectangle.m_flTopLeftY = last_window_pos.y;
        intersection_mask.m_Primitive.m_Rectangle.m_flWidth = last_window_size.x;
        intersection_mask.m_Primitive.m_Rectangle.m_flHeight = last_window_size.y;

        vr::VROverlay()->SetOverlayIntersectionMask(m_overlay_handle, &intersection_mask, 1);

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
            
            vr::VROverlay()->SetOverlayTransformAbsolute(m_overlay_handle, vr::ETrackingUniverseOrigin::TrackingUniverseStanding, (vr::HmdMatrix34_t*)&steamvr_transform);
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
            if (vr::VROverlay()->ComputeOverlayIntersection(m_overlay_handle, &intersection_params, &intersection_results)) {
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

            if (vr::VROverlay()->ComputeOverlayIntersection(m_overlay_handle, &intersection_params, &intersection_results)) {
                // Make sure the intersection hit the front of the overlay, not the back
                any_intersected = intersection_results.vNormal.v[2] > 0.0f;
            } else {
                any_intersected = false;
            }
        }

        // set overlay flag
        if (any_intersected) {
            should_show_overlay = true;
            vr::VROverlay()->SetOverlayFlag(m_overlay_handle, vr::VROverlayFlags::VROverlayFlags_MakeOverlaysInteractiveIfVisible, true);

            g_framework->set_draw_ui(true);
            m_just_closed_ui = false;
        } else {
            should_show_overlay = false;
            vr::VROverlay()->SetOverlayFlag(m_overlay_handle, vr::VROverlayFlags::VROverlayFlags_MakeOverlaysInteractiveIfVisible, false);

            if (!m_just_closed_ui) {
                g_framework->set_draw_ui(false);
                m_just_closed_ui = true;
            }
        }
    }

    if (should_show_overlay) {
        // finally set the texture
        if (is_d3d11) {
            vr::Texture_t imgui_tex{(void*)g_framework->get_rendertarget_d3d11().Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto};
            vr::VROverlay()->SetOverlayTexture(m_overlay_handle, &imgui_tex);   
        } else {
            auto& hook = g_framework->get_d3d12_hook();

            vr::D3D12TextureData_t texture_data {
                g_framework->get_rendertarget_d3d12().Get(),
                hook->get_command_queue(),
                0
            };
            
            vr::Texture_t imgui_tex{(void*)&texture_data, vr::TextureType_DirectX12, vr::ColorSpace_Auto};
            vr::VROverlay()->SetOverlayTexture(m_overlay_handle, &imgui_tex);
        }
    } else {
        if (is_d3d11) {
            // draw a blank texture (don't just call HideOverlay, we'll no longer be able to use intersection tests)
            vr::Texture_t imgui_tex{(void*)g_framework->get_blank_rendertarget_d3d11().Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto};
            vr::VROverlay()->SetOverlayTexture(m_overlay_handle, &imgui_tex);
        } else {
            auto& hook = g_framework->get_d3d12_hook();

            vr::D3D12TextureData_t texture_data {
                g_framework->get_blank_rendertarget_d3d12().Get(),
                hook->get_command_queue(),
                0
            };
            
            vr::Texture_t imgui_tex{(void*)&texture_data, vr::TextureType_DirectX12, vr::ColorSpace_Auto};
            vr::VROverlay()->SetOverlayTexture(m_overlay_handle, &imgui_tex);
        }
    }
}
}