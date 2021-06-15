#pragma once

#include "Mod.hpp"

class REManagedObject;

class SceneMods : public Mod {
public:
    SceneMods() = default;

    std::string_view get_name() const override { return "Scene"; }

    std::optional<std::string> on_initialize() override;

    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    void on_frame() override;
    void on_draw_ui() override;

private:
    const ModKey::Ptr m_timescale_toggle_key{ModKey::create(generate_name("TimeScaleToggleKey"))};
    const ModKey::Ptr m_timescale_continuous_key{ ModKey::create(generate_name("TimeScaleContinuousKey")) };
    const ModToggle::Ptr m_set_timescale{ModToggle::create(generate_name("TimeScaleEnabled"))};
    const ModSlider::Ptr m_timescale{ModSlider::create(generate_name("TimeScale"), -5.0f, 5.0f, 1.0f)};

    ValueList m_options{
        *m_timescale_toggle_key,
        *m_timescale_continuous_key,
        *m_set_timescale,
        *m_timescale
    };

    bool m_was_continuous_down{ false };
};