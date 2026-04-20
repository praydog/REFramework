#pragma once

#include <mutex>
#include <d3d12.h>

#include "ComPtr.hpp"

namespace d3d12 {
struct TextureContext;

struct CommandContext {
    CommandContext() = default;
    virtual ~CommandContext() { this->reset(); }

    bool setup(const wchar_t* name = L"CommandContext object");
    void reset();
    void wait(uint32_t ms);
    // Non-blocking variant: returns true if the fence was already signalled (or
    // nothing was pending) and the context was reset for reuse. Returns false
    // if the GPU is still busy on this context's previous submission. Used to
    // avoid stalling the Present thread on frame-generation swapchains.
    bool try_wait(uint32_t ms = 0);
    void copy(ID3D12Resource* src, ID3D12Resource* dst, 
        D3D12_RESOURCE_STATES src_state = D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATES dst_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    void copy_region(ID3D12Resource* src, ID3D12Resource* dst, D3D12_BOX* src_box, 
        D3D12_RESOURCE_STATES src_state = D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATES dst_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    void clear_rtv(ID3D12Resource* dst, D3D12_CPU_DESCRIPTOR_HANDLE rtv, const float* color, 
        D3D12_RESOURCE_STATES dst_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    void clear_rtv(TextureContext& tex, const float* color, D3D12_RESOURCE_STATES dst_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    void execute();

    ComPtr<ID3D12CommandAllocator> cmd_allocator{};
    ComPtr<ID3D12GraphicsCommandList> cmd_list{};
    ComPtr<ID3D12Fence> fence{};
    UINT64 fence_value{};
    HANDLE fence_event{};

    std::recursive_mutex mtx{};

    bool waiting_for_fence{false};
    bool has_commands{false};

    std::wstring internal_name{L"CommandContext object"};
};
}