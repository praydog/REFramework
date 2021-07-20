#include "Camera.hpp"

using namespace utility;

#ifdef RE8

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

    if (m_enabled->draw("Enabled") && !m_enabled->value()) {
        on_disabled();
    }

    if (m_disable_vignette->draw("Disable Vignette") && !m_disable_vignette->value()) {
        set_vignette_type(via::render::ToneMapping::Vignetting::Enable);
    }

    m_vignette_brightness->draw("Vignette Brightness");
    m_fov->draw("FOV");
    m_fov_aiming->draw("Aiming FOV");
}

void Camera::on_update_transform(RETransform* transform) {
    if (!m_enabled->value()) {
        return;
    }

    // Cache off "AppPropsManager" once.
    if (m_props_manager == nullptr) {
        m_props_manager = g_framework->get_globals()->get<AppPropsManager>(game_namespace("PropsManager"));
        if (m_props_manager == nullptr) {
            return;
        }
    }

    const auto valid_camera = reset_ptr(m_camera, m_props_manager->camera,
        [&](bool valid) {
            m_tone_map = nullptr;
        }
    );

    const auto valid_player = reset_ptr(m_player, m_props_manager->player,
        [&](bool valid) {
            m_player_configure = nullptr;
        }
    );

    // Run on camera transform.
    if (valid_camera) {
        if (const auto owner = m_camera->ownerGameObject; owner != nullptr && owner->transform != nullptr && owner->transform == transform) {
            on_cam_transform(transform);
        }
    }

    // Run on player transform.
    if (valid_player) {
        if (m_player->transform != nullptr && m_player->transform == transform) {
            on_player_transform(transform);
        }
    }
}

void Camera::on_cam_transform(RETransform* transform) noexcept {
    // Cache off "RenderToneMapping" once (if camera ptr changes, this will be cached again).
    if (m_tone_map == nullptr) {
        m_tone_map = re_component::find<RenderToneMapping>(m_camera, "via.render.ToneMapping");
    }

    m_tone_map_internal = (m_tone_map != nullptr) ? m_tone_map->toneMappingInternal : nullptr;
    
    if (m_disable_vignette->value()) {
        set_vignette_type(via::render::ToneMapping::Vignetting::Disable);
    } 
    else {
        set_vignette_brightness(m_vignette_brightness->value());
    }
}

void Camera::on_player_transform(RETransform* transform) noexcept {
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
}

void Camera::on_disabled() noexcept {
    set_vignette_type(via::render::ToneMapping::Vignetting::Enable);
    set_vignette_brightness(m_vignette_brightness->default_value());
    set_fov(m_fov->default_value(), m_fov_aiming->default_value());
}

void Camera::set_vignette_type(via::render::ToneMapping::Vignetting value) noexcept {
    if (m_tone_map == nullptr) {
        return;
    }

    utility::re_managed_object::call_method(m_tone_map, "setVignetting", value);
}

void Camera::set_vignette_brightness(float value) noexcept {
    if (m_tone_map == nullptr) {
        return;
    }

    utility::re_managed_object::call_method(m_tone_map, "setVignettingBrightness", (double)value);
}

void Camera::set_fov(float fov, float aiming_fov) noexcept {
    if (m_player_camera_params == nullptr) {
        return;
    }
    
    m_player_camera_params->DefaultFOV = fov;
    m_player_camera_params->AimmingFOV = aiming_fov;
}

#endif