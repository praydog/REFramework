#include "FreeCam.hpp"

void FreeCam::on_config_load(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }
}

void FreeCam::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}

void FreeCam::on_frame() {
    if (m_toggle_key->is_key_down_once()) {
        m_enabled->toggle();
        m_first_time = true;
    }

    if (m_lock_camera_key->is_key_down_once()) {
        m_lock_camera->toggle();
    }

    if (m_disable_movement_key->is_key_down_once()) {
        m_disable_movement->toggle();
    }
}

void FreeCam::on_draw_ui() {
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);

    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    if (m_enabled->draw("Enabled")) {
        m_first_time = true;
    }

    ImGui::SameLine();
    m_lock_camera->draw("Lock Position");
    m_disable_movement->draw("Disable Character Movement");

    m_toggle_key->draw("Toggle Key");
    m_lock_camera_key->draw("Lock Position Toggle Key");
    m_disable_movement_key->draw("Disable Movement Toggle Key");

    m_speed->draw("Speed");
}

void FreeCam::on_update_transform(RETransform* transform) {
    if (!m_enabled->value() && !m_first_time) {
        return;
    }

    if (!update_pointers()) {
        return;
    }

    if (m_camera_system->mainCamera == nullptr || transform != m_camera_system->mainCamera->ownerGameObject->transform) {
        return;
    }

    auto condition = m_survivor_manager->playerCondition;

    if (m_first_time || condition == nullptr) {
        m_last_camera_matrix = transform->worldTransform;

        if (condition != nullptr && condition->actionOrderer != nullptr) {
            condition->actionOrderer->enabled = true;
            m_first_time = false;
        }
        
        return;
    }

    auto orderer = condition->actionOrderer;

    if (orderer != nullptr) {
        orderer->enabled = !m_disable_movement->value();
    }

    auto controller = m_camera_system->cameraController;

    if (controller == nullptr) {
        return;
    }

    // despite the name, it works with the keyboard
    auto left_analog = utility::re_managed_object::get_field<RopewayInputSystemAnalogStick*>(m_input_system, "_LStick");

    if (left_analog == nullptr) {
        return;
    }

    // Update wanted camera position
    if (!m_lock_camera->value()) {
        // Move direction
        // It's not a Vector2f because via.vec2 is not actually 8 bytes, we don't want stack corruption to occur.
        auto axis = utility::re_managed_object::get_field<Vector3f>(left_analog, "Axis");
        auto dir = Vector4f{ axis.x, 0.0f, axis.y * -1.0f, 0.0f };

        auto delta = utility::re_component::get_delta_time(transform);

        // Use controller rotation instead of camera rotation as it's accurate, will work in cutscenes.
        auto new_pos = m_last_camera_matrix[3] + Matrix4x4f{ *(glm::quat*)&controller->worldRotation } * dir * m_speed->value() * delta;

        // Keep track of the rotation if we want to lock the camera
        m_last_camera_matrix = transform->worldTransform;
        m_last_camera_matrix[3] = new_pos;
    }

    transform->worldTransform = m_last_camera_matrix;
    transform->position = m_last_camera_matrix[3];
    controller->worldPosition = m_last_camera_matrix[3];
}

bool FreeCam::update_pointers() {
    if (m_camera_system == nullptr || m_input_system == nullptr || m_survivor_manager == nullptr) {
        auto& globals = *g_framework->get_globals();
        m_camera_system = globals.get<RopewayCameraSystem>(game_namespace("camera.CameraSystem"));
        m_input_system = globals.get<RopewayInputSystem>(game_namespace("InputSystem"));
        m_survivor_manager = globals.get<RopewaySurvivorManager>(game_namespace("SurvivorManager"));
        return false;
    }

    return true;
}
