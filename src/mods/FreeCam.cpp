#include <sdk/Application.hpp>
#include "sdk/REMath.hpp"
#include "sdk/SceneManager.hpp"

#include "HookManager.hpp"

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
    ImGui::SetNextItemOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);

    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    if (m_enabled->draw("Enabled")) {
        m_first_time = true;
    }

    ImGui::SameLine();
    m_lock_camera->draw("Lock Position");

#ifndef SF6
    m_disable_movement->draw("Disable Character Movement");
#endif

    m_toggle_key->draw("Toggle Key");
    m_move_up_key->draw("Move camera up Key");
    m_move_down_key->draw("Move camera down Key");
    m_lock_camera_key->draw("Lock Position Toggle Key");
#ifndef SF6
    m_disable_movement_key->draw("Disable Movement Toggle Key");
#endif
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
    { VkKeyScan('w'), MoveDirection::FORWARD },
    { VkKeyScan('a'), MoveDirection::LEFT },
    { VkKeyScan('s'), MoveDirection::BACKWARD },
    { VkKeyScan('d'), MoveDirection::RIGHT },
    { VK_UP, MoveDirection::FORWARD },
    { VK_LEFT, MoveDirection::LEFT },
    { VK_DOWN, MoveDirection::BACKWARD },
    { VK_RIGHT, MoveDirection::RIGHT },
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

    const auto camera = m_camera;

    if (camera == nullptr || transform != camera->ownerGameObject->transform) {
        return;
    }

#if defined(RE2) || defined(RE3)
    static auto get_player_condition_method = sdk::find_method_definition(game_namespace("SurvivorManager"), "get_Player");
    static auto get_action_orderer_method = sdk::find_method_definition(game_namespace("survivor.SurvivorCondition"), "get_ActionOrderer");

    const auto condition = get_player_condition_method->call<RopewaySurvivorPlayerCondition*>(sdk::get_thread_context(), m_survivor_manager);
    auto orderer = condition != nullptr ? get_action_orderer_method->call<RopewaySurvivorActionOrderer*>(sdk::get_thread_context(), condition) : nullptr;

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
        if (orderer != nullptr) {
            orderer->enabled = true;
        }
#endif

        /*if (joint != nullptr && transform->joints.matrices != nullptr) {
            m_last_camera_matrix = transform->joints.matrices->data[0].worldMatrix;
        }
        else {
            m_last_camera_matrix = transform->worldTransform;
        }*/

        if (joint != nullptr) {
            m_last_camera_matrix = Matrix4x4f{sdk::get_joint_rotation(joint)};
            m_last_camera_matrix[3] = sdk::get_joint_position(joint);
        }
        else {
            m_last_camera_matrix = transform->worldTransform;
        }

        m_first_time = false;

        m_custom_angles = math::euler_angles(glm::extractMatrixRotation(m_last_camera_matrix));
        m_twist = 0.0f;
        //m_custom_angles[1] *= -1.0f;
        //m_custom_angles[1] += glm::radians(180.0f);

        math::fix_angles(m_custom_angles);

        return;
    }

#if defined(RE2) || defined(RE3)
    if (orderer != nullptr) {
        orderer->enabled = !m_disable_movement->value();
    }
#endif

    // Update wanted camera position
    if (!m_lock_camera->value()) {
#if TDB_VER > 49
        auto timescale = sdk::get_timescale() * sdk::Application::get_global_speed();

        if (timescale == 0.0f) {
            timescale = std::numeric_limits<float>::epsilon();
        }
        
        const auto timescale_mult = 1.0f / timescale;
#else
        // RE7 doesn't have timescale
        const auto timescale_mult = 1.0f;
#endif

        Vector4f dir{};
#if TDB_VER > 49
        const auto delta = re_component::get_delta_time(transform);
#else
        const auto delta = sdk::call_native_func_easy<float>(m_application.object, m_application.t, "get_DeltaTime");
#endif

        // The rotation speed gets scaled down here heavily since "1.0f" is way too fast... This makes the slider a bit more user-friendly.
        // TODO: Figure out a conversion here to make KB+M & Controllers equal in rotation sensitivity.
        const auto rotation_speed = m_rotation_speed->value();
        const auto rotation_speed_kbm = rotation_speed * 0.05f;

        auto pad = sdk::call_native_func_easy<REManagedObject*>(m_via_hid_gamepad.object, m_via_hid_gamepad.t, "get_LastInputDevice");

        // Controller support
        if (pad != nullptr) {
            static const auto gamepad_device_t = sdk::find_type_definition("via.hid.GamePadDevice");
            static const auto is_down = gamepad_device_t != nullptr ? gamepad_device_t->get_method("isDown(via.hid.GamePadButton)") : nullptr;

            // Move direction
            // It's not a Vector2f because via.vec2 is not actually 8 bytes, we don't want stack corruption to occur.
            const auto axis_l = *re_managed_object::get_field<Vector3f*>(pad, "AxisL");
            const auto axis_r = *re_managed_object::get_field<Vector3f*>(pad, "AxisR");

            bool is_using_up_down_modifier = false;
            bool is_using_twist_modifier = false;

            if (is_down != nullptr) {
                const auto dpad_up_is_down = is_down->call_safe<bool>(sdk::get_thread_context(), pad, via::hid::GamePadButton::LUp);
                const auto dpad_down_is_down = is_down->call_safe<bool>(sdk::get_thread_context(), pad, via::hid::GamePadButton::LDown);

                if (dpad_up_is_down) {
                    dir.y = 1.0f;
                } else if (dpad_down_is_down) {
                    dir.y = -1.0f;
                }

                const auto dpad_left_is_down = is_down->call_safe<bool>(sdk::get_thread_context(), pad, via::hid::GamePadButton::LLeft);
                const auto dpad_right_is_down = is_down->call_safe<bool>(sdk::get_thread_context(), pad, via::hid::GamePadButton::LRight);

                if (dpad_left_is_down) {
                    dir.x -= 1.0f;
                } else if (dpad_right_is_down) {
                    dir.x += 1.0f;
                }

                const auto l_trigger_is_down = is_down->call_safe<bool>(sdk::get_thread_context(), pad, via::hid::GamePadButton::LTrigBottom);

                if (l_trigger_is_down) {
                    if (glm::length(axis_r) > 0.0f) {
                        dir += Vector4f{ 0.0, axis_r.y, 0.0, 0.0f };
                        is_using_up_down_modifier = true;
                    }
                }

                const auto r_trigger_is_down = is_down->call_safe<bool>(sdk::get_thread_context(), pad, via::hid::GamePadButton::RTrigBottom);

                if (r_trigger_is_down) {
                    if (glm::length(axis_r) > 0.0f) {
                        //m_custom_angles[2] -= axis_r.x * rotation_speed * delta * timescale_mult;
                        m_twist += axis_r.x * rotation_speed * delta * timescale_mult;
                        is_using_twist_modifier = true;
                    }
                }
            }

            if (!is_using_up_down_modifier && !is_using_twist_modifier) {
                m_custom_angles[0] += axis_r.y * rotation_speed * delta * timescale_mult;
                m_custom_angles[1] -= axis_r.x * rotation_speed * delta * timescale_mult;
                //m_custom_angles[2] = 0.0f;
            }

            if (glm::length(axis_l) > 0.0f) {
                dir += Vector4f{ axis_l.x, 0.0f, axis_l.y * -1.0f, 0.0f };
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

        if (!g_framework->is_ui_focused()) {
            const auto& mouse_delta = g_framework->get_mouse_delta();

            if (keyboard_state[VK_RBUTTON]) {
                //m_custom_angles[2] -= mouse_delta[0] * rotation_speed_kbm * delta * timescale_mult;
                m_twist -= mouse_delta[0] * rotation_speed_kbm * delta * timescale_mult;
            } else {
                m_custom_angles[0] -= mouse_delta[1] * rotation_speed_kbm * delta * timescale_mult;
                m_custom_angles[1] -= mouse_delta[0] * rotation_speed_kbm * delta * timescale_mult;
            }
        }

        math::fix_angles(m_custom_angles);
        
        auto new_rotation = glm::quat{ m_custom_angles };
        new_rotation = glm::rotate(new_rotation, m_twist, glm::vec3{0.0f, 0.0f, 1.0f});
        const auto new_pos = m_last_camera_matrix[3] + new_rotation * dir * (dir_speed * delta * timescale_mult);

        // Keep track of the rotation if we want to lock the camera
        m_last_camera_matrix = glm::mat4{new_rotation};
        m_last_camera_matrix[3] = new_pos;
    }

    transform->worldTransform = m_last_camera_matrix;
    transform->position = m_last_camera_matrix[3];

    if (joint != nullptr) {
        joint->posOffset = Vector4f{};
        *(Vector4f*)&joint->anglesOffset = Vector4f{0.0f, 0.00f, 0.0f, 1.0f};
    }
}

void FreeCam::on_pre_application_entry(void* entry, const char* name, size_t hash) {
    if (hash == "LockScene"_fnv) {
        if (!m_enabled->value()) {
            m_camera = nullptr;
#ifdef RE4
            m_re4_body = nullptr;
#endif
            return;
        }

        m_camera = sdk::get_primary_camera();

#ifdef RE4
        if (m_disable_movement->value()) {
            const auto character_manager = sdk::get_managed_singleton<::REManagedObject>(game_namespace("CharacterManager"));

            if (character_manager == nullptr) {
                m_re4_body = nullptr;
                return;
            }

            const auto player_context = sdk::call_object_func_easy<::REManagedObject*>(character_manager, "getPlayerContextRef");

            if (player_context == nullptr) {
                m_re4_body = nullptr;
                return;
            }

            m_re4_body = sdk::call_object_func_easy<::REManagedObject*>(player_context, "get_BodyGameObject");

            if (m_re4_body == nullptr) {
                return;
            }

            auto standard_skip_pre_fn = [this](std::vector<uintptr_t>& args, std::vector<sdk::RETypeDefinition*>& arg_tys, uintptr_t ret_addr) -> HookManager::PreHookResult {
                if (!m_enabled->value() || !m_disable_movement->value()) {
                    return HookManager::PreHookResult::CALL_ORIGINAL;
                }

                const auto comp = (REComponent*)args[1];
                const auto owner = utility::re_component::get_game_object(comp);

                if (owner == m_re4_body) {
                    return HookManager::PreHookResult::SKIP_ORIGINAL;
                }

                return HookManager::PreHookResult::CALL_ORIGINAL;
            };

            if (!m_player_body_updater_hook.attempted_hook) {
                m_player_body_updater_hook.attempted_hook = true;

                const auto player_body_updater_t = sdk::find_type_definition(game_namespace("PlayerBodyUpdater"));
                if (player_body_updater_t == nullptr) {
                    return;
                }

                const auto update_fn = player_body_updater_t->get_method("update");
                const auto late_update_fn = player_body_updater_t->get_method("lateUpdate");
                const auto get_past_frame_move_dir_fn = player_body_updater_t->get_method("getPastFrameMoveDirVec");

                if (update_fn != nullptr) {
                    m_player_body_updater_hook.update_id = g_hookman.add(update_fn,
                        standard_skip_pre_fn,
                        [this](uintptr_t& ret_val, sdk::RETypeDefinition* ret_ty, uintptr_t ret_addr) {
                        }
                    );
                }

                if (late_update_fn != nullptr) {
                    m_player_body_updater_hook.late_update_id = g_hookman.add(late_update_fn,
                        standard_skip_pre_fn,
                        [this](uintptr_t& ret_val, sdk::RETypeDefinition* ret_ty, uintptr_t ret_addr) {
                        }
                    );
                }

                if (get_past_frame_move_dir_fn != nullptr) {
                    m_player_body_updater_hook.get_past_move_frame_move_dir_vec_id = g_hookman.add(get_past_frame_move_dir_fn,
                        [this](std::vector<uintptr_t>& args, std::vector<sdk::RETypeDefinition*>& arg_tys, uintptr_t ret_addr) -> HookManager::PreHookResult {
                            if (!m_enabled->value() || !m_disable_movement->value()) {
                                return HookManager::PreHookResult::CALL_ORIGINAL;
                            }

                            // The component is in arg2 because ValueTypes push everything to the right
                            const auto comp = (REComponent*)args[2];
                            const auto owner = utility::re_component::get_game_object(comp);

                            if (owner == m_re4_body) {
                                return HookManager::PreHookResult::SKIP_ORIGINAL;
                            }

                            return HookManager::PreHookResult::CALL_ORIGINAL;
                        },
                        [this](uintptr_t& ret_val, sdk::RETypeDefinition* ret_ty, uintptr_t ret_addr) {
                        }
                    );
                }
            }

            if (!m_player_motion_controller_hook.attempted_hook) {
                m_player_motion_controller_hook.attempted_hook = true;

                const auto player_motion_controller_t = sdk::find_type_definition(game_namespace("MotionController"));
                if (player_motion_controller_t == nullptr) {
                    return;
                }

                const auto change_motion_internal_fn = player_motion_controller_t->get_method("changeMotionInternal");

                if (change_motion_internal_fn != nullptr) {
                    m_player_motion_controller_hook.change_motion_internal_id = g_hookman.add(change_motion_internal_fn,
                        standard_skip_pre_fn,
                        [this](uintptr_t& ret_val, sdk::RETypeDefinition* ret_ty, uintptr_t ret_addr) {
                        }
                    );
                }
            }
        }
#endif
    }
}

bool FreeCam::update_pointers() {
#if defined(RE2) || defined(RE3)
    if (m_survivor_manager == nullptr) {
        auto& globals = *reframework::get_globals();
        m_survivor_manager = globals.get<RopewaySurvivorManager>(game_namespace("SurvivorManager"));
        return false;
    }
#endif

#ifdef RE8
    if (m_props_manager == nullptr) {
        auto& globals = *reframework::get_globals();
        m_props_manager = globals.get<AppPropsManager>(game_namespace("PropsManager"));
        return false;
    }
#endif

    // Should work for all games.
    return m_via_hid_gamepad.update() && m_application.update();
}
