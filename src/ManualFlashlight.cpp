#include "REFramework.hpp"

#include "ManualFlashlight.hpp"

void ManualFlashlight::on_frame() {
    // TODO: Add controller support.
    if (m_key->is_key_down_once()) {
        m_should_pull_out = !m_should_pull_out;
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
    const auto clear_player_data = [&]() {
        m_player = nullptr;
        m_player_hand_light = nullptr;
    };

    if ((m_props_manager = g_framework->get_globals()->get<AppPropsManager>(game_namespace("PropsManager"))) == nullptr) {
        clear_player_data();
        return;
    }

    const auto player = m_props_manager->player;
    if (player == nullptr) {
        clear_player_data();
        return;
    }

    const auto player_transform = player->transform;
    if (player_transform == nullptr || transform != player_transform) {
        return;
    }

    // NOTE: This makes me a bit uneasy because if a pointer is reused then this logic will fail.
    // Surely theres a better way to check if the level/game session has changed, right?
    if (player != m_player) {
        m_player = player;
    }

    // Wait until "AppPlayerHandLight" is valid...
    if ((m_player_hand_light = utility::re_component::find<AppPlayerHandLight>(player_transform, game_namespace("PlayerHandLight"))) == nullptr) {
        return;
    }

    m_player_hand_light->IsContinuousOn = m_should_pull_out;

    // Light control.
    if (const auto light_power = m_player_hand_light->handLightPowerController) {
        if (const auto light = light_power->renderIESLight) {
            light->ShadowEnable = m_light_enable_shadows->value();
            light->Radius = m_light_radius->value();
        }
    }
#endif
}
