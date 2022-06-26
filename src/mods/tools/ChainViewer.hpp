#pragma once

#include <chrono>

#include "Tool.hpp"

class ChainViewer : public Tool {
public:
    std::string_view get_name() const override {
        return "ChainViewer";
    }
    
    std::optional<std::string> on_initialize() override;
    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    void on_draw_dev_ui() override;
    void on_frame() override;

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