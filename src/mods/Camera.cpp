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
    const auto reset_camera_data = [&](RECamera* new_camera = nullptr) {
        if (m_camera != new_camera) {
            m_camera = new_camera;
            m_tone_map = nullptr;
            m_tone_map_internal = nullptr;
        }

        return m_camera != nullptr;
    };

    const auto reset_player_data = [&](REGameObject* new_player = nullptr) {
        if (m_player != new_player) {
            m_player = new_player;
            m_player_camera_params = nullptr;
        }

        return m_player != nullptr;
    };

    if (!m_enabled->value()) {
        return;
    }

    // Wait until "AppPropsManager" is valid...
    if (m_props_manager == nullptr) {
        m_props_manager = g_framework->get_globals()->get<AppPropsManager>(game_namespace("PropsManager"));
        if (m_props_manager == nullptr) {
            return;
        }
    }

    if (reset_camera_data(m_props_manager->camera)) {
        if (const auto owner = m_camera->ownerGameObject; owner != nullptr && owner->transform != nullptr && owner->transform == transform) {
            on_cam_transform(transform);
        }
    }

    if (reset_player_data(m_props_manager->player)) {
        if (m_player->transform != nullptr && m_player->transform == transform) {
            on_player_transform(transform);
        }
    }
}

void Camera::on_cam_transform(RETransform* transform) noexcept {
    // Wait until "RenderToneMapping"'s internal data is valid...
    if (m_tone_map == nullptr) {
        m_tone_map = re_component::find<RenderToneMapping>(m_camera, "via.render.ToneMapping");
    }

    if (m_tone_map != nullptr && m_tone_map_internal == nullptr) {
        m_tone_map_internal = m_tone_map->toneMappingInternal;
    }

    if (m_disable_vignette->value()) {
        set_vignette_type(via::render::ToneMapping::Vignetting::Disable);
    } 
    else {
        set_vignette_brightness(m_vignette_brightness->value());
    }
}

void Camera::on_player_transform(RETransform* transform) noexcept {
    // Sorry about this...
    const auto get_player_camera_params = [](AppPlayerConfigure* player_configure) -> AppPlayerCameraParameter* {
        if (player_configure == nullptr) {
            return nullptr;
        }

        const auto player_configuration = player_configure->playerConfiguration;
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
    };

    // Wait until "AppPlayerCameraParameter" is valid...
    if (m_player_camera_params == nullptr) {
        m_player_camera_params = get_player_camera_params(re_component::find<AppPlayerConfigure>(transform, game_namespace("PlayerConfigure")));
    }

    set_fov(m_fov->value(), m_fov_aiming->value());
}

void Camera::on_disabled() noexcept {
    set_vignette_type(via::render::ToneMapping::Vignetting::Enable);
    set_vignette_brightness(m_vignette_brightness->default_value());
    set_fov(m_fov->default_value(), m_fov_aiming->default_value());
}

void Camera::set_vignette_type(via::render::ToneMapping::Vignetting value) noexcept
{
    if (m_tone_map_internal == nullptr) {
        return;
    }

    m_tone_map_internal->vignetting_mode = (int32_t)value;
}

void Camera::set_vignette_brightness(float value) noexcept
{
    if (m_tone_map_internal == nullptr) {
        return;
    }

    m_tone_map_internal->update_vignette = true;
    m_tone_map_internal->vignetting_brightness = value;
}

void Camera::set_fov(float fov, float aiming_fov) noexcept
{
    if (m_player_camera_params == nullptr) {
        return;
    }

    m_player_camera_params->DefaultFOV = fov;
    m_player_camera_params->AimmingFOV = aiming_fov;
}

#endif