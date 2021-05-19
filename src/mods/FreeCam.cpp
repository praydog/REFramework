#include "sdk/REMath.hpp"

#include "FreeCam.hpp"

using namespace utility;

void FreeCam::on_config_load(const Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }
}

void FreeCam::on_config_save(Config& cfg) {
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
    m_move_up_key->draw("Move camera up Key");
    m_move_down_key->draw("Move camera down Key");
    m_lock_camera_key->draw("Lock Position Toggle Key");
    m_disable_movement_key->draw("Disable Movement Toggle Key");
    m_speed_modifier_fast_key->draw("Speed modifier Fast key");
    m_speed_modifier_slow_key->draw("Speed modifier Slow key");

#ifdef RE8
    m_rotation_speed->draw("Rotation Speed");
#endif

    m_speed->draw("Speed");
    m_speed_modifier->draw("Speed Modifier");
}

#ifdef RE8
enum class MoveDirection : uint8_t {
    FORWARD = 0,
    BACKWARD,
    LEFT,
    RIGHT
};

std::unordered_map<MoveDirection, Vector4f> g_movedir_map{
    { MoveDirection::FORWARD, { 0.0f, 0.0f, -1.0f, 0.0f } },
    { MoveDirection::BACKWARD, { 0.0f, 0.0f, 1.0f, 0.0f  } },
    { MoveDirection::LEFT, { -1.0f, 0.0f, 0.0f, 0.0f  } },
    { MoveDirection::RIGHT, { 1.0f, 0.0f, 0.0f, 0.0f  } },
};

std::unordered_map<int32_t, MoveDirection> g_vk_to_movedir{
    { DIK_W, MoveDirection::FORWARD },
    { DIK_A, MoveDirection::LEFT },
    { DIK_S, MoveDirection::BACKWARD },
    { DIK_D, MoveDirection::RIGHT },
    { DIK_UP, MoveDirection::FORWARD },
    { DIK_LEFT, MoveDirection::LEFT },
    { DIK_DOWN, MoveDirection::BACKWARD },
    { DIK_RIGHT, MoveDirection::RIGHT },
};
#endif

void FreeCam::on_update_transform(RETransform* transform) {
    if (!m_enabled->value() && !m_first_time) {
        m_was_disabled = false;
        return;
    }

    if (!update_pointers()) {
        m_was_disabled = false;
        return;
    }

#ifndef RE8
    if (m_camera_system->mainCamera == nullptr || transform != m_camera_system->mainCamera->ownerGameObject->transform) {
        return;
    }

    const auto condition = m_survivor_manager->playerCondition;
    if (m_first_time || condition == nullptr) {
        m_last_camera_matrix = transform->worldTransform;

        if (condition != nullptr && condition->actionOrderer != nullptr) {
            condition->actionOrderer->enabled = true;
            m_first_time = false;
        }
        
        return;
    }

    const auto orderer = condition->actionOrderer;
    if (orderer != nullptr) {
        orderer->enabled = !m_disable_movement->value();
    }

    const auto controller = m_camera_system->cameraController;
    if (controller == nullptr) {
        return;
    }

    // despite the name, it works with the keyboard
    const auto left_analog = re_managed_object::get_field<RopewayInputSystemAnalogStick*>(m_input_system, "_LStick");
    if (left_analog == nullptr) {
        return;
    }

    // Update wanted camera position
    if (!m_lock_camera->value()) {
        // Move direction
        // It's not a Vector2f because via.vec2 is not actually 8 bytes, we don't want stack corruption to occur.
        const auto axis = re_managed_object::get_field<Vector3f>(left_analog, "Axis");
        auto dir = Vector4f{ axis.x, 0.0f, axis.y * -1.0f, 0.0f };

        const auto delta = re_component::get_delta_time(transform);

        const auto& keyboard_state = g_framework->get_keyboard_state();
        if (keyboard_state[m_move_up_key->value()]) {
            dir.y = 1.0f;
        } 
        else if (keyboard_state[m_move_down_key->value()]) {
            dir.y = -1.0f;
        }

        const auto dir_speed_mod_fast = m_speed_modifier->value();
        const auto dir_speed_mod_slow = 1.f / dir_speed_mod_fast;

        auto dir_speed = m_speed->value();
        if (keyboard_state[m_speed_modifier_fast_key->value()]) {
            dir_speed *= dir_speed_mod_fast;
        } 
        else if (keyboard_state[m_speed_modifier_slow_key->value()]) {
            dir_speed *= dir_speed_mod_slow;
        }

        // Use controller rotation instead of camera rotation as it's accurate, will work in cutscenes.
        const auto new_pos = m_last_camera_matrix[3] + Matrix4x4f{ *(glm::quat*)&controller->worldRotation } * dir * dir_speed * delta;

        // Keep track of the rotation if we want to lock the camera
        m_last_camera_matrix = transform->worldTransform;
        m_last_camera_matrix[3] = new_pos;
    }

    controller->worldPosition = m_last_camera_matrix[3];
#else
    const auto player = m_props_manager->player;
    if (player != nullptr && player->transform != nullptr && player->transform == transform) {
        if (m_disable_movement->value() || m_was_disabled) {
            player->shouldUpdate = !m_disable_movement->value();
            m_was_disabled = !player->shouldUpdate;
        }
    }

    const auto camera = m_props_manager->camera;
    if (camera == nullptr || transform != camera->ownerGameObject->transform) {
        return;
    }

    if (m_first_time) {
        if (player != nullptr && m_was_disabled) {
            player->shouldUpdate = true;
            m_was_disabled = false;
        }

        m_last_camera_matrix = transform->worldTransform;
        m_first_time = false;

        m_custom_angles = math::euler_angles(glm::extractMatrixRotation(transform->worldTransform));
        //m_custom_angles[1] *= -1.0f;
        //m_custom_angles[1] += glm::radians(180.0f);

        math::fix_angles(m_custom_angles);

        return;
    }

    // Update wanted camera position
    if (!m_lock_camera->value()) {
        Vector4f dir{};
        const auto delta = re_component::get_delta_time(transform);

        // The rotation speed gets scaled down here heavily since "1.0f" is way too fast... This makes the slider a bit more user-friendly.
        // TODO: Figure out a conversion here to make KB+M & Controllers equal in rotation sensitivity.
        const auto rotation_speed = m_rotation_speed->value();
        const auto rotation_speed_kbm = rotation_speed * 0.05f;

        // Controller support
        if (m_pad_manager->pad1 != nullptr && m_pad_manager->pad1->device != nullptr) {
            const auto device = m_pad_manager->pad1->device;

            // Move direction
            // It's not a Vector2f because via.vec2 is not actually 8 bytes, we don't want stack corruption to occur.
            const auto axis_l = *re_managed_object::get_field<Vector3f*>(device, "AxisL");
            const auto axis_r = *re_managed_object::get_field<Vector3f*>(device, "AxisR");

            m_custom_angles[0] += axis_r.y * rotation_speed * delta;
            m_custom_angles[1] -= axis_r.x * rotation_speed * delta;
            m_custom_angles[2] = 0.0f;

            if (axis_l.length() > 0.0f) {
                dir = Vector4f{ axis_l.x, 0.0f, axis_l.y * -1.0f, 0.0f };
            }
        }

        const auto& keyboard_state = g_framework->get_keyboard_state();
        for (const auto& entry : g_vk_to_movedir) {
            if (keyboard_state[entry.first]) {
                dir += g_movedir_map[entry.second];
            }
        }

        if (keyboard_state[m_move_up_key->value()]) {
            dir.y = 1.0f;
        } 
        else if (keyboard_state[m_move_down_key->value()]) {
            dir.y = -1.0f;
        }

        const auto dir_speed_mod_fast = m_speed_modifier->value();
        const auto dir_speed_mod_slow = 1.f / dir_speed_mod_fast;

        auto dir_speed = m_speed->value();
        if (keyboard_state[m_speed_modifier_fast_key->value()]) {
            dir_speed *= dir_speed_mod_fast;
        } 
        else if (keyboard_state[m_speed_modifier_slow_key->value()]) {
            dir_speed *= dir_speed_mod_slow;
        }

        const auto& mouse_delta = g_framework->get_mouse_delta();

        m_custom_angles[0] -= mouse_delta[1] * rotation_speed_kbm * delta;
        m_custom_angles[1] -= mouse_delta[0] * rotation_speed_kbm * delta;
        m_custom_angles[2] = 0.0f;

        math::fix_angles(m_custom_angles);

        const auto new_rotation = Matrix4x4f{ glm::quat{ m_custom_angles } };
        const auto new_pos = m_last_camera_matrix[3] + new_rotation * dir * dir_speed * delta;

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
