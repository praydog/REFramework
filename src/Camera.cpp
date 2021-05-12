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
        if (m_tone_map_internal != nullptr) {
            m_tone_map_internal->Vignetting = (int32_t)via::render::ToneMapping::Vignetting::Enable;
        }
    }

    m_fov->draw("FOV");
    m_fov_aiming->draw("Aiming FOV");
}

void Camera::on_update_transform(RETransform* transform) {
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

    // Run on camera transform.
    if (const auto cam = m_props_manager->camera; cam != nullptr) {
        if (const auto cam_owner = cam->ownerGameObject; cam_owner != nullptr && cam_owner->transform != nullptr && cam_owner->transform == transform) {
            // Check against cached camera pointer.
            if (m_cam != cam) {
                m_cam = cam;
                m_tone_map_internal = nullptr;
            }

            on_cam_transform(transform);
        }
    }

    // Run on player transform.
    if (const auto player = m_props_manager->player; player != nullptr) {
        if (player->transform != nullptr && player->transform == transform) {
            // Check against cached player pointer.
            if (m_player != player) {
                m_player = player;
                m_player_camera_params = nullptr;
            }

            on_player_transform(transform);
        }
    }
}

void Camera::on_cam_transform(RETransform* transform) noexcept {
    if (m_disable_vignette->value()) {
        // Wait until "RenderToneMapping"'s internal data is valid...
        if (m_tone_map_internal == nullptr) {
            if (const auto tone_map = re_component::find<RenderToneMapping>(m_cam, "via.render.ToneMapping"); tone_map != nullptr) {
                m_tone_map_internal = tone_map->toneMappingInternal;
            }
        }

        // Set vignette.
        if (m_tone_map_internal != nullptr) {
            m_tone_map_internal->Vignetting = (int32_t)via::render::ToneMapping::Vignetting::Disable;
        }
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

    if (m_player_camera_params != nullptr) {
        m_player_camera_params->DefaultFOV = m_fov->value();
        m_player_camera_params->AimmingFOV = m_fov_aiming->value();
    }
}

void Camera::on_disabled() noexcept {
    if (m_tone_map_internal != nullptr) {
        m_tone_map_internal->Vignetting = (int32_t)via::render::ToneMapping::Vignetting::Enable;
    }

    if (m_player_camera_params != nullptr) {
        m_player_camera_params->DefaultFOV = m_fov->default_value();
        m_player_camera_params->AimmingFOV = m_fov_aiming->default_value();
    }
}

#endif