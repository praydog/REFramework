#pragma once

#include "utility/Patch.hpp"

#include "Mod.hpp"

// Original founder (RE2): SkacikPL (https://github.com/SkacikPL)
// Recreated in REFramework
class ManualFlashlight : public Mod {
public:
    std::string_view get_name() const override { return "ManualFlashlight"; }

    void on_frame() override;
    void on_draw_ui() override;

    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    void on_update_transform(RETransform* transform) override;

private:
#ifndef RE8
    RopewayIlluminationManager* m_illumination_manager{ nullptr };
#else
    AppPropsManager* m_props_manager{ nullptr };
    REGameObject* m_player{ nullptr };
    AppPlayerHandLight* m_player_hand_light{ nullptr };
    IESLight* m_player_hand_ies_light{ nullptr };
#endif

    bool m_toggle_off{ false };

    // keys need to be directinput unfortunately. window messages for windows virtual keys get blocked
    const ModKey::Ptr m_key{ ModKey::create(generate_name("Key"), DIK_F) };
    const ModToggle::Ptr m_enabled{ ModToggle::create(generate_name("Enabled"), false) };

#ifdef RE8
    const ModToggle::Ptr m_light_enable_shadows{ ModToggle::create(generate_name("LightShadows"), true) };
    const ModSlider::Ptr m_light_radius{ ModSlider::create(generate_name("LightRadius"), 0.0f, 50.0f, 1.0f) };
#endif

    ValueList m_options{
        *m_key,
        *m_enabled,
#ifdef RE8
        *m_light_enable_shadows,
        *m_light_radius,
#endif
    };
};