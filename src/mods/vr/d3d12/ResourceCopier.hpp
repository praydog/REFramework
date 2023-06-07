#pragma once

#include <mutex>
#include <d3d12.h>

#include "ComPtr.hpp"

namespace d3d12 {
struct ResourceCopier {
    virtual ~ResourceCopier() { this->reset(); }

    void setup(const wchar_t* name = L"ResourceCopier object");
    void reset();
    void wait(uint32_t ms);
    void copy(ID3D12Resource* src, ID3D12Resource* dst, D3D12_RESOURCE_STATES src_state = D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATES dst_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    void execute();

    ComPtr<ID3D12CommandAllocator> cmd_allocator{};
    ComPtr<ID3D12GraphicsCommandList> cmd_list{};
    ComPtr<ID3D12Fence> fence{};
    UINT64 fence_value{};
    HANDLE fence_event{};

    std::recursive_mutex mtx{};

    bool waiting_for_fence{false};
    bool has_commands{false};
};
}