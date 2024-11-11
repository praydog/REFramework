#pragma once

#include <chrono>

#include <../directxtk12-src/Inc/SimpleMath.h>
#include <../directxtk12-src/Inc/CommonStates.h>
#include <../directxtk12-src/Inc/Effects.h>
#include <../directxtk12-src/Inc/GeometricPrimitive.h>

#include "vr/d3d12/CommandContext.hpp"
#include "vr/d3d12/TextureContext.hpp"
#include "vr/d3d12/ComPtr.hpp"

#include "Mod.hpp"

// BackBufferRenderer is a backend mod that other mods can use
// to render to the backbuffer, so we don't need to have boilerplate everywhere.
// Automatically sets up render target to backbuffer, scissor rect and viewport.
class BackBufferRenderer : public Mod {
public:
    static std::shared_ptr<BackBufferRenderer>& get();

public:
    std::string_view get_name() const override {
        return "BackBufferRenderer";
    }

    std::optional<std::string> on_initialize_d3d_thread() override;
    void on_present() override;
    void on_frame() override;
    void on_device_reset() override;

public:
    struct RenderWorkData {
        ID3D12GraphicsCommandList* command_list;
        D3D12_VIEWPORT viewport;
        d3d12::TextureContext* backbuffer_ctx;
    };
    using D3D12RenderWorkFn = std::function<void(const RenderWorkData&)>;
    
    void submit_work_d3d12(D3D12RenderWorkFn&& work) {
        std::scoped_lock _{ m_d3d12.render_work_mtx };
        m_d3d12.render_work.push_back(std::move(work));
    }

    void submit_work_d3d12(std::vector<D3D12RenderWorkFn>&& work) {
        std::scoped_lock _{ m_d3d12.render_work_mtx };

        if (m_d3d12.render_work.empty()) {
            m_d3d12.render_work = std::move(work);
            return;
        }

        m_d3d12.render_work.insert(m_d3d12.render_work.end(), work.begin(), work.end());
    }

    DirectX::RenderTargetState get_default_rt_state() {
        return m_d3d12.default_rt_state;
    }

    D3D12_VIEWPORT get_viewport_d3d12() {
        return m_d3d12.viewport;
    }

private:
    void render_d3d12();
    void render_d3d11();

    struct {
        std::array<std::unique_ptr<d3d12::CommandContext>, 3> command_contexts{};
        std::array<std::unique_ptr<d3d12::TextureContext>, 3> backbuffers{}; // For the RTV
        std::vector<D3D12RenderWorkFn> render_work{};
        std::mutex render_work_mtx{};

        DirectX::RenderTargetState default_rt_state{};

        D3D12_VIEWPORT viewport{};
    } m_d3d12;
};