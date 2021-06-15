#include "sdk/ReClass.hpp"
#include "sdk/SceneManager.hpp"

#include "Scene.hpp"

std::optional<std::string> SceneMods::on_initialize() {
    return Mod::on_initialize();
}

void SceneMods::on_config_load(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }
}

void SceneMods::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}

void SceneMods::on_frame() {
    if (m_set_timescale->value()) {
        sdk::set_timescale(m_timescale->value());
    }

    bool set_timescale = false;

    if (m_timescale_continuous_key->is_key_down()) {
        set_timescale = true;
        m_was_continuous_down = true;
        m_set_timescale->value() = true;
    } else if (m_was_continuous_down) {
        set_timescale = true;
        m_set_timescale->value() = false;
        m_was_continuous_down = false;
    }

    if (m_timescale_toggle_key->is_key_down_once()) {
        set_timescale = true;
        m_set_timescale->toggle();
    }

    if (set_timescale && !m_set_timescale->value()) {
        sdk::set_timescale(1.0f);
    }
}

void SceneMods::on_draw_ui() {
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);

    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    m_timescale_toggle_key->draw("Timescale (Toggle) Key");
    m_timescale_continuous_key->draw("Timescale (Continuous) Key");

    m_timescale->draw("");
    ImGui::SameLine();

    if (m_set_timescale->draw("Timescale")) {
        if (!m_set_timescale->value()) {
            sdk::set_timescale(1.0f);
        }
    }
}
