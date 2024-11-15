#include "BackBufferRenderer.hpp"

std::shared_ptr<BackBufferRenderer>& BackBufferRenderer::get() {
    static auto instance = std::make_shared<BackBufferRenderer>();
    return instance;
}

std::optional<std::string> BackBufferRenderer::on_initialize_d3d_thread() {
    if (g_framework->is_dx12()) {
        for (auto& context : m_d3d12.command_contexts) {
            context = std::make_unique<d3d12::CommandContext>();
            context->setup(L"BackBufferRenderer D3D12 Command Context");
        }

        auto swapchain = g_framework->get_d3d12_hook()->get_swap_chain();
        auto device = g_framework->get_d3d12_hook()->get_device();
        
        d3d12::ComPtr<ID3D12Resource> backbuffer{};
        if (FAILED(swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer)))) {
            return "Failed to get back buffer";
        }

        auto desc = backbuffer->GetDesc();

        m_d3d12.default_rt_state = DirectX::RenderTargetState{desc.Format, DXGI_FORMAT_UNKNOWN};

        spdlog::info("BackBufferRenderer D3D12 initialized");
    } else {
        // TODO
        spdlog::info("BackBufferRenderer D3D11 initialized");
    }

    // OK
    return Mod::on_initialize();
}

void BackBufferRenderer::on_device_reset() {
    for (auto& ctx : m_d3d12.command_contexts) {
        ctx.reset();
    }

    for (auto& bb : m_d3d12.backbuffers) {
        bb.reset();
    }
}

void BackBufferRenderer::render_d3d12() {
    if (m_d3d12.render_work.empty()) {
        return;
    }

    for (auto& ctx : m_d3d12.command_contexts) {
        if (ctx == nullptr) {
            ctx = std::make_unique<d3d12::CommandContext>();
            ctx->setup(L"BackBufferRenderer D3D12 Command Context");
        }
    }

    auto swapchain = g_framework->get_d3d12_hook()->get_swap_chain();
    auto device = g_framework->get_d3d12_hook()->get_device();
    for (size_t i = 0; i < m_d3d12.backbuffers.size(); ++i) {
        d3d12::ComPtr<ID3D12Resource> backbuffer{};
        if (FAILED(swapchain->GetBuffer(i, IID_PPV_ARGS(&backbuffer)))) {
            break;
        }

        if (m_d3d12.backbuffers[i] == nullptr || m_d3d12.backbuffers[i]->texture.Get() != backbuffer.Get()) {
            spdlog::info("[BackBufferRenderer] Setting up backbuffer {}", i);
            
            m_d3d12.backbuffers[i].reset();

            m_d3d12.backbuffers[i] = std::make_unique<d3d12::TextureContext>();
            if (!m_d3d12.backbuffers[i]->setup(device, backbuffer.Get(), std::nullopt, std::nullopt, L"BackBufferRenderer Backbuffer")) {
                spdlog::error("[BackBufferRenderer] Failed to setup backbuffer {}", i);
                m_d3d12.backbuffers[i].reset();
                continue;
            }
        }
    }

    const auto bb_index = swapchain->GetCurrentBackBufferIndex();

    d3d12::ComPtr<ID3D12Resource> backbuffer{};
    if (FAILED(swapchain->GetBuffer(bb_index, IID_PPV_ARGS(&backbuffer)))) {
        return;
    }

    const auto desc = backbuffer->GetDesc();

    auto& bb_context = m_d3d12.backbuffers[bb_index % m_d3d12.backbuffers.size()];
    auto& command_context = m_d3d12.command_contexts[bb_index % m_d3d12.command_contexts.size()];

    command_context->wait(2000);

    auto& cmd_list = command_context->cmd_list;

    D3D12_RECT scissor_rect{};
    scissor_rect.left = 0;
    scissor_rect.top = 0;
    scissor_rect.right = (LONG)desc.Width;
    scissor_rect.bottom = (LONG)desc.Height;

    m_d3d12.viewport.Width = (float)desc.Width;
    m_d3d12.viewport.Height = (float)desc.Height;
    m_d3d12.viewport.MaxDepth = 1.0f;

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = backbuffer.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    cmd_list->ResourceBarrier(1, &barrier);

    D3D12_CPU_DESCRIPTOR_HANDLE rtv_heaps[] = { bb_context->get_rtv() };
    cmd_list->OMSetRenderTargets(1, rtv_heaps, FALSE, nullptr);

    cmd_list->RSSetViewports(1, &m_d3d12.viewport);
    cmd_list->RSSetScissorRects(1, &scissor_rect);

    decltype(m_d3d12.render_work) works{};
    {
        std::scoped_lock _{m_d3d12.render_work_mtx};
        works = m_d3d12.render_work;
    }

    const RenderWorkData data{
        cmd_list.Get(),
        m_d3d12.viewport,
        bb_context.get()
    };

    for (auto& work : works) {
        work(data);
    }

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    cmd_list->ResourceBarrier(1, &barrier);

    command_context->has_commands = true;
    command_context->execute();
}

void BackBufferRenderer::render_d3d11() {
    // TODO
}

void BackBufferRenderer::on_present() {
    if (g_framework->is_dx12()) {
        render_d3d12();
    } else {
        render_d3d11();
    }
}

void BackBufferRenderer::on_frame() {
    // Clearing this here instead of every time whenever we present fixes flickering in some games
    if (g_framework->is_dx12() && !m_d3d12.render_work.empty()) {
        std::scoped_lock _{m_d3d12.render_work_mtx};
        m_d3d12.render_work.clear();
    }
}