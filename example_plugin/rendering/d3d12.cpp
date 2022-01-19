#include "../Plugin.hpp"
#include "../imgui/imgui_impl_dx12.h"

#include "d3d12.hpp"

D3D12 g_d3d12{};

bool D3D12::initialize() {
    auto device = (ID3D12Device*)g_param->renderer_data->device;
    auto swapchain = (IDXGISwapChain3*)g_param->renderer_data->swapchain;

    if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&this->cmd_allocator)))) {
        return false;
    }

    if (FAILED(device->CreateCommandList(
            0, D3D12_COMMAND_LIST_TYPE_DIRECT, this->cmd_allocator.Get(), nullptr, IID_PPV_ARGS(&this->cmd_list)))) {
        return false;
    }

    if (FAILED(this->cmd_list->Close())) {
        return false;
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc{};

        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.NumDescriptors = (int)D3D12::RTV::COUNT; 
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 1;

        if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&this->rtv_desc_heap)))) {
            return false;
        }
    }
    
    { 
        D3D12_DESCRIPTOR_HEAP_DESC desc{};
        
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = (int)D3D12::SRV::COUNT;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&this->srv_desc_heap)))) {
            return false;
        }
    }

    {
        for (auto i = 0; i <= (int)D3D12::RTV::BACKBUFFER_3; ++i) {
            if (SUCCEEDED(swapchain->GetBuffer(i, IID_PPV_ARGS(&this->rts[i])))) {
                device->CreateRenderTargetView(this->rts[i].Get(), nullptr, this->get_cpu_rtv(device, (D3D12::RTV)i));
            }
        }

        // Create our imgui and blank rts.
        auto& backbuffer = this->get_rt(D3D12::RTV::BACKBUFFER_0);
        auto desc = backbuffer->GetDesc();

        D3D12_HEAP_PROPERTIES props{};
        props.Type = D3D12_HEAP_TYPE_DEFAULT;
        props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        D3D12_CLEAR_VALUE clear_value{};
        clear_value.Format = desc.Format;

        if (FAILED(device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &clear_value,
                IID_PPV_ARGS(&this->get_rt(D3D12::RTV::IMGUI))))) {
            return false;
        }

        if (FAILED(device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &clear_value,
                IID_PPV_ARGS(&this->get_rt(D3D12::RTV::BLANK))))) {
            return false;
        }

        // Create imgui and blank rtvs and srvs.
        device->CreateRenderTargetView(this->get_rt(D3D12::RTV::IMGUI).Get(), nullptr, this->get_cpu_rtv(device, D3D12::RTV::IMGUI));
        device->CreateRenderTargetView(this->get_rt(D3D12::RTV::BLANK).Get(), nullptr, this->get_cpu_rtv(device, D3D12::RTV::BLANK));
        device->CreateShaderResourceView(
            this->get_rt(D3D12::RTV::IMGUI).Get(), nullptr, this->get_cpu_srv(device, D3D12::SRV::IMGUI));
        device->CreateShaderResourceView(this->get_rt(D3D12::RTV::BLANK).Get(), nullptr, this->get_cpu_srv(device, D3D12::SRV::BLANK));

        this->rt_width = (uint32_t)desc.Width;
        this->rt_height = (uint32_t)desc.Height;
    }

    if (!ImGui_ImplDX12_Init(device, 1, DXGI_FORMAT_R8G8B8A8_UNORM, this->srv_desc_heap.Get(),
        this->get_cpu_srv(device, D3D12::SRV::IMGUI_FONT), this->get_gpu_srv(device, D3D12::SRV::IMGUI_FONT))) 
    {
        return false;
    }

    return true;
}

void D3D12::render_imgui() {
    auto command_queue = (ID3D12CommandQueue*)g_param->renderer_data->command_queue;

    this->cmd_allocator->Reset();
    this->cmd_list->Reset(g_d3d12.cmd_allocator.Get(), nullptr);

    // Draw to our render target.
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = this->get_rt(D3D12::RTV::IMGUI).Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    this->cmd_list->ResourceBarrier(1, &barrier);

    float clear_color[]{0.0f, 0.0f, 0.0f, 0.0f};
    auto device = (ID3D12Device*)g_param->renderer_data->device;
    D3D12_CPU_DESCRIPTOR_HANDLE rts[1]{};
    this->cmd_list->ClearRenderTargetView(this->get_cpu_rtv(device, D3D12::RTV::IMGUI), clear_color, 0, nullptr);
    rts[0] = this->get_cpu_rtv(device, D3D12::RTV::IMGUI);
    this->cmd_list->OMSetRenderTargets(1, rts, FALSE, NULL);
    this->cmd_list->SetDescriptorHeaps(1, this->srv_desc_heap.GetAddressOf());
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), this->cmd_list.Get());
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    this->cmd_list->ResourceBarrier(1, &barrier);

    // Draw to the back buffer.
    auto swapchain = (IDXGISwapChain3*)g_param->renderer_data->swapchain;
    auto bb_index = swapchain->GetCurrentBackBufferIndex();
    barrier.Transition.pResource = this->rts[bb_index].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    this->cmd_list->ResourceBarrier(1, &barrier);
    rts[0] = this->get_cpu_rtv(device, (D3D12::RTV)bb_index);
    this->cmd_list->OMSetRenderTargets(1, rts, FALSE, NULL);
    this->cmd_list->SetDescriptorHeaps(1, this->srv_desc_heap.GetAddressOf());
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), this->cmd_list.Get());
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    this->cmd_list->ResourceBarrier(1, &barrier);
    this->cmd_list->Close();

    command_queue->ExecuteCommandLists(1, (ID3D12CommandList* const*)this->cmd_list.GetAddressOf());
}
