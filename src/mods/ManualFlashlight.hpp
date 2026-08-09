#pragma once

#include "utility/Patch.hpp"

#include "Mod.hpp"
#include <sdk/GameIdentity.hpp>

#ifdef REFRAMEWORK_UNIVERSAL
class AppPlayerHandLight2 : public AppBehaviorApp {
public:
    char pad_0050[1];
    bool IsContinuousOn;
    char pad_0052[6];
    AppHandLightPowerController *handLightPowerController;
    char pad_0060[4];
    int32_t EnterHandLightPowerOnZoneCount;
};
#else
    #ifdef RE8
    class AppPlayerHandLight2 : public AppBehaviorApp {
    public:
        char pad_0050[1];
        bool IsContinuousOn;
        char pad_0052[6];
        AppHandLightPowerController *handLightPowerController;
        char pad_0060[4];
        int32_t EnterHandLightPowerOnZoneCount;
    };
    #endif
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
    const ModKey::Ptr m_key{ ModKey::create(generate_name("Key_V2"), VkKeyScan('f')) };
    const ModToggle::Ptr m_enabled{ ModToggle::create(generate_name("Enabled"), false) };

#ifdef REFRAMEWORK_UNIVERSAL
    const ModToggle::Ptr m_light_ignore_power_on_zones{ ModToggle::create(generate_name("IgnorePowerOnZones"), false) };
#else
    #ifdef RE8
    const ModToggle::Ptr m_light_ignore_power_on_zones{ ModToggle::create(generate_name("IgnorePowerOnZones"), false) };
    #endif
#endif

    ValueList m_options{
        *m_key,
        *m_enabled,
#ifdef REFRAMEWORK_UNIVERSAL
        *m_light_ignore_power_on_zones,
#else
    #ifdef RE8
        *m_light_ignore_power_on_zones,
    #endif
#endif
    };

#ifdef REFRAMEWORK_UNIVERSAL
    RopewayIlluminationManager* m_illumination_manager{nullptr};
    AppPropsManager* m_props_manager{nullptr};
    REGameObject* m_player{nullptr};
    AppPlayerHandLight2* m_player_hand_light{nullptr};
    int32_t m_light_power_on_zones{ 0 };
#else
    #ifndef RE8
    RopewayIlluminationManager* m_illumination_manager{nullptr};
    #else
    AppPropsManager* m_props_manager{nullptr};
    REGameObject* m_player{nullptr};
    AppPlayerHandLight2* m_player_hand_light{nullptr};
    int32_t m_light_power_on_zones{ 0 };
    #endif
#endif

    bool m_wants_flashlight{ false };
    void on_disabled() noexcept;
};
