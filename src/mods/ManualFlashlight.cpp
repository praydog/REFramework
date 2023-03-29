#include "REFramework.hpp"

#include "ManualFlashlight.hpp"

using namespace utility;

ManualFlashlight* ManualFlashlight::g_manual_flashlight = nullptr;

ManualFlashlight::ManualFlashlight() {    
    g_manual_flashlight = this;
}

void ManualFlashlight::toggle_flashlight() {
    m_wants_flashlight = !m_wants_flashlight;
}

void ManualFlashlight::on_frame() {
    // TODO: Add controller support.
    if (m_key->is_key_down_once()) {
        toggle_flashlight();
    }
}

void ManualFlashlight::on_draw_ui() {
    ImGui::SetNextItemOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);
    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    if (m_enabled->draw("Enabled") && !m_enabled->value()) {
        on_disabled();
    }

    m_key->draw("Change Key");

#ifdef RE8
    m_light_ignore_power_on_zones->draw("Ignore Light Power On Zones");

    ImGui::Text("Light power on zones: %i\n", m_light_power_on_zones);
#endif
}

void ManualFlashlight::on_config_load(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }
}

void ManualFlashlight::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}

void ManualFlashlight::on_update_transform(RETransform* transform) {
    if (!m_enabled->value()) {
        return;
    }

#ifndef RE8
    if (m_illumination_manager == nullptr) {
        m_illumination_manager = reframework::get_globals()->get<RopewayIlluminationManager>(game_namespace("IlluminationManager"));
        if (m_illumination_manager == nullptr) {
            return;
        }
    }

    if (transform != m_illumination_manager->ownerGameObject->transform) {
        return;
    }

    m_illumination_manager->shouldUseFlashlight = (int32_t)m_wants_flashlight;
    m_illumination_manager->someCounter = (int32_t)m_wants_flashlight;
    m_illumination_manager->shouldUseFlashlight2 = m_wants_flashlight;
#else
    const auto reset_player_data = [&](REGameObject* new_player = nullptr) {
        m_player = new_player;
        m_player_hand_light = nullptr;
    };

    // Cache off "AppPropsManager" once.
    if (m_props_manager == nullptr) {
        m_props_manager = reframework::get_globals()->get<AppPropsManager>(game_namespace("PropsManager"));
        if (m_props_manager == nullptr) {
            return;
        }
    }

    const auto player = sdk::call_object_func_easy<REGameObject*>(m_props_manager, "get_Player");
    if (player == nullptr) {
        reset_player_data();
        return;
    }

    const auto player_transform = player->transform;
    if (player_transform == nullptr || transform != player_transform) {
        return;
    }

    if (m_player != player) {
        reset_player_data(player);
    }

    // Wait until "AppPlayerHandLight" is valid...
    if (m_player_hand_light == nullptr) {
        m_player_hand_light = re_component::find<AppPlayerHandLight2>(player_transform, game_namespace("PlayerHandLight"));
        if (m_player_hand_light == nullptr) {
            return;
        }
    }

    // TODO: Check for scene change and cache this stuff, maybe theres a power on zone count in the current scene somewhere?
    // Also, sometimes the game will force this on before it can be set here, making your flashlight toggle on.
    // I don't really care to find a way around this since it's user-friendly anyway... Some areas are pitch-black.
    auto light_power_on_zones = sdk::get_object_field<int32_t>(m_player_hand_light, "EnterHandLightPowerOnZoneCount");

    if (light_power_on_zones != nullptr) {
        m_light_power_on_zones = *light_power_on_zones;
        
        if (m_light_ignore_power_on_zones->value()) {
            *light_power_on_zones = 0;
        }
    }

    auto is_continous_on = sdk::get_object_field<bool>(m_player_hand_light, "IsContinuousOn");

    if (is_continous_on != nullptr) {
        *is_continous_on = m_wants_flashlight;
    }
#endif
}

void ManualFlashlight::on_disabled() noexcept {
#ifndef RE8
    if (m_illumination_manager != nullptr) {
        m_illumination_manager->shouldUseFlashlight = 0;
        m_illumination_manager->someCounter = 0;
        m_illumination_manager->shouldUseFlashlight2 = false;
    }
#else
    if (m_player_hand_light != nullptr) {
        *sdk::get_object_field<bool>(m_player_hand_light, "IsContinuousOn") = false;
    }
#endif
}