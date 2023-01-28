#pragma once

#include "Mod.hpp"

class Graphics : public Mod {
public:
    std::string_view get_name() const override { return "Graphics"; };

    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    void on_draw_ui() override;
    void on_present() override;

    void on_pre_application_entry(void* entry, const char* name, size_t hash) override;
    void on_application_entry(void* entry, const char* name, size_t hash) override;

    bool on_pre_gui_draw_element(REComponent* gui_element, void* primitive_context) override;
    void on_view_get_size(REManagedObject* scene_view, float* result) override;

private:
    void do_ultrawide_fix();
    void do_ultrawide_fov_restore(bool force = false);
    void set_vertical_fov(bool enable);

    float m_old_fov{90.0f};

    const ModToggle::Ptr m_ultrawide_fix{ ModToggle::create(generate_name("UltrawideFix"), false) };
    const ModToggle::Ptr m_ultrawide_vertical_fov{ ModToggle::create(generate_name("UltrawideFixVerticalFOV"), true) };
    const ModToggle::Ptr m_ultrawide_fov{ ModToggle::create(generate_name("UltrawideFixFOV"), true) };
    const ModSlider::Ptr m_ultrawide_fov_multiplier{ ModSlider::create(generate_name("UltrawideFOVMultiplier"), 0.01f, 3.0f, 0.5f) };
    const ModToggle::Ptr m_disable_gui{ ModToggle::create(generate_name("DisableGUI"), false) };
    const ModToggle::Ptr m_force_render_res_to_window{ ModToggle::create(generate_name("ForceRenderResToWindow"), false) };

    std::optional<std::array<uint32_t, 2>> m_backbuffer_size{};

    ValueList m_options{
        *m_ultrawide_fix,
        *m_ultrawide_vertical_fov,
        *m_ultrawide_fov,
        *m_ultrawide_fov_multiplier,
        *m_disable_gui,
        *m_force_render_res_to_window,
    };
};
