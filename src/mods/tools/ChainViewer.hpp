#pragma once

#include <chrono>

#include <../../directxtk12-src/Inc/SimpleMath.h>
#include <../../directxtk12-src/Inc/CommonStates.h>
#include <../../directxtk12-src/Inc/Effects.h>
#include <../../directxtk12-src/Inc/GeometricPrimitive.h>

#include "../mods/vr/d3d12/CommandContext.hpp"
#include "../mods/vr/d3d12/TextureContext.hpp"
#include "../mods/vr/d3d12/ComPtr.hpp"

#include "Tool.hpp"

class ChainViewer : public Tool {
public:
    std::string_view get_name() const override {
        return "ChainViewer";
    }
    
    std::optional<std::string> on_initialize() override;
    std::optional<std::string> on_initialize_d3d_thread() override;
    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    void on_device_reset() override {
        m_needs_d3d_init = true;
    }

    void on_draw_dev_ui() override;
    void on_frame() override;
    void on_present() override;

private:
    std::optional<std::string> initialize_d3d_resources();

    struct {
        std::unique_ptr<DirectX::DX12::BasicEffect> effect{};
        std::unique_ptr<DirectX::DX12::GeometricPrimitive> cylinder{};
        std::unique_ptr<DirectX::DX12::GeometricPrimitive> sphere{};

        bool initialized{false};
    } m_d3d12;

    float m_effect_alpha{0.5f};
    bool m_effect_dirty{false};
    bool m_needs_d3d_init{true};

private:
    const ModToggle::Ptr m_enabled{ ModToggle::create(generate_name("Enabled")) };

    struct DeltaTime {
        void update() {
            const auto now = std::chrono::high_resolution_clock::now();
            const auto delta = now - m_last_time;

            m_last_calculated_delta = std::chrono::duration_cast<std::chrono::duration<float>>(delta).count();
            m_last_time = now;
        }

        float get() const {   
            return m_last_calculated_delta;
        }

        operator float() const {
            return get();
        }

        std::chrono::high_resolution_clock::time_point m_last_time;
        float m_last_calculated_delta{};
    } m_delta_time;

    float m_pulse_time{};

    ValueList m_options{
        *m_enabled,
    };
};