#include <openvr.h>

#include "../VR.hpp"

#include "D3D12Component.hpp"

namespace vrmod {
vr::EVRCompositorError D3D12Component::on_frame(VR* vr) {
    wait_for_texture_copy(INFINITE);

    if (m_left_eye_tex == nullptr || m_force_reset) {
        setup();
    }

    // for some reason this happens.
    // causes all sorts of weird bugs
    // but it usually happens after removing the headset
    if (vr->m_frame_count == vr->m_last_frame_count) {
        return vr::VRCompositorError_None;
    }

    auto& hook = g_framework->get_d3d12_hook();
    
    // get device
    auto device = hook->get_device();

    // get command queue
    auto command_queue = hook->get_command_queue();

    // get swapchain
    auto swapchain = hook->get_swap_chain();

    // get back buffer
    ComPtr<ID3D12Resource> backbuffer{};

    swapchain->GetBuffer(swapchain->GetCurrentBackBufferIndex(), IID_PPV_ARGS(&backbuffer));

    if (backbuffer == nullptr) {
        spdlog::error("[VR] Failed to get back buffer.");
        return vr::VRCompositorError_None;;
    }

    // If m_frame_count is even, we're rendering the left eye.
    if (vr->m_frame_count % 2 == vr->m_left_eye_interval) {
        // Copy the back buffer to the left eye texture (m_left_eye_tex0 holds the intermediate frame).
        copy_texture(backbuffer.Get(), m_left_eye_tex.Get());
    } else {
        // Copy the back buffer to the right eye texture.
        copy_texture(backbuffer.Get(), m_right_eye_tex.Get());
    }

    vr::EVRCompositorError e = vr::EVRCompositorError::VRCompositorError_None;

    if (vr->m_frame_count % 2 == vr->m_right_eye_interval) {
        // Wait for GPU to finish copying the textures.
        m_cmd_list->Close();
        command_queue->ExecuteCommandLists(1, (ID3D12CommandList* const*)m_cmd_list.GetAddressOf());
        command_queue->Signal(m_fence.Get(), ++m_fence_value);
        m_fence->SetEventOnCompletion(m_fence_value, m_fence_event);
        m_waiting_for_fence = true;
        // we don't wait for the fence here because it will cause cause bad perf and in turn reprojection to occur

        if (vr->m_needs_wgp_update) {
            vr->m_submitted = false;
            spdlog::info("[VR] Needed WGP update inside present (frame {})", vr->m_frame_count);
            return vr::VRCompositorError_None;
        }

        // Submit the eye textures to the compositor at this point. It must be done every frame for both eyes otherwise
        // FPS will dive off the deep end.
        auto compositor = vr::VRCompositor();

        vr::D3D12TextureData_t left {
            m_left_eye_tex.Get(),
            command_queue,
            0
        };

        vr::D3D12TextureData_t right {
            m_right_eye_tex.Get(),
            command_queue,
            0
        };

        vr::Texture_t left_eye{(void*)&left, vr::TextureType_DirectX12, vr::ColorSpace_Auto};
        vr::Texture_t right_eye{(void*)&right, vr::TextureType_DirectX12, vr::ColorSpace_Auto};

        e = compositor->Submit(vr::Eye_Left, &left_eye, &vr->m_left_bounds);

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

    return e;
}

void D3D12Component::on_reset(VR* vr) {
    wait_for_texture_copy(2000);

    m_cmd_allocator.Reset();
    m_cmd_list.Reset();
    m_fence.Reset();
    m_fence_value = 0;
    CloseHandle(m_fence_event);
    m_fence_event = 0;
    m_waiting_for_fence = false;
    m_left_eye_tex.Reset();
    m_right_eye_tex.Reset();
}

void D3D12Component::wait_for_texture_copy(uint32_t ms) {
	if (m_fence_event && m_waiting_for_fence) {
        WaitForSingleObject(m_fence_event, ms);
        ResetEvent(m_fence_event);
        m_waiting_for_fence = false;
        m_cmd_allocator->Reset();
        m_cmd_list->Reset(m_cmd_allocator.Get(), nullptr);
    }
}

void D3D12Component::setup() {
    spdlog::info("[VR] Setting up d3d12 textures...");

    auto& hook = g_framework->get_d3d12_hook();

    auto device = hook->get_device();
    auto swapchain = hook->get_swap_chain();

    if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmd_allocator)))) {
        spdlog::error("[VR] Failed to create command allocator.");
        return;
    }

    if (FAILED(device->CreateCommandList(
            0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmd_allocator.Get(), nullptr, IID_PPV_ARGS(&m_cmd_list)))) {
        spdlog::error("[VR] Failed to create command list.");
        return;
    }

    if (FAILED(device->CreateFence(m_fence_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)))) {
        spdlog::error("[VR] Failed to create fence.");
        return;
    }

    m_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    ComPtr<ID3D12Resource> backbuffer{};

    if (FAILED(swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer)))) {
        spdlog::error("[VR] Failed to get back buffer.");
        return;
    }

    auto backbuffer_desc = backbuffer->GetDesc();

    spdlog::info("[VR] D3D12 Backbuffer width: {}, height: {}", backbuffer_desc.Width, backbuffer_desc.Height);

    D3D12_HEAP_PROPERTIES heap_props{};
    heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
    heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &backbuffer_desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr,
            IID_PPV_ARGS(&m_left_eye_tex)))) {
        spdlog::error("[VR] Failed to create left eye texture.");
        return;
    }

    if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &backbuffer_desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr,
            IID_PPV_ARGS(&m_right_eye_tex)))) {
        spdlog::error("[VR] Failed to create right eye texture.");
        return;
    }

    m_backbuffer_size[0] = backbuffer_desc.Width;
    m_backbuffer_size[1] = backbuffer_desc.Height;

    spdlog::info("[VR] d3d12 textures have been setup");
    m_force_reset = false;
}

void D3D12Component::copy_texture(ID3D12Resource* src, ID3D12Resource* dst) {
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
    dst_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    dst_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

    {
        D3D12_RESOURCE_BARRIER barriers[2]{src_barrier, dst_barrier};
        m_cmd_list->ResourceBarrier(2, barriers);
    }

    // Copy the resource.
    m_cmd_list->CopyResource(dst, src);

    // Switch back to present.
    src_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
    src_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    dst_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    dst_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    {
        D3D12_RESOURCE_BARRIER barriers[2]{src_barrier, dst_barrier};
        m_cmd_list->ResourceBarrier(2, barriers);
    }
}

void D3D12Component::OpenXR::initialize() {
	auto& hook = g_framework->get_d3d12_hook();

    auto device = hook->get_device();
    auto command_queue = hook->get_command_queue();

    this->binding.device = device;
    this->binding.queue = command_queue;

    PFN_xrGetD3D12GraphicsRequirementsKHR fn = nullptr;
    xrGetInstanceProcAddr(VR::get()->m_openxr.instance, "xrGetD3D12GraphicsRequirementsKHR", (PFN_xrVoidFunction*)(&fn));

    XrGraphicsRequirementsD3D12KHR gr{XR_TYPE_GRAPHICS_REQUIREMENTS_D3D12_KHR};
    fn(VR::get()->m_openxr.instance, VR::get()->m_openxr.system, &gr);
}

std::optional<std::string> D3D12Component::OpenXR::create_swapchains() {
    spdlog::info("[VR] Creating OpenXR swapchains for D3D12");
    
    auto& hook = g_framework->get_d3d12_hook();
    auto device = hook->get_device();
    auto swapchain = hook->get_swap_chain();

    ComPtr<ID3D12Resource> backbuffer{};

    // Get the existing backbuffer
    // so we can get the format and stuff.
    if (FAILED(swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer)))) {
        spdlog::error("[VR] Failed to get back buffer.");
        return "Failed to get back buffer.";
    }

    D3D12_HEAP_PROPERTIES heap_props{};
    heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
    heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    auto backbuffer_desc = backbuffer->GetDesc();
    auto vr = VR::get();
    auto& openxr = vr->m_openxr;

    this->contexts.clear();
    this->contexts.resize(openxr.views.size());
    
    for (auto i = 0; i < openxr.views.size(); ++i) {
        const auto& vp = openxr.view_configs[i];

        spdlog::info("[VR] Creating swapchain for eye {}", i);
        spdlog::info("[VR] Width: {}", vp.recommendedImageRectWidth);
        spdlog::info("[VR] Height: {}", vp.recommendedImageRectHeight);

        // Create the swapchain.
        XrSwapchainCreateInfo swapchain_create_info{XR_TYPE_SWAPCHAIN_CREATE_INFO};
        swapchain_create_info.arraySize = 1;
        swapchain_create_info.format = backbuffer_desc.Format;
        swapchain_create_info.width = vp.recommendedImageRectWidth;
        swapchain_create_info.height = vp.recommendedImageRectHeight;
        swapchain_create_info.mipCount = 1;
        swapchain_create_info.faceCount = 1;
        swapchain_create_info.sampleCount = backbuffer_desc.SampleDesc.Count;
        swapchain_create_info.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;

        VR::OpenXR::Swapchain swapchain{};
        swapchain.width = swapchain_create_info.width;
        swapchain.height = swapchain_create_info.height;

        if (xrCreateSwapchain(openxr.session, &swapchain_create_info, &swapchain.handle) != XR_SUCCESS) {
            spdlog::error("[VR] D3D12: Failed to create swapchain.");
            return "Failed to create swapchain.";
        }

        vr->m_openxr.swapchains.push_back(swapchain);

        uint32_t image_count{};
        auto result = xrEnumerateSwapchainImages(swapchain.handle, 0, &image_count, nullptr);

        if (result != XR_SUCCESS) {
            spdlog::error("[VR] Failed to enumerate swapchain images.");
            return "Failed to enumerate swapchain images.";
        }

        auto& ctx = this->contexts[i];

        ctx.textures.clear();
        ctx.textures.resize(image_count);

        for (uint32_t j = 0; j < image_count; ++j) {
            spdlog::info("[VR] Creating swapchain image {} for swapchain {}", j, i);

            ctx.textures[j] = {XR_TYPE_SWAPCHAIN_IMAGE_D3D12_KHR};

            if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &backbuffer_desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr, IID_PPV_ARGS(&ctx.textures[j].texture)))) {
                spdlog::error("[VR] Failed to create swapchain texture {} {}", i, j);
                return "Failed to create swapchain texture.";
            }
        }

        result = xrEnumerateSwapchainImages(swapchain.handle, image_count, &image_count, (XrSwapchainImageBaseHeader*)&ctx.textures[0]);

        if (result != XR_SUCCESS) {
            spdlog::error("[VR] Failed to enumerate swapchain images after texture creation.");
            return "Failed to enumerate swapchain images after texture creation.";
        }
    }

    return std::nullopt;
}
} // namespace vrmod