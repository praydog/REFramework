#include <spdlog/spdlog.h>

#include <../../directxtk12-src/Inc/ResourceUploadBatch.h>
#include <../../directxtk12-src/Inc/RenderTargetState.h>

#include "REFramework.hpp"
#include "DirectXTK.hpp"

namespace d3d12 {
void render_srv_to_rtv(
    DirectX::DX12::SpriteBatch* batch,
    ID3D12GraphicsCommandList* command_list, 
    const d3d12::TextureContext& src, 
    const d3d12::TextureContext& dst, 
    D3D12_RESOURCE_STATES src_state, 
    D3D12_RESOURCE_STATES dst_state)
{
    if (!src.has_srv() || !dst.has_rtv()) {
        spdlog::error("[VR] render_srv_to_rtv: missing views (src srv={}, dst rtv={}); skipping draw",
            src.has_srv(), dst.has_rtv());
        return;
    }

    const auto dst_desc = dst.texture->GetDesc();
    const auto src_desc = src.texture->GetDesc();

    D3D12_VIEWPORT viewport{};
    viewport.Width = (float)dst_desc.Width;
    viewport.Height = (float)dst_desc.Height;
    viewport.MinDepth = D3D12_MIN_DEPTH;
    viewport.MaxDepth = D3D12_MAX_DEPTH;
    
    batch->SetViewport(viewport);

    D3D12_RECT scissor_rect{};
    scissor_rect.left = 0;
    scissor_rect.top = 0;
    scissor_rect.right = (LONG)dst_desc.Width;
    scissor_rect.bottom = (LONG)dst_desc.Height;

    // Transition dst to D3D12_RESOURCE_STATE_RENDER_TARGET
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = dst.texture.Get();

    // dst is the resource being transitioned here, so its current state is dst_state, not src_state.
    if (dst_state != D3D12_RESOURCE_STATE_RENDER_TARGET) {
        barrier.Transition.StateBefore = dst_state;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        command_list->ResourceBarrier(1, &barrier);
    }

    // Set RTV to backbuffer
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_heaps[] = { dst.get_rtv() };
    command_list->OMSetRenderTargets(1, rtv_heaps, FALSE, nullptr);

    // Setup viewport and scissor rects
    command_list->RSSetViewports(1, &viewport);
    command_list->RSSetScissorRects(1, &scissor_rect);

    batch->Begin(command_list, DirectX::DX12::SpriteSortMode::SpriteSortMode_Immediate);

    RECT dest_rect{ 0, 0, (LONG)dst_desc.Width, (LONG)dst_desc.Height };

    // Set descriptor heaps
    ID3D12DescriptorHeap* game_heaps[] = { src.srv_heap->Heap() };
    command_list->SetDescriptorHeaps(1, game_heaps);

    batch->Draw(src.get_srv_gpu(), 
        DirectX::XMUINT2{ (uint32_t)src_desc.Width, (uint32_t)src_desc.Height },
        dest_rect,
        DirectX::Colors::White);

    batch->End();

    // Transition dst to dst_state
    if (dst_state != D3D12_RESOURCE_STATE_RENDER_TARGET) {
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = dst_state;
        command_list->ResourceBarrier(1, &barrier);
    }
}

std::unique_ptr<DirectX::DX12::SpriteBatch> setup_sprite_batch_pso(
    DXGI_FORMAT output_format, 
    std::span<const uint8_t> ps, 
    std::span<const uint8_t> vs, 
    std::optional<DirectX::SpriteBatchPipelineStateDescription> pd) 
{
    spdlog::info("[D3D12] Setting up sprite batch PSO");

    auto& hook = g_framework->get_d3d12_hook();

    auto device = hook->get_device();
    auto command_queue = hook->get_command_queue();
    auto swapchain = hook->get_swap_chain();

    DirectX::ResourceUploadBatch upload{ device };
    upload.Begin();

    if (!pd) {
        pd = DirectX::SpriteBatchPipelineStateDescription{DirectX::RenderTargetState{output_format, DXGI_FORMAT_UNKNOWN}};
    }

    if (ps.size() > 0) {
        pd->customPixelShader = D3D12_SHADER_BYTECODE{ps.data(), ps.size()};
    }

    if (vs.size() > 0) {
        pd->customVertexShader = D3D12_SHADER_BYTECODE{vs.data(), vs.size()};
    }

    auto batch = std::make_unique<DirectX::DX12::SpriteBatch>(device, upload, *pd);

    auto result = upload.End(command_queue);
    result.wait();

    spdlog::info("[D3D12] Sprite batch PSO setup complete");

    return batch;
}
}