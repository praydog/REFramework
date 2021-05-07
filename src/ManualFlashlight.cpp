#include "REFramework.hpp"

#include "ManualFlashlight.hpp"

using namespace utility;

void ManualFlashlight::on_frame() {
    // TODO: Add controller support.
    if (m_key->is_key_down_once()) {
        m_enabled->toggle();
    }
}

void ManualFlashlight::on_draw_ui() {
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);
    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    m_enabled->draw("Enabled");
    m_key->draw("Change Key");

#ifdef RE8
    m_light_enable_shadows->draw("Enable light shadows");
    m_light_radius->draw("Light radius");
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
        // Turn off flashlight when not enabled.
        if (m_player_hand_light != nullptr) {
            m_player_hand_light->IsContinuousOn = false;
        }

        return;
    } 

#ifndef RE8
    if (m_illumination_manager == nullptr) {
        m_illumination_manager = g_framework->get_globals()->get<RopewayIlluminationManager>(game_namespace("IlluminationManager"));
        return;
    }

    // No patch is needed if we are modifying the variables after the transform is updated
    if (transform != m_illumination_manager->ownerGameObject->transform) {
        return;
    }

    m_illumination_manager->shouldUseFlashlight = (int)m_should_pull_out;
    m_illumination_manager->someCounter = (int)m_should_pull_out;
    m_illumination_manager->shouldUseFlashlight2 = m_should_pull_out;
#else
    const auto reset_player_data = [&](REGameObject* new_player = nullptr) {
        m_player = new_player;
        m_player_hand_light = nullptr;
        m_player_hand_ies_light = nullptr;
    };

    // Wait until "AppPropsManager" is valid and cache it off once.
    if (m_props_manager == nullptr) {
        reset_player_data();

        m_props_manager = g_framework->get_globals()->get<AppPropsManager>(game_namespace("PropsManager"));
        if (m_props_manager == nullptr) {
            return;
        }
    }

    const auto player = m_props_manager->player;
    if (player == nullptr) {
        reset_player_data();
        return;
    }

    // Only update when it's the "pl1000" transform.
    const auto player_transform = player->transform;
    if (player_transform == nullptr || transform != player_transform) {
        return;
    }

    // TODO: Find a better way to do this (figure out when the player is actually reset).
    if (m_player != player) {
        reset_player_data(player);
    }

    // Wait until "AppPlayerHandLight" is valid...
    if (m_player_hand_light == nullptr) {
        m_player_hand_light = re_component::find<AppPlayerHandLight>(player_transform, game_namespace("PlayerHandLight"));
        if (m_player_hand_light == nullptr) {
            return;
        }
    }
    
    // Wait until the "IESLight" pointer inside "AppHandLightPowerController" is valid...
    if (m_player_hand_ies_light == nullptr) {
        if (const auto light_power = m_player_hand_light->handLightPowerController; light_power != nullptr) {
            if (const auto ies_light = light_power->renderIESLight; ies_light != nullptr) {
                m_player_hand_ies_light = ies_light;
            }
        }

        if (m_player_hand_ies_light == nullptr) {
            return;
        }
    }

    m_player_hand_light->IsContinuousOn = true;

    m_player_hand_ies_light->ShadowEnable = m_light_enable_shadows->value();
    m_player_hand_ies_light->Radius = m_light_radius->value();
#endif
}
