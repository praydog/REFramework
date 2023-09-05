#pragma once

#include <optional>

#include <../../directxtk12-src/Inc/DescriptorHeap.h>

#include "CommandContext.hpp"

namespace d3d12 {
struct TextureContext {
    CommandContext commands{};
    ComPtr<ID3D12Resource> texture{};
    std::unique_ptr<DirectX::DescriptorHeap> rtv_heap{};
    std::unique_ptr<DirectX::DescriptorHeap> srv_heap{};

    bool setup(ID3D12Device* device, ID3D12Resource* rsrc, std::optional<DXGI_FORMAT> rtv_format, std::optional<DXGI_FORMAT> srv_format, const wchar_t* name = L"TextureContext object");
    bool create_rtv(ID3D12Device* device, std::optional<DXGI_FORMAT> format = std::nullopt);
    bool create_srv(ID3D12Device* device, std::optional<DXGI_FORMAT> format = std::nullopt);

    D3D12_CPU_DESCRIPTOR_HANDLE get_rtv() const {
        return rtv_heap->GetCpuHandle(0);
    }

    D3D12_GPU_DESCRIPTOR_HANDLE get_srv_gpu() const {
        return srv_heap->GetGpuHandle(0);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE get_srv_cpu() const {
        return srv_heap->GetCpuHandle(0);
    }

    void reset() {
        commands.reset();
        rtv_heap.reset();
        srv_heap.reset();
        texture.Reset();
    }

    virtual ~TextureContext() {
        reset();
    }
};
}