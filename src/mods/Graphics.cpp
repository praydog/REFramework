#include <sdk/SceneManager.hpp>

#include "VR.hpp"
#include "Graphics.hpp"

void Graphics::on_config_load(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }
}

void Graphics::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}

void Graphics::on_draw_ui() {
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);
    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    if (m_ultrawide_fix->draw("Ultrawide/Aspect Ratio Fix") && m_ultrawide_fix->value() == false) {
        set_vertical_fov(false);
        do_ultrawide_fov_restore(true);
    }

    if (m_ultrawide_fix->value()) {
        m_ultrawide_vertical_fov->draw("Ultrawide: Enable Vertical FOV");
        m_ultrawide_fov->draw("Ultrawide: Override FOV");
        m_ultrawide_fov_multiplier->draw("Ultrawide: FOV Multiplier");
    }
}

void Graphics::on_pre_application_entry(void* entry, const char* name, size_t hash) {
    // To fix the world-space GUI icons.
    if (hash == "UpdateBehavior"_fnv) {
        do_ultrawide_fix();
    }

    if (hash == "UnlockScene"_fnv) {
        do_ultrawide_fov_restore();
    }
}

void Graphics::on_application_entry(void* entry, const char* name, size_t hash) {
    if (hash == "UpdateBehavior"_fnv) {
        do_ultrawide_fov_restore();
    }

    // To actually fix the rendering.
    if (hash == "LockScene"_fnv) {
        do_ultrawide_fix();
    }
}

void Graphics::do_ultrawide_fix() {
    if (!m_ultrawide_fix->value()) {
        return;
    }

    // No need to perform ultrawide fix if VR is running.
    if (VR::get()->is_hmd_active()) {
        return;
    }

    set_vertical_fov(m_ultrawide_vertical_fov->value());

    static auto via_scene_view = sdk::find_type_definition("via.SceneView");
    static auto set_display_type_method = via_scene_view->get_method("set_DisplayType");

    auto main_view = sdk::get_main_view();

    if (main_view == nullptr) {
        return;
    }

    if (set_display_type_method != nullptr) {
        set_display_type_method->call(sdk::get_thread_context(), main_view, via::DisplayType::Fit);
    }
}

void Graphics::do_ultrawide_fov_restore(bool force) {
    if (!m_ultrawide_fix->value() && !force) {
        return;
    }

    // No need to perform ultrawide fix if VR is running.
    if (VR::get()->is_hmd_active()) {
        return;
    }

    static auto via_camera = sdk::find_type_definition("via.Camera");
    static auto set_fov_method = via_camera->get_method("set_FOV");

    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    if (set_fov_method != nullptr) {
        if (m_ultrawide_fov->value()) {
            set_fov_method->call(sdk::get_thread_context(), camera, m_old_fov);
        }
    }
}

void Graphics::set_vertical_fov(bool enable) {
    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    static auto via_camera = sdk::find_type_definition("via.Camera");
    static auto set_vertical_enable_method = via_camera->get_method("set_VerticalEnable");
    static auto get_fov_method = via_camera->get_method("get_FOV");
    static auto set_fov_method = via_camera->get_method("set_FOV");

    if (set_vertical_enable_method != nullptr) {
        set_vertical_enable_method->call(sdk::get_thread_context(), camera, enable);
    }

    if (m_ultrawide_fov->value() && get_fov_method != nullptr && set_fov_method != nullptr) {
        const auto hfov = get_fov_method->call<float>(sdk::get_thread_context(), camera);
        const auto vfov = std::clamp(hfov * m_ultrawide_fov_multiplier->value(), 0.01f, 179.9f);

        m_old_fov = hfov;
        set_fov_method->call(sdk::get_thread_context(), camera, vfov);
    }
}