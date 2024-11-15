#pragma once

#include "Tool.hpp"

#include <../../directxtk12-src/Inc/SimpleMath.h>
#include <../../directxtk12-src/Inc/CommonStates.h>
#include <../../directxtk12-src/Inc/Effects.h>
#include <../../directxtk12-src/Inc/GeometricPrimitive.h>
#include <../../directxtk12-src/Inc/SpriteFont.h>
#include <../../directxtk12-src/Inc/DescriptorHeap.h>

#include "../vr/d3d12/TextureContext.hpp"

class GameObjectsDisplay : public Tool {
public:
    std::string_view get_name() const override {
        return "GameObjectsDisplay";
    }
    
    std::optional<std::string> on_initialize_d3d_thread() override;
    std::optional<std::string> on_initialize() override;
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

    enum FontHeapIndices {
        SpriteFont_Data = 0,
        FontHeap_Count
    };

    struct {
        std::unique_ptr<d3d12::TextureContext> text_texture{};

        std::unique_ptr<DirectX::DX12::BasicEffect> effect{};
        std::unique_ptr<DirectX::DX12::BasicEffect> effect_no_tex{};
        std::unique_ptr<DirectX::DX12::SpriteFont> font{};
        std::unique_ptr<DirectX::DX12::GeometricPrimitive> quad{}; // For rendering the text in 3D space

        std::unique_ptr<DirectX::DescriptorHeap> font_heap{};
        std::unique_ptr<DirectX::DX12::SpriteBatch> sprite_batch{};
        std::unique_ptr<DirectX::DX12::CommonStates> states{};
    } m_d3d12;

private:
    const ModToggle::Ptr m_enabled{ ModToggle::create(generate_name("Enabled")) };
    const ModSlider::Ptr m_max_distance{ ModSlider::create(generate_name("MaxDistance"), 0.0f, 1000.0f, 0.0f) };

    float m_effect_alpha{ 0.5f };
    bool m_effect_dirty{ false };
    bool m_needs_d3d_init{ false };

    ValueList m_options{
        *m_enabled,
        *m_max_distance
    };
};