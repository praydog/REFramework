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
}
