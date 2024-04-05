#pragma once

#include "Tool.hpp"

class GameObjectsDisplay : public Tool {
public:
    std::string_view get_name() const override {
        return "GameObjectsDisplay";
    }
    
    std::optional<std::string> on_initialize() override;
    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    void on_draw_dev_ui() override;
    void on_frame() override;

private:
    const ModToggle::Ptr m_enabled{ ModToggle::create(generate_name("Enabled")) };
    const ModSlider::Ptr m_max_distance{ ModSlider::create(generate_name("MaxDistance"), 0.0f, 1000.0f, 0.0f) };

    ValueList m_options{
        *m_enabled,
        *m_max_distance
    };
};