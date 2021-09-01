#include <openvr/headers/openvr.h>

#include "../VR.hpp"

#include "D3D12Component.hpp"

namespace vrmod {
void D3D12Component::on_frame(VR* vr) {
    if (m_left_eye_tex == nullptr) {
        setup();
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

    if (vr->m_use_afr) {
        if (vr->m_frame_count != vr->m_last_frame_count) {
            // If m_frame_count is even, we're rendering the left eye.
            if (vr->m_frame_count % 2 == 0) {
                // Copy the back buffer to the left eye texture (m_left_eye_tex0 holds the intermediate frame).
                copy_texture(backbuffer, m_left_eye_tex0.Get());
            } else {
                // Copy the back buffer to the right eye texture.
                copy_texture(backbuffer, m_right_eye_tex.Get());

                // Copy the intermediate left eye texture to the actual left eye texture.
                copy_texture(m_left_eye_tex0.Get(), m_left_eye_tex.Get());
            }

            // Wait for GPU to finish copying the textures.
            m_cmd_list->Close();
            command_queue->ExecuteCommandLists(1, (ID3D12CommandList* const*)m_cmd_list.GetAddressOf());
            command_queue->Signal(m_fence.Get(), ++m_fence_value);
            m_fence->SetEventOnCompletion(m_fence_value, m_fence_event);
            WaitForSingleObject(m_fence_event, INFINITE);
            m_cmd_allocator->Reset();
            m_cmd_list->Reset(m_cmd_allocator.Get(), nullptr);
        }

        if (vr->m_frame_count != vr->m_last_frame_count && vr->m_frame_count % 2 == 1) {
            // Submit the eye textures to the compositor at this point. It must be done every frame for both eyes otherwise
            // FPS will dive off the deep end.
            auto compositor = vr::VRCompositor();

            vr::Texture_t left_eye{(void*)m_left_eye_tex.Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto};
            vr::Texture_t right_eye{(void*)m_right_eye_tex.Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto};

            auto e = compositor->Submit(vr::Eye_Left, &left_eye, &vr->m_left_bounds);

            if (e != vr::VRCompositorError_None) {
                spdlog::error("[VR] VRCompositor failed to submit left eye: {}", (int)e);
            }

            e = compositor->Submit(vr::Eye_Right, &right_eye, &vr->m_right_bounds);

            if (e != vr::VRCompositorError_None) {
                spdlog::error("[VR] VRCompositor failed to submit right eye: {}", (int)e);
            }

            vr->m_submitted = true;
        }
    } else {
        auto compositor = vr::VRCompositor();

        vr::Texture_t texture{(void*)backbuffer, vr::TextureType_DirectX, vr::ColorSpace_Auto};

        auto e = compositor->Submit(vr::Eye_Left, &texture, &vr->m_left_bounds);

        if (e != vr::VRCompositorError_None) {
            spdlog::error("[VR] VRCompositor failed to submit left eye: {}", (int)e);
        }

        e = compositor->Submit(vr::Eye_Right, &texture, &vr->m_right_bounds);

        if (e != vr::VRCompositorError_None) {
            spdlog::error("[VR] VRCompositor failed to submit right eye: {}", (int)e);
        }

        vr->m_submitted = true;
    }

    // Release the back buffer.
    backbuffer->Release();
}

void D3D12Component::on_reset(VR* vr) {
    m_cmd_allocator.Reset();
    m_cmd_list.Reset();
    m_fence.Reset();
    m_fence_value = 0;
    CloseHandle(m_fence_event);
    m_left_eye_tex0.Reset();
    m_left_eye_tex.Reset();
    m_right_eye_tex.Reset();
}

void D3D12Component::setup() {
    spdlog::info("[VR] Setting up d3d12 textures...");

    auto& hook = g_framework->get_d3d12_hook();

    auto device = hook->get_device();
    auto swapchain = hook->get_swap_chain();

    if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmd_allocator)))) {
        spdlog::error("[VR] Failed to create command allocator.");
    }

    if (FAILED(device->CreateCommandList(
            0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmd_allocator.Get(), nullptr, IID_PPV_ARGS(&m_cmd_list)))) {
        spdlog::error("[VR] Failed to create command list.");
    }

    if (FAILED(device->CreateFence(m_fence_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)))) {
        spdlog::error("[VR] Failed to create fence.");
    }

    m_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);

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
    if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &backbuffer_desc, D3D12_RESOURCE_STATE_PRESENT, nullptr,
            IID_PPV_ARGS(&m_left_eye_tex0)))) {
        spdlog::error("[VR] Failed to create left eye texture.");
    }

    if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &backbuffer_desc, D3D12_RESOURCE_STATE_PRESENT, nullptr,
            IID_PPV_ARGS(&m_left_eye_tex)))) {
        spdlog::error("[VR] Failed to create left eye texture.");
    }

    if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &backbuffer_desc, D3D12_RESOURCE_STATE_PRESENT, nullptr,
            IID_PPV_ARGS(&m_right_eye_tex)))) {
        spdlog::error("[VR] Failed to create right eye texture.");
    }

    backbuffer->Release();

    spdlog::info("[VR] d3d12 textures have been setup");
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
    dst_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    dst_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

    D3D12_RESOURCE_BARRIER barriers[2]{src_barrier, dst_barrier};

    m_cmd_list->ResourceBarrier(2, barriers);

    // Copy the resource.
    m_cmd_list->CopyResource(dst, src);

    // Switch back to present.
    src_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
    src_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    dst_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    dst_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

    m_cmd_list->ResourceBarrier(2, barriers);
}
} // namespace vrmod