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

    ValueList m_options{
        *m_enabled,
    };
};