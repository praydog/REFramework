#include "sdk/REMath.hpp"
#include "sdk/SceneManager.hpp"

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

    m_rotation_speed->draw("Rotation Speed");

    m_speed->draw("Speed");
    m_speed_modifier->draw("Speed Modifier");
}

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

void FreeCam::on_update_transform(RETransform* transform) {
    if (!m_enabled->value() && !m_first_time) {
        m_was_disabled = false;
        return;
    }

    if (!update_pointers()) {
        spdlog::error("FreeCam: Failed to update pointers");
        m_was_disabled = false;
        return;
    }


#ifdef RE8
    const auto player = m_props_manager->player;
    if (player != nullptr && player->transform != nullptr && player->transform == transform) {
        if (m_disable_movement->value() || m_was_disabled) {
            player->shouldUpdate = !m_disable_movement->value();
            m_was_disabled = !player->shouldUpdate;
        }
    }
#endif

    const auto camera = sdk::get_primary_camera();

    if (camera == nullptr || transform != camera->ownerGameObject->transform) {
        return;
    }


#if defined(RE2) || defined(RE3)
    const auto condition = m_survivor_manager->playerCondition;
#endif

    // first joint
    auto joint = utility::re_transform::get_joint(*transform, 0);

    if (m_first_time) {
#ifdef RE8
        if (player != nullptr && m_was_disabled) {
            player->shouldUpdate = true;
            m_was_disabled = false;
        }
#endif

#if defined(RE2) || defined(RE3)
        if (condition != nullptr && condition->actionOrderer != nullptr) {
            condition->actionOrderer->enabled = true;
        }
#endif

        if (joint != nullptr && transform->joints.matrices != nullptr) {
            m_last_camera_matrix = transform->joints.matrices->data[0].worldMatrix;
        }
        else {
            m_last_camera_matrix = transform->worldTransform;
        }

        m_first_time = false;

        m_custom_angles = math::euler_angles(glm::extractMatrixRotation(m_last_camera_matrix));
        //m_custom_angles[1] *= -1.0f;
        //m_custom_angles[1] += glm::radians(180.0f);

        math::fix_angles(m_custom_angles);

        return;
    }

#if defined(RE2) || defined(RE3)
    if (condition != nullptr) {
        const auto orderer = condition->actionOrderer;
        if (orderer != nullptr) {
            orderer->enabled = !m_disable_movement->value();
        }
    }
#endif

    // Update wanted camera position
    if (!m_lock_camera->value()) {
#ifndef RE7
        auto timescale = sdk::get_timescale();

        if (timescale == 0.0f) {
            timescale = std::numeric_limits<float>::epsilon();
        }

        const auto timescale_mult = 1.0f / timescale;
#else
        // RE7 doesn't have timescale
        const auto timescale_mult = 1.0f;
#endif

        Vector4f dir{};
#ifndef RE7
        const auto delta = re_component::get_delta_time(transform);
#else
        const auto delta = sdk::call_object_func<float>(m_application.object, m_application.t, "get_DeltaTime", sdk::get_thread_context(), m_application.object);
#endif

        // The rotation speed gets scaled down here heavily since "1.0f" is way too fast... This makes the slider a bit more user-friendly.
        // TODO: Figure out a conversion here to make KB+M & Controllers equal in rotation sensitivity.
        const auto rotation_speed = m_rotation_speed->value();
        const auto rotation_speed_kbm = rotation_speed * 0.05f;

        auto pad = sdk::call_object_func<REManagedObject*>(m_via_hid_gamepad.object, m_via_hid_gamepad.t, "get_LastInputDevice", sdk::get_thread_context(), m_via_hid_gamepad.object);

        // Controller support
        if (pad != nullptr) {
            // Move direction
            // It's not a Vector2f because via.vec2 is not actually 8 bytes, we don't want stack corruption to occur.
            const auto axis_l = *re_managed_object::get_field<Vector3f*>(pad, "AxisL");
            const auto axis_r = *re_managed_object::get_field<Vector3f*>(pad, "AxisR");

            m_custom_angles[0] += axis_r.y * rotation_speed * delta * timescale_mult;
            m_custom_angles[1] -= axis_r.x * rotation_speed * delta * timescale_mult;
            m_custom_angles[2] = 0.0f;

            if (glm::length(axis_l) > 0.0f) {
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

        m_custom_angles[0] -= mouse_delta[1] * rotation_speed_kbm * delta * timescale_mult;
        m_custom_angles[1] -= mouse_delta[0] * rotation_speed_kbm * delta * timescale_mult;
        m_custom_angles[2] = 0.0f;

        math::fix_angles(m_custom_angles);

        const auto new_rotation = Matrix4x4f{ glm::quat{ m_custom_angles } };
        const auto new_pos = m_last_camera_matrix[3] + new_rotation * dir * (dir_speed * delta * timescale_mult);

        // Keep track of the rotation if we want to lock the camera
        m_last_camera_matrix = new_rotation;
        m_last_camera_matrix[3] = new_pos;
    }

    transform->worldTransform = m_last_camera_matrix;
    transform->position = m_last_camera_matrix[3];

    if (joint != nullptr) {
        joint->posOffset = Vector4f{};
        *(Vector4f*)&joint->anglesOffset = Vector4f{0.0f, 0.00f, 0.0f, 1.0f};
    }
}

bool FreeCam::update_pointers() {
#if defined(RE2) || defined(RE3)
    if (m_survivor_manager == nullptr) {
        auto& globals = *g_framework->get_globals();
        m_survivor_manager = globals.get<RopewaySurvivorManager>(game_namespace("SurvivorManager"));
        return false;
    }
#endif

#ifdef RE8
    if (m_props_manager == nullptr) {
        auto& globals = *g_framework->get_globals();
        m_props_manager = globals.get<AppPropsManager>(game_namespace("PropsManager"));
        return false;
    }
#endif

    // Should work for all games.
    return m_via_hid_gamepad.update() && m_application.update();
}
