#include "sdk/REMath.hpp"

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

#ifdef RE8
    m_rotation_speed->draw("Rotation Speed");
#endif
}

void FreeCam::on_update_transform(RETransform* transform) {
    if (!m_enabled->value() && !m_first_time) {
        return;
    }

    if (!update_pointers()) {
        return;
    }

#ifndef RE8
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

    controller->worldPosition = m_last_camera_matrix[3];
#else
    if (m_props_manager->camera == nullptr || transform != m_props_manager->camera->ownerGameObject->transform) {
        return;
    }

    auto camera = m_props_manager->camera;

    // Controllers only for now
    if (m_pad_manager->pad1 == nullptr || m_pad_manager->pad1->device == nullptr) {
        m_first_time = true;
        return;
    }

    if (m_first_time) {
        m_last_camera_matrix = transform->worldTransform;
        m_first_time = false;

        m_custom_angles = utility::math::euler_angles(glm::extractMatrixRotation(transform->worldTransform));
        //m_custom_angles[1] *= -1.0f;
        //m_custom_angles[1] += glm::radians(180.0f);

        utility::math::fix_angles(m_custom_angles);

        return;
    }

    auto device = m_pad_manager->pad1->device;

    // Update wanted camera position
    if (!m_lock_camera->value()) {
        // Move direction
        // It's not a Vector2f because via.vec2 is not actually 8 bytes, we don't want stack corruption to occur.
        auto axis_l = *utility::re_managed_object::get_field<Vector3f*>(device, "AxisL");
        auto axis_r = *utility::re_managed_object::get_field<Vector3f*>(device, "AxisR");
        auto dir = Vector4f{axis_l.x, 0.0f, axis_l.y * -1.0f, 0.0f};

        auto delta = utility::re_component::get_delta_time(transform);

        m_custom_angles[0] += axis_r.y * m_rotation_speed->value() * delta;
        m_custom_angles[1] -= axis_r.x * m_rotation_speed->value() * delta;
        m_custom_angles[2] = 0.0f;

        utility::math::fix_angles(m_custom_angles);

        auto new_rotation = Matrix4x4f{ glm::quat{ m_custom_angles } };
        auto new_pos = m_last_camera_matrix[3] + new_rotation * dir * m_speed->value() * delta;

        // Keep track of the rotation if we want to lock the camera
        m_last_camera_matrix = new_rotation;
        m_last_camera_matrix[3] = new_pos;
    }
#endif

    transform->worldTransform = m_last_camera_matrix;
    transform->position = m_last_camera_matrix[3];
}

bool FreeCam::update_pointers() {
#ifndef RE8
    if (m_camera_system == nullptr || m_input_system == nullptr || m_survivor_manager == nullptr) {
        auto& globals = *g_framework->get_globals();
        m_camera_system = globals.get<RopewayCameraSystem>(game_namespace("camera.CameraSystem"));
        m_input_system = globals.get<RopewayInputSystem>(game_namespace("InputSystem"));
        m_survivor_manager = globals.get<RopewaySurvivorManager>(game_namespace("SurvivorManager"));
        return false;
    }
#else
    if (m_pad_manager == nullptr || m_props_manager == nullptr) {
        auto& globals = *g_framework->get_globals();
        m_pad_manager = globals.get<AppHIDPadManager>(game_namespace("HIDPadManager"));
        m_props_manager = globals.get<AppPropsManager>(game_namespace("PropsManager"));
        return false;
    }
#endif

    return true;
}
