#include "sdk/Math.hpp"
#include "sdk/SceneManager.hpp"
#include "sdk/RETypeDB.hpp"

#include "utility/Scan.hpp"
#include "utility/FunctionHook.hpp"

#include "VR.hpp"

std::shared_ptr<VR>& VR::get() {
    static std::shared_ptr<VR> inst{};

    if (inst == nullptr) {
        inst = std::make_shared<VR>();
    }

    return inst;
}

std::unique_ptr<FunctionHook> g_get_size_hook{};

// Purpose: spoof the render target size to the size of the HMD displays
float* get_size_hook(float* result, void* ctx, REManagedObject* scene_view) {
    auto original_func = g_get_size_hook->get_original<decltype(get_size_hook)>();

    auto out = original_func(result, ctx, scene_view);

    // spoof the size to the HMD's size
    auto mod = VR::get();

    out[0] = (float)mod->get_hmd_width();
    out[1] = (float)mod->get_hmd_height();

    return out;
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

    // all OK
    return Mod::on_initialize();
}

void VR::on_post_frame() {
    auto scene = sdk::get_current_scene();

    if (scene == nullptr) {
        return;
    }

    m_frame_count = sdk::call_object_func<int32_t>(scene, "get_FrameCount", sdk::get_thread_context(), scene);
    m_main_view = sdk::get_main_view();

    if (m_main_view != nullptr) {
        Vector2f size{ (float)m_w, (float)m_h };
        //sdk::call_object_func<void*>(m_main_view, "set_Size", sdk::get_thread_context(), m_main_view, &size);
    }

    const auto renderer = g_framework->get_renderer_type();

    if (renderer == REFramework::RendererType::D3D11) {
        on_frame_d3d11();
    } else if (renderer == REFramework::RendererType::D3D12) {
        on_frame_d3d12();
    }

    vr::VRCompositor()->WaitGetPoses(m_poses, vr::k_unMaxTrackedDeviceCount, NULL, 0);

    m_last_frame_count = m_frame_count;
}

void VR::on_frame_d3d11() {
    // Increment frame count.
    //++m_frame_count;

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
    device->CreateTexture2D(&backbuffer_desc, nullptr, &m_d3d11.left_eye_tex0);
    device->CreateTexture2D(&backbuffer_desc, nullptr, &m_d3d11.left_eye_tex);
    device->CreateTexture2D(&backbuffer_desc, nullptr, &m_d3d11.right_eye_tex);

    spdlog::info("[VR] d3d11 textures have been setup");
}

void VR::on_update_transform(RETransform* transform) {
    
}

void VR::on_update_camera_controller(RopewayPlayerCameraController* controller) {
    // get headset rotation
    const auto& headset_pose = m_poses[0];

    if (!headset_pose.bPoseIsValid) {
        return;
    }

    auto headset_matrix = Matrix4x4f{ *(Matrix3x4f*)&headset_pose.mDeviceToAbsoluteTracking };
    auto headset_rotation = glm::extractMatrixRotation(glm::rowMajor4(headset_matrix));

    headset_rotation *= get_current_rotation_offset();

    *(glm::quat*)&controller->worldRotation = glm::quat{ headset_rotation  };

    controller->worldPosition += get_current_offset();

    auto scene_view = sdk::get_main_view();
    auto camera = sdk::get_primary_camera();

    if (camera != nullptr) {
        // Fixes warping effect in the vertical part of the camera when looking up and down
        sdk::call_object_func<void*>((::REManagedObject*)camera, "set_VerticalEnable", sdk::get_thread_context(), camera, true);
    }
}

void VR::on_frame_d3d12() {
    // Increment frame count.
    ++m_frame_count;

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

    // Release the back buffer.
    backbuffer->Release();
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
}

void VR::on_config_load(const utility::Config& cfg) {
    
}

void VR::on_config_save(utility::Config& cfg) {
    
}

Matrix4x4f VR::get_rotation(uint32_t index) {
    if (index >= vr::k_unMaxTrackedDeviceCount) {
        return glm::identity<Matrix4x4f>();
    }

    auto& pose = m_poses[index];
    auto matrix = Matrix4x4f{ *(Matrix3x4f*)&pose.mDeviceToAbsoluteTracking };
    return glm::extractMatrixRotation(glm::rowMajor4(matrix));
}
