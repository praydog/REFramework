#include <fstream>

#include "sdk/Math.hpp"
#include "sdk/SceneManager.hpp"
#include "sdk/RETypeDB.hpp"

#include "utility/Scan.hpp"
#include "utility/FunctionHook.hpp"
#include "utility/Module.hpp"

#include "VR.hpp"

std::shared_ptr<VR>& VR::get() {
    static std::shared_ptr<VR> inst{};

    if (inst == nullptr) {
        inst = std::make_shared<VR>();
    }

    return inst;
}

std::unique_ptr<FunctionHook> g_get_size_hook{};
std::unique_ptr<FunctionHook> g_input_hook{};

// Purpose: spoof the render target size to the size of the HMD displays
float* VR::get_size_hook(float* result, void* ctx, REManagedObject* scene_view) {
    auto original_func = g_get_size_hook->get_original<decltype(VR::get_size_hook)>();

    auto out = original_func(result, ctx, scene_view);

    // spoof the size to the HMD's size
    auto mod = VR::get();

    out[0] = (float)mod->get_hmd_width();
    out[1] = (float)mod->get_hmd_height();

    return out;
}

void VR::inputsystem_update_hook(void* ctx, REManagedObject* input_system) {
    auto original_func = g_input_hook->get_original<decltype(VR::inputsystem_update_hook)>();
    auto lstick = sdk::call_object_func<REManagedObject*>(input_system, "get_LStick", sdk::get_thread_context());
    auto rstick = sdk::call_object_func<REManagedObject*>(input_system, "get_RStick", sdk::get_thread_context());

    if (lstick == nullptr || rstick == nullptr) {
        return;
    }

    auto set_button_state = [&](app::ropeway::InputDefine::Kind kind, bool state) {
        // static so we dont pass the object
        sdk::call_object_func<void*>(input_system, "setForce", sdk::get_thread_context(), kind, state);
    };

    auto mod = VR::get();
    auto left_axis = mod->get_left_stick_axis();
    auto right_axis = mod->get_right_stick_axis();
    const auto left_axis_len = glm::length(left_axis);
    const auto right_axis_len = glm::length(right_axis);

    if (left_axis_len > 0.01f) {
        // Override the left stick's axis values to the VR controller's values
        Vector3f axis{ left_axis.x, left_axis.y, 0.0f };
        sdk::call_object_func<void*>(lstick, "update", sdk::get_thread_context(), lstick, &axis, &axis);
    }

    if (right_axis_len > 0.01f) {
        // Override the right stick's axis values to the VR controller's values
        Vector3f axis{ right_axis.x, right_axis.y, 0.0f };
        sdk::call_object_func<void*>(rstick, "update", sdk::get_thread_context(), rstick, &axis, &axis);
    }

    set_button_state(app::ropeway::InputDefine::Kind::MOVE, left_axis_len > 0.01f);
    set_button_state(app::ropeway::InputDefine::Kind::UI_L_STICK, left_axis_len > 0.01f);

    set_button_state(app::ropeway::InputDefine::Kind::WATCH, right_axis_len > 0.01f);
    set_button_state(app::ropeway::InputDefine::Kind::UI_R_STICK, right_axis_len > 0.01f);
    //set_button_state(app::ropeway::InputDefine::Kind::RUN, right_axis_len > 0.01f);

    original_func(ctx, input_system);

    if (left_axis_len > 0.01f) {
        // Override the left stick's axis values to the VR controller's values
        Vector3f axis{ left_axis.x, left_axis.y, 0.0f };
        sdk::call_object_func<void*>(lstick, "update", sdk::get_thread_context(), lstick, &axis, &axis);
    }

    if (right_axis_len > 0.01f) {
        // Override the right stick's axis values to the VR controller's values
        Vector3f axis{ right_axis.x, right_axis.y, 0.0f };
        sdk::call_object_func<void*>(rstick, "update", sdk::get_thread_context(), rstick, &axis, &axis);
    }

    set_button_state(app::ropeway::InputDefine::Kind::MOVE, left_axis_len > 0.01f);
    set_button_state(app::ropeway::InputDefine::Kind::UI_L_STICK, left_axis_len > 0.01f);

    set_button_state(app::ropeway::InputDefine::Kind::WATCH, right_axis_len > 0.01f);
    set_button_state(app::ropeway::InputDefine::Kind::UI_R_STICK, right_axis_len > 0.01f);

    // Causes the right stick to take effect properly
    if (right_axis_len > 0.01f || left_axis_len > 0.01f) {
        sdk::call_object_func<void*>(input_system, "set_InputMode", sdk::get_thread_context(), input_system, app::ropeway::InputDefine::InputMode::Pad);
    }

    mod->openvr_input_to_game();
}

// Called when the mod is initialized
std::optional<std::string> VR::on_initialize() {
    auto openvr_error = initialize_openvr();

    if (openvr_error) {
        return openvr_error;
    }

    auto hijack_error = hijack_resolution();

    if (hijack_error) {
        return hijack_error;
    }

    hijack_error = hijack_input();

    if (hijack_error) {
        return hijack_error;

    }

    // all OK
    return Mod::on_initialize();
}

std::optional<std::string> VR::initialize_openvr() {
    auto error = vr::VRInitError_None;
	m_hmd = vr::VR_Init(&error, vr::VRApplication_Scene);

    // check if error
    if (error != vr::VRInitError_None) {
        return "VR_Init failed: " + std::string{vr::VR_GetVRInitErrorAsEnglishDescription(error)};
    }

    if (m_hmd == nullptr) {
        return "VR_Init failed: HMD is null";
    }

    // get render target size
    m_hmd->GetRecommendedRenderTargetSize(&m_w, &m_h);

    if (!vr::VRCompositor()) {
        return "VRCompositor failed to initialize.";
    }

    // go through all the device indices and get the controllers
    for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
        if (m_hmd->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_Controller) {
            m_controllers.push_back(i);

            spdlog::info("Found controller {}", i);
        }
    }

    const auto module_directory = *utility::get_module_directory(g_framework->get_module().as<HMODULE>());

    // write default actions and bindings with the static strings we have
    {
        std::ofstream actions_file{ module_directory + "/actions.json" };
        actions_file << VR::actions_json;

        std::ofstream rift_file{ module_directory + "/binding_rift.json" };
        rift_file << VR::binding_rift_json;

        std::ofstream oculus_touch_file{ module_directory + "/bindings_oculus_touch.json" };
        oculus_touch_file << VR::bindings_oculus_touch_json;
    }

    const auto actions_path = module_directory + "/actions.json";
    auto input_error = vr::VRInput()->SetActionManifestPath(actions_path.c_str());

    if (input_error != vr::VRInputError_None) {
        return "VRInput failed to set action manifest path: " + std::to_string((uint32_t)input_error);
    }

    // get action set
    auto action_set_error = vr::VRInput()->GetActionSetHandle("/actions/default", &m_action_set);

    if (action_set_error != vr::VRInputError_None) {
        return "VRInput failed to get action set: " + std::to_string((uint32_t)action_set_error);
    }

    if (m_action_set == vr::k_ulInvalidActionSetHandle) {
        return "VRInput failed to get action set handle.";
    }

    // get action handles
    auto trigger_error = vr::VRInput()->GetActionHandle("/actions/default/in/Trigger", &m_action_trigger);
    auto grip_error = vr::VRInput()->GetActionHandle("/actions/default/in/Grip", &m_action_grip);

    if (trigger_error != vr::VRInputError_None || grip_error != vr::VRInputError_None) {
        return "VRInput failed to get action handles: " + std::to_string((uint32_t)trigger_error) + " " + std::to_string((uint32_t)grip_error);
    }

    if (m_action_trigger == vr::k_ulInvalidActionHandle || m_action_grip == vr::k_ulInvalidActionHandle) {
        return "VRInput failed to get action handles.";
    }

    auto joystick_error = vr::VRInput()->GetActionHandle("/actions/default/in/Joystick", &m_action_joystick);

    if (joystick_error != vr::VRInputError_None) {
        return "VRInput failed to get action handles (Joystick): " + std::to_string((uint32_t)joystick_error);
    }

    auto joystick_click_error = vr::VRInput()->GetActionHandle("/actions/default/in/JoystickClick", &m_action_joystick_click);

    if (joystick_click_error != vr::VRInputError_None) {
        return "VRInput failed to get action handles (JoystickClick): " + std::to_string((uint32_t)joystick_click_error);
    }

    auto a_button_error = vr::VRInput()->GetActionHandle("/actions/default/in/AButton", &m_action_a_button);

    if (a_button_error != vr::VRInputError_None) {
        return "VRInput failed to get action handles (AButton): " + std::to_string((uint32_t)a_button_error);
    }

    auto b_button_error = vr::VRInput()->GetActionHandle("/actions/default/in/BButton", &m_action_b_button);

    if (b_button_error != vr::VRInputError_None) {
        return "VRInput failed to get action handles (BButton): " + std::to_string((uint32_t)b_button_error);
    }

    // get the source input device handles for the joysticks
    auto left_joystick_error = vr::VRInput()->GetInputSourceHandle("/user/hand/left", &m_left_joystick);

    if (left_joystick_error != vr::VRInputError_None) {
        return "VRInput failed to get input source handles (left): " + std::to_string((uint32_t)left_joystick_error);
    }

    auto right_joystick_error = vr::VRInput()->GetInputSourceHandle("/user/hand/right", &m_right_joystick);

    if (right_joystick_error != vr::VRInputError_None) {
        return "VRInput failed to get input source handles (right): " + std::to_string((uint32_t)right_joystick_error);
    }

    m_active_action_set.ulActionSet = m_action_set;
    m_active_action_set.ulRestrictedToDevice = vr::k_ulInvalidInputValueHandle;
    m_active_action_set.nPriority = 0;

    return std::nullopt;
}

std::optional<std::string> VR::hijack_resolution() {
    // We're going to hook via.SceneView.get_Size so we can
    // spoof the render target size to the HMD's resolution.
    auto sceneview_type = sdk::RETypeDB::get()->find_type("via.SceneView");

    if (sceneview_type == nullptr) {
        return "VR init failed: via.SceneView type not found.";
    }

    auto get_size_method = sceneview_type->get_method("get_Size");

    if (get_size_method == nullptr) {
        return "VR init failed: via.SceneView.get_Size method not found.";
    }

    auto get_size_func = get_size_method->get_function();

    if (get_size_func == nullptr) {
        return "VR init failed: via.SceneView.get_Size function not found.";
    }

    spdlog::info("via.SceneView.get_Size: {:x}", (uintptr_t)get_size_func);

    // Pattern scan for the native function call
    auto ref = utility::scan((uintptr_t)get_size_func, 0x100, "49 8B C8 E8");

    if (!ref) {
        return "VR init failed: via.SceneView.get_Size native function not found. Pattern scan failed.";
    }

    auto native_func = utility::calculate_absolute(*ref + 4);

    // Hook the native function
    g_get_size_hook = std::make_unique<FunctionHook>(native_func, get_size_hook);

    if (!g_get_size_hook->create()) {
        return "VR init failed: via.SceneView.get_Size native function hook failed.";
    }

    return std::nullopt;
}

std::optional<std::string> VR::hijack_input() {
    // We're going to hook InputSystem.update so we can
    // override the analog stick values with the VR controller's
    auto t = sdk::RETypeDB::get()->find_type(game_namespace("InputSystem"));

    if (t == nullptr) {
        return "VR init failed: InputSystem type not found.";
    }

    auto method = t->get_method("update");

    if (method == nullptr) {
        return "VR init failed: InputSystem.update method not found.";
    }

    auto func = method->get_function();

    if (func == nullptr) {
        return "VR init failed: InputSystem.update function not found.";
    }

    spdlog::info("InputSystem.update: {:x}", (uintptr_t)func);

    // Hook the native function
    g_input_hook = std::make_unique<FunctionHook>(func, inputsystem_update_hook);

    if (!g_input_hook->create()) {
        return "VR init failed: InputSystem.update native function hook failed.";
    }

    return std::nullopt;
}

int32_t VR::get_frame_count() const {
    auto scene = sdk::get_current_scene();

    if (scene == nullptr) {
        return 0;
    }

    return sdk::call_object_func<int32_t>(scene, "get_FrameCount", sdk::get_thread_context(), scene);
}

void VR::on_post_frame() {
    //std::scoped_lock _{ m_camera_mtx };

    m_frame_count = get_frame_count();
    m_main_view = sdk::get_main_view();

    const auto renderer = g_framework->get_renderer_type();

    if (renderer == REFramework::RendererType::D3D11) {
        on_frame_d3d11();
    } else if (renderer == REFramework::RendererType::D3D12) {
        on_frame_d3d12();
    }

    m_last_frame_count = m_frame_count;

    if (m_submitted) {
        std::unique_lock _{ m_pose_mtx };

        // Update input
        auto error = vr::VRInput()->UpdateActionState(&m_active_action_set, sizeof(m_active_action_set), 1);

        if (error != vr::VRInputError_None) {
            spdlog::error("VRInput failed to update action state: {}", (uint32_t)error);
        }

        vr::VRCompositor()->WaitGetPoses(m_render_poses, vr::k_unMaxTrackedDeviceCount, m_game_poses, vr::k_unMaxTrackedDeviceCount);
        m_submitted = false;
    }
}

void VR::on_frame_d3d11() {
    if (m_d3d11.left_eye_tex == nullptr) {
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
        if (m_frame_count != m_last_frame_count) {
            // If m_frame_count is even, we're rendering the left eye.
            if (m_frame_count % 2 == 0) {
                // Copy the back buffer to the left eye texture (m_left_eye_tex0 holds the intermediate frame).
                context->CopyResource(m_d3d11.left_eye_tex0.Get(), backbuffer);
            } else {
                // Copy the back buffer to the right eye texture.
                context->CopyResource(m_d3d11.right_eye_tex.Get(), backbuffer);

                // Copy the intermediate left eye texture to the actual left eye texture.
                context->CopyResource(m_d3d11.left_eye_tex.Get(), m_d3d11.left_eye_tex0.Get());
            }
        }

        if (m_frame_count != m_last_frame_count && m_frame_count % 2 == 1) {
            // Submit the eye textures to the compositor at this point. It must be done every frame for both eyes otherwise
            // FPS will dive off the deep end.
            auto compositor = vr::VRCompositor();

            vr::Texture_t left_eye{(void*)m_d3d11.left_eye_tex.Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto};
            vr::Texture_t right_eye{(void*)m_d3d11.right_eye_tex.Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto};

            auto e = compositor->Submit(vr::Eye_Left, &left_eye, &m_left_bounds);

            if (e != vr::VRCompositorError_None) {
                spdlog::error("[VR] VRCompositor failed to submit left eye: {}", (int)e);
            }

            e = compositor->Submit(vr::Eye_Right, &right_eye, &m_right_bounds);

            if (e != vr::VRCompositorError_None) {
                spdlog::error("[VR] VRCompositor failed to submit right eye: {}", (int)e);
            }

            m_submitted = true;
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

        m_submitted = true;
    }

    // Release the back buffer.
    backbuffer->Release();

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
    device->CreateTexture2D(&backbuffer_desc, nullptr, &m_d3d11.left_eye_tex0);
    device->CreateTexture2D(&backbuffer_desc, nullptr, &m_d3d11.left_eye_tex);
    device->CreateTexture2D(&backbuffer_desc, nullptr, &m_d3d11.right_eye_tex);

    // Release the back buffer.
    backbuffer->Release();

    spdlog::info("[VR] d3d11 textures have been setup");
}

void VR::on_update_transform(RETransform* transform) {
    
}

void VR::on_update_camera_controller(RopewayPlayerCameraController* controller) {
    // get headset rotation
    /*const auto& headset_pose = m_game_poses[0];

    if (!headset_pose.bPoseIsValid) {
        return;
    }

    auto headset_matrix = Matrix4x4f{ *(Matrix3x4f*)&headset_pose.mDeviceToAbsoluteTracking };
    auto headset_rotation = glm::extractMatrixRotation(glm::rowMajor4(headset_matrix));

    headset_rotation *= get_current_rotation_offset();

    *(glm::quat*)&controller->worldRotation = glm::quat{ headset_rotation  };

    controller->worldPosition += get_current_offset();*/

    auto scene_view = sdk::get_main_view();
    auto camera = sdk::get_primary_camera();

    if (camera != nullptr) {
        // Fixes warping effect in the vertical part of the camera when looking up and down
        sdk::call_object_func<void*>((::REManagedObject*)camera, "set_VerticalEnable", sdk::get_thread_context(), camera, true);
    }
}

void VR::on_frame_d3d12() {
    auto copy_texture = [this](ID3D12Resource* src, ID3D12Resource* dst) {
        // Switch src into copy source.
        D3D12_RESOURCE_BARRIER src_barrier{};

        src_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        src_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        src_barrier.Transition.pResource = src;
        src_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        src_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        src_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;

        // Switch dst into copy destination.
        D3D12_RESOURCE_BARRIER dst_barrier{};
        dst_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        dst_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        dst_barrier.Transition.pResource = dst;
        dst_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        dst_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        dst_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

        D3D12_RESOURCE_BARRIER barriers[2]{src_barrier, dst_barrier};

        m_d3d12.cmd_list->ResourceBarrier(2, barriers);

        // Copy the resource.
        m_d3d12.cmd_list->CopyResource(dst, src);

        // Switch back to present.
        src_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
        src_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        dst_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        dst_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

        m_d3d12.cmd_list->ResourceBarrier(2, barriers);
    };

    if (m_d3d12.left_eye_tex == nullptr) {
        setup_d3d12();
    }

    auto& hook = g_framework->get_d3d12_hook();
    
    // get device
    auto device = hook->get_device();

    // get command queue
    auto command_queue = hook->get_command_queue();

    // get swapchain
    auto swapchain = hook->get_swap_chain();

    // get back buffer
    ID3D12Resource* backbuffer{};
    swapchain->GetBuffer(swapchain->GetCurrentBackBufferIndex(), IID_PPV_ARGS(&backbuffer));

    if (backbuffer == nullptr) {
        spdlog::error("[VR] Failed to get back buffer.");
        return;
    }

    if (m_use_afr) {
        if (m_frame_count != m_last_frame_count) {
            // If m_frame_count is even, we're rendering the left eye.
            if (m_frame_count % 2 == 0) {
                // Copy the back buffer to the left eye texture (m_left_eye_tex0 holds the intermediate frame).
                copy_texture(backbuffer, m_d3d12.left_eye_tex0.Get());
            } else {
                // Copy the back buffer to the right eye texture.
                copy_texture(backbuffer, m_d3d12.right_eye_tex.Get());

                // Copy the intermediate left eye texture to the actual left eye texture.
                copy_texture(m_d3d12.left_eye_tex0.Get(), m_d3d12.left_eye_tex.Get());
            }

            // Wait for GPU to finish copying the textures.
            m_d3d12.cmd_list->Close();
            command_queue->ExecuteCommandLists(1, (ID3D12CommandList* const*)m_d3d12.cmd_list.GetAddressOf());
            command_queue->Signal(m_d3d12.fence.Get(), ++m_d3d12.fence_value);
            m_d3d12.fence->SetEventOnCompletion(m_d3d12.fence_value, m_d3d12.fence_event);
            WaitForSingleObject(m_d3d12.fence_event, INFINITE);
            m_d3d12.cmd_allocator->Reset();
            m_d3d12.cmd_list->Reset(m_d3d12.cmd_allocator.Get(), nullptr);
        }

        if (m_frame_count != m_last_frame_count && m_frame_count % 2 == 1) {
            // Submit the eye textures to the compositor at this point. It must be done every frame for both eyes otherwise
            // FPS will dive off the deep end.
            auto compositor = vr::VRCompositor();

            vr::Texture_t left_eye{(void*)m_d3d12.left_eye_tex.Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto};
            vr::Texture_t right_eye{(void*)m_d3d12.right_eye_tex.Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto};

            auto e = compositor->Submit(vr::Eye_Left, &left_eye, &m_left_bounds);

            if (e != vr::VRCompositorError_None) {
                spdlog::error("[VR] VRCompositor failed to submit left eye: {}", (int)e);
            }

            e = compositor->Submit(vr::Eye_Right, &right_eye, &m_right_bounds);

            if (e != vr::VRCompositorError_None) {
                spdlog::error("[VR] VRCompositor failed to submit right eye: {}", (int)e);
            }

            m_submitted = true;
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

        m_submitted = true;
    }

    // Release the back buffer.
    backbuffer->Release();
}

void VR::openvr_input_to_game() {
    // RE2/RE3 only (i think?)
    auto input_system = g_framework->get_globals()->get(game_namespace("InputSystem"));

    if (input_system == nullptr) {
        spdlog::error("[VR] Failed to get the game's input system.");
        return;
    }

    // Get OpenVR input system
    auto openvr_input = vr::VRInput();

    if (openvr_input == nullptr) {
        spdlog::error("[VR] Failed to get OpenVR input system.");
        return;
    }

    const auto is_grip_down = is_action_active(m_action_grip, m_right_joystick);
    const auto is_trigger_down = is_action_active(m_action_trigger, m_right_joystick);
    const auto is_left_joystick_click_down = is_action_active(m_action_joystick_click, m_left_joystick);
    const auto is_right_joystick_click_down = is_action_active(m_action_joystick_click, m_right_joystick);

    const auto is_left_a_button_down = is_action_active(m_action_a_button, m_left_joystick);
    const auto is_left_b_button_down = is_action_active(m_action_b_button, m_left_joystick);
    const auto is_right_a_button_down = is_action_active(m_action_a_button, m_right_joystick);
    const auto is_right_b_button_down = is_action_active(m_action_b_button, m_right_joystick);

    auto set_button_state = [&](app::ropeway::InputDefine::Kind kind, bool state) {
        // static so we dont pass the object
        sdk::call_object_func<void*>(input_system, "setForce", sdk::get_thread_context(), kind, state);
    };

    // Aim
    set_button_state(app::ropeway::InputDefine::Kind::HOLD, is_grip_down);

    // Attack
    set_button_state(app::ropeway::InputDefine::Kind::ATTACK, is_trigger_down);

    // Sprint
    set_button_state(app::ropeway::InputDefine::Kind::JOG1, is_left_joystick_click_down);

    // Reset camera
    set_button_state(app::ropeway::InputDefine::Kind::RESET_CAMERA, is_right_joystick_click_down);

    // Inventory, PRESS_START
    set_button_state(app::ropeway::InputDefine::Kind::INVENTORY, is_left_b_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::PRESS_START, is_left_b_button_down);

    // QUICK_TURN, PRESS_START, CANCEL, DIALOG_CANCEL
    set_button_state(app::ropeway::InputDefine::Kind::QUICK_TURN, is_left_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::PRESS_START, is_left_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::CANCEL, is_left_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::DIALOG_CANCEL, is_left_a_button_down);
    
    // Action, ITEM, PRESS_START, DECIDE, DIALOG_DECIDE, (1 << 51)
    set_button_state(app::ropeway::InputDefine::Kind::ACTION, is_right_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::ITEM, is_right_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::PRESS_START, is_right_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::DECIDE, is_right_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::DIALOG_DECIDE, is_right_a_button_down);
    set_button_state((app::ropeway::InputDefine::Kind)((uint64_t)1 << 51), is_right_a_button_down);
    
    // Reload, Skip Event, UI_EXCHANGE, UI_RESET, (1 << 52) (that one is RE3 only? don't see it in the enum)
    set_button_state(app::ropeway::InputDefine::Kind::RELOAD, is_right_b_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::SKIP_EVENT, is_right_b_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::UI_EXCHANGE, is_right_b_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::UI_RESET, is_right_b_button_down);
    set_button_state((app::ropeway::InputDefine::Kind)((uint64_t)1 << 52), is_right_b_button_down);
}

void VR::setup_d3d12() {
    spdlog::info("[VR] Setting up d3d12 textures...");

    auto& hook = g_framework->get_d3d12_hook();

    auto device = hook->get_device();
    auto swapchain = hook->get_swap_chain();

    if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_d3d12.cmd_allocator)))) {
        spdlog::error("[VR] Failed to create command allocator.");
    }

    if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_d3d12.cmd_allocator.Get(), nullptr, IID_PPV_ARGS(&m_d3d12.cmd_list)))) {
        spdlog::error("[VR] Failed to create command list.");
    }

    if (FAILED(device->CreateFence(m_d3d12.fence_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_d3d12.fence)))) {
        spdlog::error("[VR] Failed to create fence.");
    }

    m_d3d12.fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    ID3D12Resource* backbuffer{};

    if (FAILED(swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer)))) {
        spdlog::error("[VR] Failed to get back buffer.");
    }

    auto backbuffer_desc = backbuffer->GetDesc();

    D3D12_HEAP_PROPERTIES heap_props{};
    heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
    heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    // Create the textures.
    if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &backbuffer_desc, D3D12_RESOURCE_STATE_PRESENT, nullptr, IID_PPV_ARGS(&m_d3d12.left_eye_tex0)))) {
        spdlog::error("[VR] Failed to create left eye texture.");
    }

    if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &backbuffer_desc, D3D12_RESOURCE_STATE_PRESENT, nullptr, IID_PPV_ARGS(&m_d3d12.left_eye_tex)))) {
        spdlog::error("[VR] Failed to create left eye texture.");
    }

    if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &backbuffer_desc, D3D12_RESOURCE_STATE_PRESENT, nullptr, IID_PPV_ARGS(&m_d3d12.right_eye_tex)))) {
        spdlog::error("[VR] Failed to create right eye texture.");
    }

    backbuffer->Release();

    spdlog::info("[VR] d3d12 textures have been setup");
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
    ImGui::DragFloat4("Right Bounds", (float*)&m_right_bounds, 0.005f, 0.0f, 5.0f);
    ImGui::DragFloat4("Left Bounds", (float*)&m_left_bounds, 0.005f, 0.0f, 5.0f);
    ImGui::DragFloat("Eye Distance", (float*)&m_eye_distance, 0.005f, -5.0f, 5.0f);
    ImGui::DragFloat("Eye Rotation", (float*)&m_eye_rotation, 0.005f, -5.0f, 5.0f);
    ImGui::Checkbox("Use AFR", &m_use_afr);
    ImGui::Checkbox("Use Predicted Poses", &m_use_predicted_poses);
}

void VR::on_device_reset() {
    m_d3d11 = D3D11Resources{};
    m_d3d12 = D3D12Resources{};
}

void VR::on_config_load(const utility::Config& cfg) {
    
}

void VR::on_config_save(utility::Config& cfg) {
    
}

Vector4f VR::get_position(uint32_t index) {
    if (index >= vr::k_unMaxTrackedDeviceCount) {
        return Vector4f{};
    }

    std::shared_lock _{ m_pose_mtx };

    auto& pose = get_poses()[index];
    auto matrix = Matrix4x4f{ *(Matrix3x4f*)&pose.mDeviceToAbsoluteTracking };
    auto result = glm::rowMajor4(matrix)[3];
    result.w = 1.0f;

    return result;
}

Matrix4x4f VR::get_rotation(uint32_t index) {
    if (index >= vr::k_unMaxTrackedDeviceCount) {
        return glm::identity<Matrix4x4f>();
    }

    std::shared_lock _{ m_pose_mtx };

    auto& pose = get_poses()[index];
    auto matrix = Matrix4x4f{ *(Matrix3x4f*)&pose.mDeviceToAbsoluteTracking };
    return glm::extractMatrixRotation(glm::rowMajor4(matrix));
}

bool VR::is_action_active(vr::VRActionHandle_t action, vr::VRInputValueHandle_t source) const {
    vr::InputDigitalActionData_t data{};
	vr::VRInput()->GetDigitalActionData(action, &data, sizeof(data), source);

    return data.bActive && data.bState;
}

Vector2f VR::get_joystick_axis(vr::VRInputValueHandle_t handle) const {
    vr::InputAnalogActionData_t data{};
    vr::VRInput()->GetAnalogActionData(m_action_joystick, &data, sizeof(data), handle);

    return Vector2f{ data.x, data.y };
}

Vector2f VR::get_left_stick_axis() const {
    return get_joystick_axis(m_left_joystick);
}

Vector2f VR::get_right_stick_axis() const {
    return get_joystick_axis(m_right_joystick);
}

std::string VR::actions_json = R"(
{
  "actions": [
    {
      "name": "/actions/default/in/HeadsetOnHead",
      "type": "boolean"
    },
    {
      "name": "/actions/default/in/SkeletonLeftHand",
      "type": "skeleton",
      "skeleton": "/skeleton/hand/left"
    },
    {
      "name": "/actions/default/in/SkeletonRightHand",
      "type": "skeleton",
      "skeleton": "/skeleton/hand/right"
    },
    {
      "name": "/actions/default/in/Pose",
      "type": "pose"
    },
    {
      "name": "/actions/default/in/Trigger",
      "type": "boolean"
    },
    {
      "name": "/actions/default/in/Grip",
      "type": "boolean"
    },
    {
      "name": "/actions/default/in/Touchpad",
      "type": "vector2"
    },
    {
      "name": "/actions/default/in/TouchpadClick",
      "type": "boolean"
    },
    {
      "name": "/actions/default/in/Joystick",
      "type": "vector2"
    },
    {
      "name": "/actions/default/in/JoystickClick",
      "type": "boolean"
    },
    {
      "name": "/actions/default/in/AButton",
      "type": "boolean"
    },
    {
      "name": "/actions/default/in/BButton",
      "type": "boolean"
    },
    {
      "name": "/actions/default/in/SystemButton",
      "type": "boolean"
    },
    {
      "name": "/actions/default/in/Squeeze",
      "type": "vector1"
    },
    {
      "name": "/actions/default/in/Teleport",
      "type": "boolean"
    },
    {
      "name": "/actions/default/out/Haptic",
      "type": "vibration"
    }
  ],
  "action_sets": [
    {
      "name": "/actions/default",
      "usage": "single"
    }
  ],
  "default_bindings": [
    {
      "controller_type": "oculus_touch",
      "binding_url": "bindings_oculus_touch.json"
    },
    {
      "controller_type": "rift",
      "binding_url": "binding_rift.json"
    }
  ],
  "localization": []
})";

std::string VR::binding_rift_json = R"(
{
   "alias_info" : {},
   "bindings" : {
      "/actions/default" : {
         "chords" : [],
         "haptics" : [],
         "poses" : [],
         "skeleton" : [],
         "sources" : [
            {
               "inputs" : {
                  "click" : {
                     "output" : "/actions/default/in/headsetonhead"
                  }
               },
               "mode" : "button",
               "path" : "/user/head/proximity"
            }
         ]
      }
   },
   "controller_type" : "rift",
   "description" : "",
   "name" : "rift defaults",
   "options" : {},
   "simulated_actions" : []
})";

std::string VR::bindings_oculus_touch_json = R"(
{
   "alias_info" : {},
   
   "bindings" : {
      "/actions/buggy" : {
         "chords" : [],
         "haptics" : [],
         "poses" : [],
         "skeleton" : [],
         "sources" : []
      },
      "/actions/default" : {
         "chords" : [],
         "haptics" : [
            {
               "output" : "/actions/default/out/haptic",
               "path" : "/user/hand/left/output/haptic"
            },
            {
               "output" : "/actions/default/out/haptic",
               "path" : "/user/hand/right/output/haptic"
            }
         ],
         "poses" : [
            {
               "output" : "/actions/default/in/pose",
               "path" : "/user/hand/left/pose/raw"
            },
            {
               "output" : "/actions/default/in/pose",
               "path" : "/user/hand/right/pose/raw"
            }
         ],
         "skeleton" : [
            {
               "output" : "/actions/default/in/skeletonrighthand",
               "path" : "/user/hand/left/input/skeleton/right"
            },
            {
               "output" : "/actions/default/in/skeletonrighthand",
               "path" : "/user/hand/right/input/skeleton/right"
            },
            {
               "output" : "/actions/default/in/skeletonlefthand",
               "path" : "/user/hand/left/input/skeleton/left"
            }
         ],
         "sources" : [
            {
               "inputs" : {
                  "pull" : {
                     "output" : "/actions/default/in/squeeze"
                  }
               },
               "mode" : "trigger",
               "path" : "/user/hand/left/input/grip"
            },
            {
               "inputs" : {
                  "pull" : {
                     "output" : "/actions/default/in/squeeze"
                  }
               },
               "mode" : "trigger",
               "path" : "/user/hand/right/input/grip"
            },
            {
               "inputs" : {
                  "click" : {
                     "output" : "/actions/default/in/trigger"
                  }
               },
               "mode" : "button",
               "path" : "/user/hand/left/input/trigger"
            },
            {
               "inputs" : {
                  "click" : {
                     "output" : "/actions/default/in/trigger"
                  }
               },
               "mode" : "button",
               "path" : "/user/hand/right/input/trigger"
            },
            {
               "inputs" : {
                  "click" : {
                     "output" : "/actions/default/in/grip"
                  }
               },
               "mode" : "button",
               "path" : "/user/hand/left/input/grip"
            },
            {
               "inputs" : {
                  "click" : {
                     "output" : "/actions/default/in/grip"
                  }
               },
               "mode" : "button",
               "path" : "/user/hand/right/input/grip"
            },
            {
               "inputs" : {
                  "click" : {
                     "output" : "/actions/default/in/abutton"
                  }
               },
               "mode" : "button",
               "path" : "/user/hand/left/input/x"
            },
            {
               "inputs" : {
                  "click" : {
                     "output" : "/actions/default/in/abutton"
                  }
               },
               "mode" : "button",
               "path" : "/user/hand/right/input/x"
            },
            {
               "inputs" : {
                  "click" : {
                     "output" : "/actions/default/in/bbutton"
                  }
               },
               "mode" : "button",
               "path" : "/user/hand/left/input/y"
            },
            {
               "inputs" : {
                  "click" : {
                     "output" : "/actions/default/in/bbutton"
                  }
               },
               "mode" : "button",
               "path" : "/user/hand/right/input/y"
            },
            {
               "inputs" : {
                  "click" : {
                     "output" : "/actions/default/in/abutton"
                  }
               },
               "mode" : "button",
               "path" : "/user/hand/right/input/a"
            },
            {
               "inputs" : {
                  "click" : {
                     "output" : "/actions/default/in/joystickclick"
                  },
                  "position" : {
                     "output" : "/actions/default/in/joystick"
                  }
               },
               "mode" : "joystick",
               "path" : "/user/hand/left/input/joystick"
            },
            {
               "inputs" : {
                  "click" : {
                     "output" : "/actions/default/in/joystickclick"
                  },
                  "position" : {
                     "output" : "/actions/default/in/joystick"
                  }
               },
               "mode" : "joystick",
               "path" : "/user/hand/right/input/joystick"
            },
            {
               "inputs" : {
                  "click" : {
                     "output" : "/actions/default/in/bbutton"
                  }
               },
               "mode" : "button",
               "path" : "/user/hand/right/input/b"
            },
            {
               "inputs" : {
                  "click" : {
                     "output" : "/actions/default/in/systembutton"
                  }
               },
               "mode" : "button",
               "path" : "/user/hand/left/input/system"
            }
         ]
      },
      "/actions/platformer" : {
         "chords" : [],
         "haptics" : [],
         "poses" : [],
         "skeleton" : [],
         "sources" : []
      }
   },
   "controller_type" : "oculus_touch",
   "description" : "",
   "name" : "oculus_touch",
   "options" : {},
   "simulated_actions" : []
})";
