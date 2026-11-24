#include <utility/String.hpp>

#include <spdlog/spdlog.h>

#include "CommandContext.hpp"
#include "TextureContext.hpp"

namespace d3d12 {
bool TextureContext::setup(ID3D12Device* device, ID3D12Resource* rsrc, std::optional<DXGI_FORMAT> rtv_format, std::optional<DXGI_FORMAT> srv_format, const wchar_t* name) {
    spdlog::info("Setting up texture context for {}", utility::narrow(name));
    
    reset();

    commands.setup(name);

    texture.Reset();
    texture = rsrc;

    if (rsrc == nullptr) {
        return false;
    }

    return create_rtv(device, rtv_format) && create_srv(device, srv_format);
}

// A view's dimension must match the resource. These contexts wrap resources we do not own
// (OpenXR swapchain images, the game's multipass eye textures), so hardcoding TEXTURE2D can
// produce CREATERENDERTARGETVIEW_INVALIDRESOURCE, which removes the device. Validate first and
// log the desc so a rejected resource is identifiable instead of fatal.
static bool is_typeless(DXGI_FORMAT format) {
    switch (format) {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        case DXGI_FORMAT_R32G32B32_TYPELESS:
        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        case DXGI_FORMAT_R32G32_TYPELESS:
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R16G16_TYPELESS:
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_R8G8_TYPELESS:
        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_R8_TYPELESS:
        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC4_TYPELESS:
        case DXGI_FORMAT_BC5_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        case DXGI_FORMAT_BC6H_TYPELESS:
        case DXGI_FORMAT_BC7_TYPELESS:
            return true;
        default:
            return false;
    }
}

void TextureContext::log_desc(const char* view_kind, const D3D12_RESOURCE_DESC& desc) const {
    spdlog::info("[VR] {}: {} source dim={} {}x{} array={} mips={} samples={} format={} flags={:#x}",
        utility::narrow(commands.internal_name), view_kind, (int)desc.Dimension, (uint64_t)desc.Width, desc.Height,
        desc.DepthOrArraySize, desc.MipLevels, desc.SampleDesc.Count, (int)desc.Format, (uint32_t)desc.Flags);
}

bool TextureContext::create_rtv(ID3D12Device* device, std::optional<DXGI_FORMAT> format) {
    spdlog::info("Creating RTV for texture context {}", utility::narrow(commands.internal_name));

    rtv_heap.reset();

    const auto desc = texture->GetDesc();
    log_desc("RTV", desc);

    if (desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D) {
        spdlog::error("[VR] {}: cannot create an RTV, resource is not a 2D texture", utility::narrow(commands.internal_name));
        return false;
    }

    if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) == 0) {
        spdlog::error("[VR] {}: cannot create an RTV, resource lacks ALLOW_RENDER_TARGET", utility::narrow(commands.internal_name));
        return false;
    }

    D3D12_RENDER_TARGET_VIEW_DESC rtv_desc{};
    rtv_desc.Format = format.value_or(desc.Format);

    if (is_typeless(rtv_desc.Format)) {
        spdlog::error("[VR] {}: cannot create an RTV, typeless resource needs an explicit view format", utility::narrow(commands.internal_name));
        return false;
    }

    if (desc.SampleDesc.Count > 1) {
        if (desc.DepthOrArraySize > 1) {
            rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
            rtv_desc.Texture2DMSArray.ArraySize = desc.DepthOrArraySize;
        } else {
            rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
        }
    } else if (desc.DepthOrArraySize > 1) {
        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
        rtv_desc.Texture2DArray.ArraySize = desc.DepthOrArraySize;
    } else {
        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    }

    // create descriptor heap
    try {
        rtv_heap = std::make_unique<DirectX::DescriptorHeap>(device,
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
            1);
    } catch(...) {
        spdlog::error("Failed to create RTV descriptor heap for {}", utility::narrow(commands.internal_name));
        return false;
    }

    if (rtv_heap->Heap() == nullptr) {
        return false;
    }

    rtv_heap->Heap()->SetName((commands.internal_name + L" RTV Heap").c_str());
    device->CreateRenderTargetView(texture.Get(), &rtv_desc, get_rtv());

    return true;
}

bool TextureContext::create_srv(ID3D12Device* device, std::optional<DXGI_FORMAT> format) {
    spdlog::info("Creating SRV for texture context {}", utility::narrow(commands.internal_name));

    srv_heap.reset();

    const auto desc = texture->GetDesc();
    log_desc("SRV", desc);

    if (desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D) {
        spdlog::error("[VR] {}: cannot create an SRV, resource is not a 2D texture", utility::narrow(commands.internal_name));
        return false;
    }

    if ((desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) != 0) {
        spdlog::error("[VR] {}: cannot create an SRV, resource denies shader resources", utility::narrow(commands.internal_name));
        return false;
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{};
    srv_desc.Format = format.value_or(desc.Format);
    srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    if (is_typeless(srv_desc.Format)) {
        spdlog::error("[VR] {}: cannot create an SRV, typeless resource needs an explicit view format", utility::narrow(commands.internal_name));
        return false;
    }

    if (desc.SampleDesc.Count > 1) {
        if (desc.DepthOrArraySize > 1) {
            srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
            srv_desc.Texture2DMSArray.ArraySize = desc.DepthOrArraySize;
        } else {
            srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
        }
    } else if (desc.DepthOrArraySize > 1) {
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
        srv_desc.Texture2DArray.ArraySize = desc.DepthOrArraySize;
        srv_desc.Texture2DArray.MipLevels = desc.MipLevels;
    } else {
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels = desc.MipLevels;
    }

    // create descriptor heap
    try {
        srv_heap = std::make_unique<DirectX::DescriptorHeap>(device,
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            1);
    } catch(...) {
        spdlog::error("Failed to create SRV descriptor heap for {}", utility::narrow(commands.internal_name));
        return false;
    }

    if (srv_heap->Heap() == nullptr) {
        return false;
    }

    srv_heap->Heap()->SetName((commands.internal_name + L" SRV Heap").c_str());
    device->CreateShaderResourceView(texture.Get(), &srv_desc, get_srv_cpu());

    return true;
}
}