#include <openvr.h>
#include <utility/Profiler.hpp>

#include "../VR.hpp"
#include "../TemporalUpscaler.hpp"

#include "D3D12Component.hpp"

namespace vrmod {
vr::EVRCompositorError D3D12Component::on_frame(VR* vr) {
    REF_PROFILE_FUNCTION();

    if (m_openvr.left_eye_tex[0].texture == nullptr || m_force_reset) {
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
    ComPtr<ID3D12Resource> backbuffer{};

    if (m_prev_backbuffer == nullptr) {
        m_prev_backbuffer = backbuffer;
    }

    if (FAILED(swapchain->GetBuffer(swapchain->GetCurrentBackBufferIndex(), IID_PPV_ARGS(&backbuffer)))) {
        spdlog::error("[VR] Failed to get back buffer");
        return vr::VRCompositorError_None;
    }

    if (backbuffer == nullptr) {
        spdlog::error("[VR] Failed to get back buffer.");
        return vr::VRCompositorError_None;
    }

    auto runtime = vr->get_runtime();
    const auto frame_count = vr->m_render_frame_count;
    const auto is_multipass = vr->is_using_multipass();

    if (is_multipass && (vr->m_multipass.eye_textures[0] == nullptr || vr->m_multipass.eye_textures[1] == nullptr)) {
        return vr::VRCompositorError_None;
    }

    if (is_multipass) {
        const auto eye_desc = vr->m_multipass.eye_textures[0]->GetDesc();

        if (runtime->is_openxr()) {
            if (eye_desc.Format != m_openxr.last_format) {
                spdlog::info("[VR] OpenXR format changed from {} to {}", m_openxr.last_format, eye_desc.Format);
                m_openxr.create_swapchains();
            }
        } else {
            if (eye_desc.Format != m_openvr.last_format) {
                spdlog::info("[VR] OpenVR format changed from {} to {}", m_openvr.last_format, eye_desc.Format);
                on_reset(vr);
                return vr::VRCompositorError_None;
            }
        }
    }

    // If m_frame_count is even, we're rendering the left eye.
    if (frame_count % 2 == vr->m_left_eye_interval && !is_multipass) {
        // OpenXR texture
        if (runtime->is_openxr() && vr->m_openxr->ready()) {
            m_openxr.copy(0, backbuffer.Get(), nullptr, D3D12_RESOURCE_STATE_PRESENT);
        }

        // OpenVR texture
        // Copy the back buffer to the left eye texture (m_left_eye_tex0 holds the intermediate frame).
        if (runtime->is_openvr()) {
            m_openvr.copy_left(backbuffer.Get());

            vr::D3D12TextureData_t left {
                m_openvr.get_left().texture.Get(),
                command_queue,
                0
            };
            
            vr::Texture_t left_eye{(void*)&left, vr::TextureType_DirectX12, vr::ColorSpace_Auto};

            auto e = vr::VRCompositor()->Submit(vr::Eye_Left, &left_eye, &vr->m_left_bounds);

            if (e != vr::VRCompositorError_None) {
                spdlog::error("[VR] VRCompositor failed to submit left eye: {}", (int)e);
                return e;
            }
        }
    } else {
        // OpenXR texture
        if (runtime->is_openxr() && vr->m_openxr->ready()) {
            if (is_multipass) {
                /*D3D12_BOX src_box{};
                src_box.back = 1;
                src_box.right = vr->get_hmd_width();
                src_box.bottom = vr->get_hmd_height();
                m_openxr.copy(0, (ID3D12Resource*)vr->m_multipass.eye_textures[0], &src_box, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

                src_box.left = src_box.right;
                src_box.right *= 2;
                m_openxr.copy(1, (ID3D12Resource*)vr->m_multipass.eye_textures[0], &src_box, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);*/

                if (!TemporalUpscaler::get()->ready()) {
                    m_openxr.copy(0, vr->m_multipass.eye_textures[0].Get(), nullptr, D3D12_RESOURCE_STATE_COMMON);
                    m_openxr.copy(1, vr->m_multipass.eye_textures[1].Get(), nullptr, D3D12_RESOURCE_STATE_COMMON);
                } else {
                    m_openxr.copy(0, vr->m_multipass.eye_textures[0].Get(), nullptr, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
                    m_openxr.copy(1, vr->m_multipass.eye_textures[1].Get(), nullptr, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
                }

                vr->m_multipass.eye_textures[0].Reset();
                vr->m_multipass.eye_textures[1].Reset();
            } else {
                m_openxr.copy(1, backbuffer.Get(), nullptr, D3D12_RESOURCE_STATE_PRESENT);
            }
        }

        // OpenVR texture
        // Copy the back buffer to the right eye texture.
        if (runtime->is_openvr()) {
            if (is_multipass) {
                if (!TemporalUpscaler::get()->ready()) {
                    m_openvr.copy_left(vr->m_multipass.eye_textures[0].Get());
                    m_openvr.copy_right(vr->m_multipass.eye_textures[1].Get());
                } else {
                    m_openvr.copy_left(vr->m_multipass.eye_textures[0].Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
                    m_openvr.copy_right(vr->m_multipass.eye_textures[1].Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
                }
            } else {
                m_openvr.copy_right(backbuffer.Get());
            }

            vr::D3D12TextureData_t right {
                m_openvr.get_right().texture.Get(),
                command_queue,
                0
            };

            vr::Texture_t right_eye{(void*)&right, vr::TextureType_DirectX12, vr::ColorSpace_Auto};

            if (is_multipass) {
                vr::D3D12TextureData_t left {
                    m_openvr.get_left().texture.Get(),
                    command_queue,
                    0
                };

                vr::VRTextureWithPose_t left_eye{
                    (void*)&left, vr::TextureType_DirectX12, vr::ColorSpace_Auto
                };

                auto e = vr::VRCompositor()->Submit(vr::Eye_Left, &left_eye, &vr->m_left_bounds, vr::EVRSubmitFlags::Submit_TextureWithPose);

                if (e != vr::VRCompositorError_None) {
                    spdlog::error("[VR] VRCompositor failed to submit left eye: {}", (int)e);
                    return e;
                }
            }

            auto e = vr::VRCompositor()->Submit(vr::Eye_Right, &right_eye, &vr->m_right_bounds);

            if (e != vr::VRCompositorError_None) {
                spdlog::error("[VR] VRCompositor failed to submit right eye: {}", (int)e);
                return e;
            } else {
                vr->m_submitted = true;
            }

            ++m_openvr.texture_counter;
        }
    }

    vr::EVRCompositorError e = vr::EVRCompositorError::VRCompositorError_None;

    if (frame_count % 2 == vr->m_right_eye_interval || is_multipass) {
        ////////////////////////////////////////////////////////////////////////////////
        // OpenXR start ////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        if (runtime->ready() && runtime->get_synchronize_stage() == VRRuntime::SynchronizeStage::VERY_LATE) {
            runtime->synchronize_frame();

            if (!runtime->got_first_poses) {
                runtime->update_poses();
            }
        }

        if (runtime->is_openxr() && vr->m_openxr->ready()) {
            if (runtime->get_synchronize_stage() == VRRuntime::SynchronizeStage::VERY_LATE || !vr->m_openxr->frame_began) {
                vr->m_openxr->begin_frame();
            }

            auto result = vr->m_openxr->end_frame();

            if (result == XR_ERROR_LAYER_INVALID) {
                spdlog::info("[VR] Attempting to correct invalid layer");

                m_openxr.wait_for_all_copies();

                spdlog::info("[VR] Calling xrEndFrame again");
                result = vr->m_openxr->end_frame();
            }

            vr->m_openxr->needs_pose_update = true;
            vr->m_submitted = result == XR_SUCCESS;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // OpenVR start ////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        if (runtime->is_openvr()) {
            if (runtime->needs_pose_update) {
                vr->m_submitted = false;
                spdlog::info("[VR] Runtime needed pose update inside present (frame {})", vr->m_frame_count);
                return vr::VRCompositorError_None;
            }

            //++m_openvr.texture_counter;
        }

        // Allows the desktop window to be recorded.
        if (vr->m_desktop_fix->value()) {
            if (runtime->ready() && m_prev_backbuffer != backbuffer && m_prev_backbuffer != nullptr) {
                m_generic_copiers[frame_count % 3].wait(INFINITE);
                m_generic_copiers[frame_count % 3].copy(m_prev_backbuffer.Get(), backbuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PRESENT);
                m_generic_copiers[frame_count % 3].execute();
            }
        }
    }

    m_prev_backbuffer = backbuffer;

    return e;
}

void D3D12Component::on_reset(VR* vr) {
    REF_PROFILE_FUNCTION();

    auto runtime = vr->get_runtime();

    for (auto& ctx : m_openvr.left_eye_tex) {
        ctx.copier.reset();
        ctx.texture.Reset();
    }

    for (auto& ctx : m_openvr.right_eye_tex) {
        ctx.copier.reset();
        ctx.texture.Reset();
    }

    for (auto& copier : m_generic_copiers) {
        copier.reset();
    }

    if (runtime->is_openxr() && runtime->loaded) {
        if (m_openxr.last_resolution[0] != vr->get_hmd_width() || m_openxr.last_resolution[1] != vr->get_hmd_height()) {
            m_openxr.create_swapchains();
        }

        // end the frame before something terrible happens
        //vr->m_openxr.synchronize_frame();
        //vr->m_openxr.begin_frame();
        //vr->m_openxr.end_frame();
    }

    m_prev_backbuffer = nullptr;
    m_openvr.texture_counter = 0;
}

void D3D12Component::setup() {
    REF_PROFILE_FUNCTION();

    spdlog::info("[VR] Setting up d3d12 textures...");
    
    m_prev_backbuffer = nullptr;

    auto& hook = g_framework->get_d3d12_hook();

    auto device = hook->get_device();
    auto swapchain = hook->get_swap_chain();

    ComPtr<ID3D12Resource> backbuffer{};
    ComPtr<ID3D12Resource> real_backbuffer{};

    const auto& vr = VR::get();
    const auto is_multipass = vr->is_using_multipass();
    
    if (is_multipass && vr->m_multipass.eye_textures[0].Get() != nullptr && vr->m_multipass.eye_textures[1].Get() != nullptr) {
        backbuffer = vr->m_multipass.eye_textures[0];
    } else if (is_multipass) {
        return; // return until valid textures are available
    }

    if (FAILED(swapchain->GetBuffer(0, IID_PPV_ARGS(&real_backbuffer)))) {
        spdlog::error("[VR] Failed to get back buffer.");
        return;
    }

    if (backbuffer == nullptr) {
        backbuffer = real_backbuffer;
    }

    if (backbuffer == nullptr) {
        spdlog::error("[VR] Failed to get back buffer.");
        return;
    }

    auto backbuffer_desc = backbuffer->GetDesc();
    const auto real_backbuffer_desc = real_backbuffer->GetDesc();

    spdlog::info("[VR] D3D12 Backbuffer width: {}, height: {}", backbuffer_desc.Width, backbuffer_desc.Height);
    spdlog::info("[VR] D3D12 Real Backbuffer width: {}, height: {}", real_backbuffer_desc.Width, real_backbuffer_desc.Height);

    D3D12_HEAP_PROPERTIES heap_props{};
    heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
    heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    backbuffer_desc.Width = real_backbuffer_desc.Width;
    backbuffer_desc.Height = real_backbuffer_desc.Height;
    m_openvr.last_format = backbuffer_desc.Format;

    for (auto& ctx : m_openvr.left_eye_tex) {
        if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &backbuffer_desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr,
                IID_PPV_ARGS(&ctx.texture)))) {
            spdlog::error("[VR] Failed to create left eye texture.");
            return;
        }

        ctx.copier.setup(L"OpenVR Left Eye");
    }

    for (auto& ctx : m_openvr.right_eye_tex) {
        if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &backbuffer_desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr,
                IID_PPV_ARGS(&ctx.texture)))) {
            spdlog::error("[VR] Failed to create right eye texture.");
            return;
        }

        ctx.copier.setup(L"OpenVR Right Eye");
    }

    for (auto& copier : m_generic_copiers) {
        copier.setup(L"Generic Copier");
    }

    m_backbuffer_size[0] = real_backbuffer_desc.Width;
    m_backbuffer_size[1] = real_backbuffer_desc.Height;

    spdlog::info("[VR] d3d12 textures have been setup");
    m_force_reset = false;
}

void D3D12Component::OpenXR::initialize(XrSessionCreateInfo& session_info) {
    REF_PROFILE_FUNCTION();

    std::scoped_lock _{this->mtx};

	auto& hook = g_framework->get_d3d12_hook();

    auto device = hook->get_device();
    auto command_queue = hook->get_command_queue();

    this->binding.device = device;
    this->binding.queue = command_queue;

    spdlog::info("[VR] Searching for xrGetD3D12GraphicsRequirementsKHR...");
    PFN_xrGetD3D12GraphicsRequirementsKHR fn = nullptr;
    xrGetInstanceProcAddr(VR::get()->m_openxr->instance, "xrGetD3D12GraphicsRequirementsKHR", (PFN_xrVoidFunction*)(&fn));

    XrGraphicsRequirementsD3D12KHR gr{XR_TYPE_GRAPHICS_REQUIREMENTS_D3D12_KHR};
    gr.adapterLuid = device->GetAdapterLuid();
    gr.minFeatureLevel = D3D_FEATURE_LEVEL_11_0;

    spdlog::info("[VR] Calling xrGetD3D12GraphicsRequirementsKHR");
    fn(VR::get()->m_openxr->instance, VR::get()->m_openxr->system, &gr);

    session_info.next = &this->binding;
}

std::optional<std::string> D3D12Component::OpenXR::create_swapchains() {
    std::scoped_lock _{this->mtx};

    spdlog::info("[VR] Creating OpenXR swapchains for D3D12");

    this->destroy_swapchains();
    
    auto& hook = g_framework->get_d3d12_hook();
    auto device = hook->get_device();
    auto swapchain = hook->get_swap_chain();

    ComPtr<ID3D12Resource> backbuffer{};

    const auto& vr = VR::get();
    const auto is_multipass = vr->is_using_multipass();

    // Get the existing backbuffer
    // so we can get the format and stuff.
    bool has_multipass_buffer = false;
    if (is_multipass && vr->m_multipass.eye_textures[0].Get() != nullptr) {
        backbuffer = vr->m_multipass.eye_textures[0];
        has_multipass_buffer = true;
    }

    if (backbuffer.Get() == nullptr && FAILED(swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer)))) {
        spdlog::error("[VR] Failed to get back buffer.");
        return "Failed to get back buffer.";
    }

    D3D12_HEAP_PROPERTIES heap_props{};
    heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
    heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    auto backbuffer_desc = backbuffer->GetDesc();
    auto& openxr = vr->m_openxr;

    this->contexts.clear();
    this->contexts.resize(openxr->views.size());

    this->last_format = backbuffer_desc.Format;

    DXGI_FORMAT swapchain_format{DXGI_FORMAT_R8G8B8A8_UNORM_SRGB};

    switch (backbuffer_desc.Format) {
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
            swapchain_format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;

        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
            swapchain_format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
            break;
        
        default:
            spdlog::error("[VR] Possibly unsupported backbuffer format: {}", backbuffer_desc.Format);
            break;
    };
    
    for (auto i = 0; i < openxr->views.size(); ++i) {
        spdlog::info("[VR] Creating swapchain for eye {}", i);
        spdlog::info("[VR] Width: {}", vr->get_hmd_width());
        spdlog::info("[VR] Height: {}", vr->get_hmd_height());

        backbuffer_desc.Width = vr->get_hmd_width();
        backbuffer_desc.Height = vr->get_hmd_height();

        // Create the swapchain.
        XrSwapchainCreateInfo swapchain_create_info{XR_TYPE_SWAPCHAIN_CREATE_INFO};
        swapchain_create_info.arraySize = 1;
        swapchain_create_info.format = swapchain_format;
        swapchain_create_info.width = backbuffer_desc.Width;
        swapchain_create_info.height = backbuffer_desc.Height;
        swapchain_create_info.mipCount = 1;
        swapchain_create_info.faceCount = 1;
        swapchain_create_info.sampleCount = backbuffer_desc.SampleDesc.Count;
        swapchain_create_info.usageFlags = XR_SWAPCHAIN_USAGE_MUTABLE_FORMAT_BIT | XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT | XR_SWAPCHAIN_USAGE_TRANSFER_DST_BIT;

        runtimes::OpenXR::Swapchain swapchain{};
        swapchain.width = swapchain_create_info.width;
        swapchain.height = swapchain_create_info.height;

        if (xrCreateSwapchain(openxr->session, &swapchain_create_info, &swapchain.handle) != XR_SUCCESS) {
            spdlog::error("[VR] D3D12: Failed to create swapchain.");
            return "Failed to create swapchain.";
        }

        vr->m_openxr->swapchains.push_back(swapchain);

        uint32_t image_count{};
        auto result = xrEnumerateSwapchainImages(swapchain.handle, 0, &image_count, nullptr);

        if (result != XR_SUCCESS) {
            spdlog::error("[VR] Failed to enumerate swapchain images.");
            return "Failed to enumerate swapchain images.";
        }

        auto& ctx = this->contexts[i];

        ctx.textures.clear();
        ctx.textures.resize(image_count);
        ctx.texture_contexts.clear();
        ctx.texture_contexts.resize(image_count);

        for (uint32_t j = 0; j < image_count; ++j) {
            spdlog::info("[VR] Creating swapchain image {} for swapchain {}", j, i);

            ctx.textures[j] = {XR_TYPE_SWAPCHAIN_IMAGE_D3D12_KHR};
            ctx.texture_contexts[j] = std::make_unique<D3D12Component::OpenXR::SwapchainContext::TextureContext>();
            ctx.texture_contexts[j]->copier.setup(L"OpenXR Swapchain");

            backbuffer_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            backbuffer_desc.Flags &= ~D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;

            if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &backbuffer_desc, D3D12_RESOURCE_STATE_RENDER_TARGET, nullptr, IID_PPV_ARGS(&ctx.textures[j].texture)))) {
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

    this->last_resolution = {vr->get_hmd_width(), vr->get_hmd_height()};

    return std::nullopt;
}

void D3D12Component::OpenXR::destroy_swapchains() {
    std::scoped_lock _{this->mtx};

	if (this->contexts.empty()) {
        return;
    }

    spdlog::info("[VR] Destroying swapchains.");

    for (auto i = 0; i < this->contexts.size(); ++i) {
        auto& ctx = this->contexts[i];
        ctx.texture_contexts.clear();

        auto result = xrDestroySwapchain(VR::get()->m_openxr->swapchains[i].handle);

        if (result != XR_SUCCESS) {
            spdlog::error("[VR] Failed to destroy swapchain {}.", i);
        } else {
            spdlog::info("[VR] Destroyed swapchain {}.", i);
        }

        for (auto& tex : ctx.textures) {
            tex.texture->Release();
        }
        
        ctx.textures.clear();
    }

    this->contexts.clear();
    VR::get()->m_openxr->swapchains.clear();
}

void D3D12Component::OpenXR::copy(uint32_t swapchain_idx, ID3D12Resource* resource, D3D12_BOX* src_box, D3D12_RESOURCE_STATES src_state) {
    REF_PROFILE_FUNCTION();

    std::scoped_lock _{this->mtx};

    auto& vr = VR::get();

    if (vr->m_openxr->frame_state.shouldRender != XR_TRUE) {
        return;
    }

    if (!vr->m_openxr->frame_began) {
        if (vr->m_openxr->get_synchronize_stage() != VRRuntime::SynchronizeStage::VERY_LATE) {
            spdlog::error("[VR] OpenXR: Frame not begun when trying to copy.");
            return;
        }
    }

    if (this->contexts[swapchain_idx].num_textures_acquired > 0) {
        spdlog::info("[VR] Already acquired textures for swapchain {}?", swapchain_idx);
    }

    const auto& swapchain = vr->m_openxr->swapchains[swapchain_idx];
    auto& ctx = this->contexts[swapchain_idx];

    XrSwapchainImageAcquireInfo acquire_info{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};

    uint32_t texture_index{};
    auto result = xrAcquireSwapchainImage(swapchain.handle, &acquire_info, &texture_index);

    if (result == XR_ERROR_RUNTIME_FAILURE) {
        spdlog::error("[VR] xrAcquireSwapchainImage failed: {}", vr->m_openxr->get_result_string(result));
        spdlog::info("[VR] Attempting to correct...");

        for (auto& texture_ctx : ctx.texture_contexts) {
            texture_ctx->copier.reset();
        }

        texture_index = 0;
        result = xrAcquireSwapchainImage(swapchain.handle, &acquire_info, &texture_index);
    }


    if (result != XR_SUCCESS) {
        spdlog::error("[VR] xrAcquireSwapchainImage failed: {}", vr->m_openxr->get_result_string(result));
    } else {
        ctx.num_textures_acquired++;

        XrSwapchainImageWaitInfo wait_info{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
        //wait_info.timeout = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
        wait_info.timeout = XR_INFINITE_DURATION;
        result = xrWaitSwapchainImage(swapchain.handle, &wait_info);

        if (result != XR_SUCCESS) {
            spdlog::error("[VR] xrWaitSwapchainImage failed: {}", vr->m_openxr->get_result_string(result));
        } else {
            auto& texture_ctx = ctx.texture_contexts[texture_index];
            texture_ctx->copier.wait(INFINITE);
            if (src_box != nullptr) {
                texture_ctx->copier.copy_region(
                    resource, 
                    ctx.textures[texture_index].texture, 
                    0, 0, 0, src_box, src_state, 
                    D3D12_RESOURCE_STATE_RENDER_TARGET);
            } else {
                texture_ctx->copier.copy(
                    resource, 
                    ctx.textures[texture_index].texture, 
                    src_state, 
                    D3D12_RESOURCE_STATE_RENDER_TARGET);
            }
            texture_ctx->copier.execute();

            XrSwapchainImageReleaseInfo release_info{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
            auto result = xrReleaseSwapchainImage(swapchain.handle, &release_info);

            // SteamVR shenanigans.
            if (result == XR_ERROR_RUNTIME_FAILURE) {
                spdlog::error("[VR] xrReleaseSwapchainImage failed: {}", vr->m_openxr->get_result_string(result));
                spdlog::info("[VR] Attempting to correct...");

                result = xrWaitSwapchainImage(swapchain.handle, &wait_info);

                if (result != XR_SUCCESS) {
                    spdlog::error("[VR] xrWaitSwapchainImage failed: {}", vr->m_openxr->get_result_string(result));
                }

                for (auto& texture_ctx : ctx.texture_contexts) {
                    texture_ctx->copier.wait(INFINITE);
                }

                result = xrReleaseSwapchainImage(swapchain.handle, &release_info);
            }

            if (result != XR_SUCCESS) {
                spdlog::error("[VR] xrReleaseSwapchainImage failed: {}", vr->m_openxr->get_result_string(result));
                return;
            }

            ctx.num_textures_acquired--;
        }
    }
}

void D3D12Component::ResourceCopier::setup(const wchar_t* name) {
    std::scoped_lock _{this->mtx};

    this->internal_name = name;

    auto& hook = g_framework->get_d3d12_hook();
    auto device = hook->get_device();

    if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&this->cmd_allocator)))) {
        spdlog::error("[VR] Failed to create command allocator.");
        return;
    }

    if (FAILED(device->CreateCommandList(
            0, D3D12_COMMAND_LIST_TYPE_DIRECT, this->cmd_allocator.Get(), nullptr, IID_PPV_ARGS(&this->cmd_list)))) {
        spdlog::error("[VR] Failed to create command list.");
        return;
    }

    if (FAILED(device->CreateFence(this->fence_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&this->fence)))) {
        spdlog::error("[VR] Failed to create fence.");
        return;
    }

    this->fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void D3D12Component::ResourceCopier::reset() {
    std::scoped_lock _{this->mtx};
    this->wait(2000);
    //this->on_post_present(VR::get().get());

    this->cmd_allocator.Reset();
    this->cmd_list.Reset();
    this->fence.Reset();
    this->fence_value = 0;
    CloseHandle(this->fence_event);
    this->fence_event = 0;
    this->waiting_for_fence = false;
}

void D3D12Component::ResourceCopier::wait(uint32_t ms) {
    REF_PROFILE_FUNCTION();

    std::scoped_lock _{this->mtx};

	if (this->fence_event && this->waiting_for_fence) {
        WaitForSingleObject(this->fence_event, ms);
        ResetEvent(this->fence_event);
        this->waiting_for_fence = false;
        this->cmd_allocator->Reset();
        this->cmd_list->Reset(this->cmd_allocator.Get(), nullptr);
        this->has_commands = false;
    }
}

void D3D12Component::ResourceCopier::copy(ID3D12Resource* src, ID3D12Resource* dst, D3D12_RESOURCE_STATES src_state, D3D12_RESOURCE_STATES dst_state) {
    REF_PROFILE_FUNCTION();
    
    std::scoped_lock _{this->mtx};

    // Switch src into copy source.
    D3D12_RESOURCE_BARRIER src_barrier{};

    src_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    src_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    src_barrier.Transition.pResource = src;
    src_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    src_barrier.Transition.StateBefore = src_state;
    src_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;

    // Switch dst into copy destination.
    D3D12_RESOURCE_BARRIER dst_barrier{};
    dst_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    dst_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    dst_barrier.Transition.pResource = dst;
    dst_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    dst_barrier.Transition.StateBefore = dst_state;
    dst_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

    {
        D3D12_RESOURCE_BARRIER barriers[2]{src_barrier, dst_barrier};
        this->cmd_list->ResourceBarrier(2, barriers);
    }

    // Copy the resource.
    this->cmd_list->CopyResource(dst, src);

    // Switch back to present.
    src_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
    src_barrier.Transition.StateAfter = src_state;
    dst_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    dst_barrier.Transition.StateAfter = dst_state;

    {
        D3D12_RESOURCE_BARRIER barriers[2]{src_barrier, dst_barrier};
        this->cmd_list->ResourceBarrier(2, barriers);
    }

    this->has_commands = true;
}

void D3D12Component::ResourceCopier::copy_region(ID3D12Resource* src, ID3D12Resource* dst, 
    uint32_t dst_x, uint32_t dst_y, uint32_t dst_z, D3D12_BOX* src_box, 
    D3D12_RESOURCE_STATES src_state, D3D12_RESOURCE_STATES dst_state) 
{
    std::scoped_lock _{this->mtx};

    // Switch src into copy source.
    D3D12_RESOURCE_BARRIER src_barrier{};

    src_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    src_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    src_barrier.Transition.pResource = src;
    src_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    src_barrier.Transition.StateBefore = src_state;
    src_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;

    // Switch dst into copy destination.
    D3D12_RESOURCE_BARRIER dst_barrier{};
    dst_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    dst_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    dst_barrier.Transition.pResource = dst;
    dst_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    dst_barrier.Transition.StateBefore = dst_state;
    dst_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

    {
        D3D12_RESOURCE_BARRIER barriers[2]{src_barrier, dst_barrier};
        this->cmd_list->ResourceBarrier(2, barriers);
    }

    // Copy the resource.
    D3D12_TEXTURE_COPY_LOCATION src_loc{};
    src_loc.pResource = src;
    src_loc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    src_loc.SubresourceIndex = 0;

    D3D12_TEXTURE_COPY_LOCATION dst_loc{};
    dst_loc.pResource = dst;
    dst_loc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dst_loc.SubresourceIndex = 0;

    this->cmd_list->CopyTextureRegion(&dst_loc, dst_x, dst_y, dst_z, &src_loc, src_box);

    // Switch back to present.
    src_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
    src_barrier.Transition.StateAfter = src_state;
    dst_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    dst_barrier.Transition.StateAfter = dst_state;

    {
        D3D12_RESOURCE_BARRIER barriers[2]{src_barrier, dst_barrier};
        this->cmd_list->ResourceBarrier(2, barriers);
    }

    this->has_commands = true;
}

void D3D12Component::ResourceCopier::clear_rtv(ID3D12Resource* dst, D3D12_CPU_DESCRIPTOR_HANDLE rtv, const float* color, D3D12_RESOURCE_STATES dst_state) {
    std::scoped_lock _{this->mtx};

    // Switch dst into copy destination.
    D3D12_RESOURCE_BARRIER dst_barrier{};
    dst_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    dst_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    dst_barrier.Transition.pResource = dst;
    dst_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    dst_barrier.Transition.StateBefore = dst_state;
    dst_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

    {
        D3D12_RESOURCE_BARRIER barriers[1]{dst_barrier};
        this->cmd_list->ResourceBarrier(1, barriers);
    }

    // Clear the resource.
    this->cmd_list->ClearRenderTargetView(rtv, color, 0, nullptr);

    // Switch back to present.
    dst_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    dst_barrier.Transition.StateAfter = dst_state;

    {
        D3D12_RESOURCE_BARRIER barriers[1]{dst_barrier};
        this->cmd_list->ResourceBarrier(1, barriers);
    }

    this->has_commands = true;
}

void D3D12Component::ResourceCopier::execute() {
    REF_PROFILE_FUNCTION();

    std::scoped_lock _{this->mtx};
    
    if (this->has_commands) {
        if (FAILED(this->cmd_list->Close())) {
            spdlog::error("[VR] Failed to close command list. ({})", utility::narrow(this->internal_name));
            return;
        }
        
        auto command_queue = g_framework->get_d3d12_hook()->get_command_queue();
        command_queue->ExecuteCommandLists(1, (ID3D12CommandList* const*)this->cmd_list.GetAddressOf());
        command_queue->Signal(this->fence.Get(), ++this->fence_value);
        this->fence->SetEventOnCompletion(this->fence_value, this->fence_event);
        this->waiting_for_fence = true;
        this->has_commands = false;
    }
}
} // namespace vrmod
