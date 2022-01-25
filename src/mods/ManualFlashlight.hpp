#pragma once

#include "utility/Patch.hpp"

#include "Mod.hpp"

#ifdef RE8
class AppPlayerHandLight2 : public AppBehaviorApp {
public:
    char pad_0050[1];                                      // 0x0050
	bool IsContinuousOn;                                   // 0x0051
	char pad_0052[6];                                      // 0x0052
	AppHandLightPowerController *handLightPowerController; // 0x0058
	char pad_0060[4];                                      // 0x0060
	int32_t EnterHandLightPowerOnZoneCount;                // 0x0064
};
#endif

// Original founder (RE2): SkacikPL (https://github.com/SkacikPL)
// Recreated in REFramework
class ManualFlashlight : public Mod {
public:
    static ManualFlashlight* g_manual_flashlight;

public:
    ManualFlashlight();
    
    void toggle_flashlight();
    
    std::string_view get_name() const override { return "ManualFlashlight"; }

    void on_frame() override;
    void on_draw_ui() override;

    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    void on_update_transform(RETransform* transform) override;

private:
    // keys need to be directinput unfortunately. window messages for windows virtual keys get blocked
    const ModKey::Ptr m_key{ ModKey::create(generate_name("Key"), DIK_F) };
    const ModToggle::Ptr m_enabled{ ModToggle::create(generate_name("Enabled"), false) };

#ifdef RE8
    const ModToggle::Ptr m_light_ignore_power_on_zones{ ModToggle::create(generate_name("IgnorePowerOnZones"), false) };
#endif

    ValueList m_options{
        *m_key,
        *m_enabled,
#ifdef RE8
        *m_light_ignore_power_on_zones,
#endif
    };

#ifndef RE8
    RopewayIlluminationManager* m_illumination_manager{nullptr};
#else
    AppPropsManager* m_props_manager{nullptr};
    REGameObject* m_player{nullptr};
    AppPlayerHandLight2* m_player_hand_light{nullptr};

    int32_t m_light_power_on_zones{ 0 };
#endif

    bool m_wants_flashlight{ false };
    void on_disabled() noexcept;
};