#include <sdk/SceneManager.hpp>
#include <sdk/RETypeDB.hpp>

#include "Camera.hpp"

using namespace utility;

void Camera::on_config_load(const Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }
}

void Camera::on_config_save(Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}

void Camera::on_draw_ui() {
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);
    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    ImGui::TextWrapped("Make sure to tick \"Enabled\" for any of the below settings to take effect.");

    if (m_enabled->draw("Enabled") && !m_enabled->value()) {
        on_disabled();
    }

    if (m_disable_vignette->draw("Disable Vignette") && !m_disable_vignette->value()) {
        set_vignette_type(via::render::ToneMapping::Vignetting::Enable);
    }

    // RE8 and above have vignetting brightness
#if TDB_VER >= 69
    m_vignette_brightness->draw("Vignette Brightness");
#endif

#ifdef RE8
    m_fov->draw("FOV");
    m_fov_aiming->draw("Aiming FOV");
#endif
}

void Camera::on_update_transform(RETransform* transform) {
#ifdef RE8
    if (!m_enabled->value()) {
        return;
    }

    // Cache off "AppPropsManager" once.
    if (m_props_manager == nullptr) {
        m_props_manager = reframework::get_globals()->get<AppPropsManager>(game_namespace("PropsManager"));
        if (m_props_manager == nullptr) {
            return;
        }
    }

    const auto valid_player = reset_ptr(m_player, m_props_manager->player,
        [&](bool valid) {
            m_player_configure = nullptr;
        }
    );

    // Run on player transform.
    if (valid_player) {
        if (m_player->transform != nullptr && m_player->transform == transform) {
            on_player_transform(transform);
        }
    }
#endif
}

void Camera::on_application_entry(void* entry, const char* name, size_t hash) {
    if (!m_enabled->value()) {
        return;
    }

    if (hash == "LockScene"_fnv) {
        const auto valid_camera = reset_ptr(m_camera, sdk::get_primary_camera(),
            [&](bool valid) {
                m_tone_map = nullptr;
            }
        );

        if (valid_camera) {
            if (const auto owner = m_camera->ownerGameObject; owner != nullptr && owner->transform != nullptr) {
                update_vignetting();
            }
        }
    }
}

void Camera::update_vignetting() noexcept {
    // Cache off "RenderToneMapping" once (if camera ptr changes, this will be cached again).
    if (m_tone_map == nullptr) {
        m_tone_map = re_component::find<RenderToneMapping>(m_camera, "via.render.ToneMapping");
    }

    m_tone_map_internal = (m_tone_map != nullptr) ? m_tone_map->toneMappingInternal : nullptr;
    
    if (m_disable_vignette->value()) {
        set_vignette_type(via::render::ToneMapping::Vignetting::Disable);
    } 
#if TDB_VER >= 69
    else {
        set_vignette_brightness(m_vignette_brightness->value());
    }
#endif
}

void Camera::on_player_transform(RETransform* transform) noexcept {
#ifdef RE8
    // Cache off "AppPlayerConfigure" once (if player ptr changes, this will be cached again).
    if (m_player_configure == nullptr) {
        m_player_configure = re_component::find<AppPlayerConfigure>(transform, game_namespace("PlayerConfigure"));
    }

    if (m_player_configure != nullptr) {
        m_player_camera_params = [&]() -> AppPlayerCameraParameter* {
            const auto player_configuration = m_player_configure->playerConfiguration;
            if (player_configuration == nullptr) {
                return nullptr;
            }

            const auto cam_configuration = player_configuration->cameraConfiguration;
            if (cam_configuration == nullptr) {
                return nullptr;
            }

            const auto player_cam_configuration = cam_configuration->playerCameraConfiguration;
            if (player_cam_configuration == nullptr) {
                return nullptr;
            }

            const auto player_cam_configuration_base = player_cam_configuration->playerCameraConfigurationBase;
            if (player_cam_configuration_base == nullptr) {
                return nullptr;
            }

            return player_cam_configuration_base->playerCameraParameter;
        }();
    }

    set_fov(m_fov->value(), m_fov_aiming->value());
#endif
}

void Camera::on_disabled() noexcept {
    set_vignette_type(via::render::ToneMapping::Vignetting::Enable);
    set_vignette_brightness(m_vignette_brightness->default_value());

#ifdef RE8
    set_fov(m_fov->default_value(), m_fov_aiming->default_value());
#endif
}

void Camera::set_vignette_type(via::render::ToneMapping::Vignetting value) noexcept {
    if (m_tone_map == nullptr) {
        return;
    }

    static auto set_vignetting_method = sdk::find_method_definition("via.render.ToneMapping", "setVignetting");

    if (set_vignetting_method != nullptr) {
        set_vignetting_method->call<void*>(sdk::get_thread_context(), m_tone_map, value);
    }
}

void Camera::set_vignette_brightness(float value) noexcept {
    if (m_tone_map == nullptr) {
        return;
    }

    // Not a TDB method.
    utility::re_managed_object::call_method((::REManagedObject*)m_tone_map, "setVignettingBrightness", (double)value);
}

void Camera::set_fov(float fov, float aiming_fov) noexcept {
#ifdef RE8
    if (m_player_camera_params == nullptr) {
        return;
    }
    
    m_player_camera_params->DefaultFOV = fov;
    m_player_camera_params->AimmingFOV = aiming_fov;
#endif
}