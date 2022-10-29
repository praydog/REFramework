#if defined(RE7) || defined(RE8)
#include <sdk/SceneManager.hpp>
#include <sdk/MurmurHash.hpp>
#include <sdk/Application.hpp>

#include "HookManager.hpp"

// Reminder: THIS MUST BE INCLUDED OR THE LOG FILE WILL BALLOON TO GIGANTIC SIZE
// AND THE GAME MAY CRASH. THIS IS REQUIRED FOR THE sol_lua_push DECLARATION.
#include "../../../mods/ScriptRunner.hpp"
#include "../../../mods/VR.hpp"

#include "RE8VR.hpp"

std::shared_ptr<RE8VR>& RE8VR::get() {
    static auto inst = std::make_shared<RE8VR>();
    return inst;
}

std::optional<std::string> RE8VR::on_initialize() {
    auto app_player_shadow_late_update = sdk::find_method_definition("app.PlayerShadow", "lateUpdate");

    if (app_player_shadow_late_update == nullptr) {
        spdlog::info("[RE8VR] Could not find app.PlayerShadow.lateUpdate");
        spdlog::info("[RE8VR] Shadows may look abnormal");
    } else {
        spdlog::info("[RE8VR] Found app.PlayerShadow.lateUpdate");

        g_hookman.add(app_player_shadow_late_update, &RE8VR::pre_shadow_late_update, &RE8VR::post_shadow_late_update);
    }

    return std::nullopt;
}

void RE8VR::on_config_load(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }
}

void RE8VR::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}

void RE8VR::on_lua_state_created(sol::state& lua) {
    lua.new_usertype<RE8VR>("RE8VR",
        "player", &RE8VR::m_player_downcast,
        "transform", &RE8VR::m_transform,
        "inventory", &RE8VR::m_inventory,
        "updater", &RE8VR::m_updater,
        "weapon", &RE8VR::m_weapon,
        "hand_touch", &RE8VR::m_hand_touch,
        "order", &RE8VR::m_order,
        "status", &RE8VR::m_status,
        "event_action_controller", &RE8VR::m_event_action_controller,
        "game_event_action_controller", &RE8VR::m_game_event_action_controller,
        "hit_controller", &RE8VR::m_hit_controller,
        "left_hand_ik", &RE8VR::m_left_hand_ik,
        "right_hand_ik", &RE8VR::m_right_hand_ik,
        "left_hand_ik_transform", &RE8VR::m_left_hand_ik_transform,
        "right_hand_ik_transform", &RE8VR::m_right_hand_ik_transform,
        "left_hand_ik_object", &RE8VR::m_left_hand_ik_object,
        "right_hand_ik_object", &RE8VR::m_right_hand_ik_object,
        "left_hand_position_offset", &RE8VR::m_left_hand_position_offset,
        "right_hand_position_offset", &RE8VR::m_right_hand_position_offset,
        "left_hand_rotation_offset", &RE8VR::m_left_hand_rotation_offset,
        "right_hand_rotation_offset", &RE8VR::m_right_hand_rotation_offset,
        "last_right_hand_position", &RE8VR::m_last_right_hand_position,
        "last_right_hand_rotation", &RE8VR::m_last_right_hand_rotation,
        "last_left_hand_position", &RE8VR::m_last_left_hand_position,
        "last_left_hand_rotation", &RE8VR::m_last_left_hand_rotation,
        "last_shoot_pos", &RE8VR::m_last_shoot_pos,
        "last_shoot_dir", &RE8VR::m_last_shoot_dir,
        "last_muzzle_pos", &RE8VR::m_last_muzzle_pos,
        "last_muzzle_forward", &RE8VR::m_last_muzzle_forward,
        "was_gripping_weapon", &RE8VR::m_was_gripping_weapon,
        "is_holding_left_grip", &RE8VR::m_is_holding_left_grip,
        "is_in_cutscene", &RE8VR::m_is_in_cutscene,
        "is_grapple_aim", &RE8VR::m_is_grapple_aim,
        "is_reloading", &RE8VR::m_is_reloading,
        "is_motion_play", &RE8VR::m_is_motion_play,
        "in_re8_end_game_event", &RE8VR::m_in_re8_end_game_event,
        "has_vehicle", &RE8VR::m_has_vehicle,
        "can_use_hands", &RE8VR::m_can_use_hands,
        "is_arm_jacked", &RE8VR::m_is_arm_jacked,
        "wants_block", &RE8VR::m_wants_block,
        "wants_heal", &RE8VR::m_wants_heal,
        "delta_time", &RE8VR::m_delta_time,
        "movement_speed_rate", &RE8VR::m_movement_speed_rate,
        "set_hand_joints_to_tpose", &RE8VR::set_hand_joints_to_tpose,
        "update_hand_ik", &RE8VR::update_hand_ik,
        "update_body_ik", &RE8VR::update_body_ik,
        "update_player_gestures", &RE8VR::update_player_gestures,
        "update_pointers", &RE8VR::update_pointers,
        "update_ik_pointers", &RE8VR::update_ik_pointers,
        "fix_player_camera", &RE8VR::fix_player_camera,
        "fix_player_shadow", &RE8VR::fix_player_shadow,
        "get_localplayer", &RE8VR::get_localplayer,
        "get_weapon_object", &RE8VR::get_weapon_object);

    lua["re8vr"] = this;
}

void RE8VR::on_lua_state_destroyed(sol::state& lua) {
    
}

void RE8VR::on_draw_ui() {
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);

    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    m_hide_upper_body->draw("Hide Upper Body");
    m_hide_lower_body->draw("Hide Lower Body");
    m_hide_arms->draw("Hide Arms");
    m_hide_upper_body_cutscenes->draw("Auto Hide Upper Body in Cutscenes");
    m_hide_lower_body_cutscenes->draw("Auto Hide Lower Body in Cutscenes");
}

void RE8VR::on_pre_application_entry(void* entry, const char* name, size_t hash) {
    switch (hash) {
    case "LockScene"_fnv:
        on_pre_lock_scene(entry);
        break;
    default:
        break;
    }
}

void RE8VR::on_application_entry(void* entry, const char* name, size_t hash) {
    
}

void RE8VR::on_pre_lock_scene(void* entry) {
    auto& vr = VR::get();

    if (!vr->is_hmd_active()) {
        return;
    }

    fix_player_shadow();
}

void RE8VR::reset_data() {
    m_player = nullptr;
    m_transform = nullptr;
    m_inventory = nullptr;
    m_updater = nullptr;
    m_weapon = nullptr;
    m_hand_touch = nullptr;
    m_order = nullptr;
    m_status = nullptr;
    m_event_action_controller = nullptr;
    m_game_event_action_controller = nullptr;
    m_hit_controller = nullptr;
    m_left_hand_ik = nullptr;
    m_right_hand_ik = nullptr;
    m_left_hand_ik_transform = nullptr;
    m_right_hand_ik_transform = nullptr;
    m_left_hand_ik_object = nullptr;
    m_right_hand_ik_object = nullptr;
}

void RE8VR::set_hand_joints_to_tpose(::REManagedObject* hand_ik) {
    //fixes an instance in!a hero where
    // chris is supposed to jump down something without tripping a laser
    if (m_player == nullptr || m_is_in_cutscene) {
        return;
    }

#ifdef RE7
        std::vector<uint32_t> hashes = {
            *sdk::get_object_field<uint32_t>(hand_ik, "HashJoint0"),
            *sdk::get_object_field<uint32_t>(hand_ik, "HashJoint1"),
            *sdk::get_object_field<uint32_t>(hand_ik, "HashJoint2"),
            *sdk::get_object_field<uint32_t>(hand_ik, "HashJoint3")
        };
#else
        std::vector<uint32_t> hashes = {
            *sdk::get_object_field<uint32_t>(hand_ik, "<HashJoint0>k__BackingField"),
            *sdk::get_object_field<uint32_t>(hand_ik, "<HashJoint1>k__BackingField"),
            *sdk::get_object_field<uint32_t>(hand_ik, "<HashJoint2>k__BackingField")
        };
#endif

    std::vector<::REJoint*> joints{};
    auto player_transform = m_player->transform;

    for (auto hash : hashes) {
        if (hash == 0) {
            continue;
        }

        auto joint = sdk::get_transform_joint_by_hash(player_transform, hash);

        if (joint != nullptr) {
            joints.push_back(sdk::get_joint_parent(joint));
        }
    }

    uint32_t additional_parents = 0;

    if (!m_is_grapple_aim) {
        additional_parents = 2;
    }

    utility::re_transform::apply_joints_tpose(*player_transform, joints, additional_parents);
}

void RE8VR::update_hand_ik() {
    if (m_in_re8_end_game_event) {
        return;
    }

    static auto motion_get_joint_index_by_name_hash = sdk::find_type_definition("via.motion.Motion")->get_method("getJointIndexByNameHash");
    static auto motion_get_world_position = sdk::find_type_definition("via.motion.Motion")->get_method("getWorldPosition");
    static auto motion_get_world_rotation = sdk::find_type_definition("via.motion.Motion")->get_method("getWorldRotation");
    static auto motion_typedef = sdk::find_type_definition("via.motion.Motion");
    static auto motion_type = motion_typedef->get_type();
    static uint32_t head_hash = 0;

    auto& vr = VR::get();
    
    if (m_player == nullptr || m_left_hand_ik == nullptr || m_right_hand_ik == nullptr) {
        m_was_gripping_weapon = false;
        m_is_holding_left_grip = false;
        return;
    }

    if (!vr->is_hmd_active() || !vr->is_using_controllers()) {
        m_was_gripping_weapon = false;
        m_is_holding_left_grip = false;
        return;
    }

    if (!m_can_use_hands || m_has_vehicle) {
        m_was_gripping_weapon = false;
        return;
    }

    const auto controllers = vr->get_controllers();

    ::REJoint* head_joint = nullptr;
    auto motion = utility::re_component::find<::REManagedObject>(m_player->transform, motion_type);

    std::optional<glm::quat> original_head_rotation{};
    auto original_right_rot = glm::identity<glm::quat>();
    auto original_left_rot_relative = glm::identity<glm::quat>();
    auto original_left_pos_relative = Vector4f{0, 0, 0, 1};
    auto original_right_rot_relative = glm::identity<glm::quat>();
    auto original_right_pos_relative = Vector4f{0, 0, 0, 1};

    if (motion != nullptr) {
        auto transform = m_player->transform;

        if (head_hash == 0) {
            auto head_joint = sdk::get_transform_joint_by_name(transform, L"Head");

            if (head_joint != nullptr) {
                head_hash = sdk::get_joint_hash(head_joint);
            }
        }

        if (head_hash != 0) {
            head_joint = sdk::get_transform_joint_by_hash(transform, head_hash);

            if (head_joint != nullptr) {
                original_head_rotation = sdk::get_joint_rotation(head_joint);
            }
        }

        uint32_t left_hash = 0;
        uint32_t right_hash = 0;

#ifdef RE7
        left_hash = *sdk::get_object_field<uint32_t>(m_left_hand_ik, "HashJoint2");
        right_hash = *sdk::get_object_field<uint32_t>(m_right_hand_ik, "HashJoint2");
#else
        left_hash = *sdk::get_object_field<uint32_t>(m_left_hand_ik, "<HashJoint2>k__BackingField");
        right_hash = *sdk::get_object_field<uint32_t>(m_right_hand_ik, "<HashJoint2>k__BackingField");
#endif

        const auto left_index = motion_get_joint_index_by_name_hash->call<uint32_t>(sdk::get_thread_context(), motion, left_hash);
        const auto right_index = motion_get_joint_index_by_name_hash->call<uint32_t>(sdk::get_thread_context(), motion, right_hash);

        Vector4f original_left_pos{};
        Vector4f original_right_pos{};
        glm::quat original_left_rot{};
        glm::quat original_right_rot{};

        motion_get_world_position->call(&original_left_pos, sdk::get_thread_context(), motion, left_index);
        motion_get_world_position->call(&original_right_pos, sdk::get_thread_context(), motion, right_index);
        motion_get_world_rotation->call(&original_left_rot, sdk::get_thread_context(), motion, left_index);
        motion_get_world_rotation->call(&original_right_rot, sdk::get_thread_context(), motion, right_index);

        const auto right_rot_inverse = glm::inverse(original_right_rot);
        original_left_pos_relative = right_rot_inverse * (original_left_pos - original_right_pos);
        original_left_rot_relative = right_rot_inverse * original_left_rot;

        const auto left_rot_inverse = glm::inverse(original_left_rot);
        original_right_pos_relative = left_rot_inverse * (original_right_pos - original_left_pos);
        original_right_rot_relative = left_rot_inverse * original_right_rot;
    }

    const auto left_controller_transform = vr->get_transform(controllers[0]);
    const auto right_controller_transform = vr->get_transform(controllers[1]);
    const auto left_controller_rotation = glm::quat{left_controller_transform};
    const auto right_controller_rotation = glm::quat{right_controller_transform};

    const auto hmd_transform = vr->get_transform(0);

    const auto left_controller_offset = left_controller_transform[3] - hmd_transform[3];
    const auto right_controller_offset = right_controller_transform[3] - hmd_transform[3];

    auto camera = sdk::get_primary_camera();
    const auto camera_rotation = glm::quat{vr->get_last_render_matrix()};

    auto original_camera_matrix = sdk::call_object_func_easy<Matrix4x4f>(camera, "get_WorldMatrix");
    auto original_camera_rotation = glm::quat{original_camera_matrix};
    auto updated_camera_pos = original_camera_matrix[3];

    vr->apply_hmd_transform(original_camera_rotation, updated_camera_pos);

    original_camera_rotation = glm::normalize(original_camera_rotation * glm::inverse(glm::quat{hmd_transform}));

    auto rh_rotation = original_camera_rotation * right_controller_rotation * m_right_hand_rotation_offset;
    auto rh_pos = updated_camera_pos
                + ((original_camera_rotation * right_controller_offset) 
                + (glm::normalize(original_camera_rotation * right_controller_rotation) * m_right_hand_position_offset));

    rh_pos.w = 1.0f;

    auto lh_grip_position = rh_pos + (glm::normalize(rh_rotation) * original_left_pos_relative);
    lh_grip_position.w = 1.0f;

    auto lh_rotation = original_camera_rotation * left_controller_rotation * m_left_hand_rotation_offset;
    auto lh_pos = updated_camera_pos
                + ((original_camera_rotation * left_controller_offset)
                + (glm::normalize(original_camera_rotation * left_controller_rotation) * m_left_hand_position_offset));

    lh_pos.w = 1.0f;

    const auto lh_delta_to_rh = (lh_pos - rh_pos);
    const auto lh_grip_delta_to_rh = (lh_grip_position - rh_pos);
    const auto lh_grip_delta = (lh_grip_position - lh_pos);
    const auto lh_grip_distance = glm::length(lh_grip_delta);

    m_was_gripping_weapon = lh_grip_distance <= 0.1f || (m_was_gripping_weapon && m_is_holding_left_grip);

    // Lets the player hold their left hand near the original (grip) position of the weapon
    if (m_was_gripping_weapon && !m_is_reloading) {
        if (glm::length(original_left_pos_relative) >= 0.1f) {
            auto original_grip_rot = utility::math::to_quat(glm::normalize(lh_grip_delta_to_rh));
            auto current_grip_rot = utility::math::to_quat(glm::normalize(lh_delta_to_rh));

            auto grip_rot_delta = glm::normalize(current_grip_rot * glm::inverse(original_grip_rot));

            // Adjust the right hand rotation;
            rh_rotation = glm::normalize(grip_rot_delta * rh_rotation);

            // Adjust the grip position
            lh_grip_position = rh_pos + (rh_rotation * original_left_pos_relative);
            lh_grip_position.w = 1.0f;
        }

        // Set the left hand position and rotation to the grip position
        lh_pos = lh_grip_position;
        lh_rotation = rh_rotation * original_left_rot_relative;
    } else {
        if (m_is_reloading) {
            lh_pos = lh_grip_position;
            lh_rotation = rh_rotation * original_left_rot_relative;
        } else {
            lh_pos = lh_pos;
            lh_rotation = lh_rotation;
        }
    }

    m_last_left_hand_position = lh_pos;
    m_last_left_hand_rotation = lh_rotation;
    m_last_left_hand_rotation.w = 1.0f;

    set_hand_joints_to_tpose(m_left_hand_ik);

    sdk::set_transform_position(m_left_hand_ik_transform, lh_pos);
    sdk::set_transform_rotation(m_left_hand_ik_transform, lh_rotation);
    *sdk::get_object_field<float>(m_left_hand_ik, "Transition") = 1.0f;
    sdk::call_object_func_easy<void*>(m_left_hand_ik, "calc");

    set_hand_joints_to_tpose(m_right_hand_ik);

    sdk::set_transform_position(m_right_hand_ik_transform, rh_pos);
    sdk::set_transform_rotation(m_right_hand_ik_transform, rh_rotation);
    *sdk::get_object_field<float>(m_right_hand_ik, "Transition") = 1.0f;
    sdk::call_object_func_easy<void*>(m_right_hand_ik, "calc");

    m_last_right_hand_position = rh_pos;
    m_last_right_hand_rotation = rh_rotation;
    m_last_right_hand_rotation.w = 1.0f;

    if (head_joint != nullptr && original_head_rotation) {
        sdk::set_joint_rotation(head_joint, *original_head_rotation);
    }
}

void RE8VR::update_body_ik(glm::quat* camera_rotation, Vector4f* camera_pos) {
    if (m_player == nullptr) {
        return;
    }

    if (m_in_re8_end_game_event) {
        return;
    }

    static auto via_motion_ik_leg = sdk::find_type_definition("via.motion.IkLeg");
    static auto via_motion_ik_leg_type = via_motion_ik_leg->get_type();
    static auto via_motion_motion = sdk::find_type_definition("via.motion.Motion");
    static auto via_motion_motion_type = via_motion_motion->get_type();
    static auto ik_leg_set_center_offset = via_motion_ik_leg->get_method("set_CenterOffset");
    static auto ik_leg_set_center_adjust = via_motion_ik_leg->get_method("setCenterAdjust");
    static auto ik_leg_set_center_position_ctrl = via_motion_ik_leg->get_method("set_CenterPositionCtrl");
    static auto ik_leg_set_ground_contact_up_distance = via_motion_ik_leg->get_method("set_GroundContactUpDistance");
    static auto ik_leg_set_enabled = via_motion_ik_leg->get_method("set_Enabled");

    auto& vr = VR::get();
    auto ik_leg = utility::re_component::find<::REManagedObject>(m_player->transform, via_motion_ik_leg_type);

    if (ik_leg == nullptr) {
        if (!vr->is_using_controllers() || m_is_in_cutscene || camera_rotation == nullptr || camera_pos == nullptr) {
            return;
        }

        ik_leg = sdk::call_object_func_easy<::REManagedObject*>(m_player, "createComponent(System.Type)", via_motion_ik_leg->get_runtime_type());

        if (ik_leg == nullptr) {
            spdlog::error("[RE8VR] Failed to create IK leg component");
            return;
        }
    }

    if (m_is_in_cutscene || m_has_vehicle || !vr->is_hmd_active() || !vr->is_using_controllers()) {
        //ik_leg:call("set_Enabled", false)
        const auto zero_vec = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
        ik_leg_set_center_offset->call(sdk::get_thread_context(), ik_leg, &zero_vec);
        ik_leg_set_center_adjust->call(sdk::get_thread_context(), ik_leg, 0);
        ik_leg_set_center_position_ctrl->call(sdk::get_thread_context(), ik_leg, 2); // world offset
        ik_leg_set_ground_contact_up_distance->call(sdk::get_thread_context(), ik_leg, 0.0f); // Fixes the whole player being jarringly moved upwards.

        if (!vr->is_using_controllers()) {
            sdk::call_object_func<void*>(ik_leg, "destroy", sdk::get_thread_context(), ik_leg);
        }
        
        return;
    } else {
        ik_leg_set_enabled->call(sdk::get_thread_context(), ik_leg, true);
    }

    if (camera_rotation == nullptr || camera_pos == nullptr) {
        return;
    }

    auto motion = utility::re_component::find<::REManagedObject>(m_player->transform, via_motion_motion_type);

    if (motion == nullptr) {
        spdlog::error("[RE8VR] Failed to get motion component");
        return;
    }

    auto transform = m_player->transform;

    static uint32_t head_hash = sdk::murmur_hash::calc32("Head");
    
    const auto transform_rot = sdk::get_transform_rotation(transform);
    const auto transform_pos = sdk::get_transform_position(transform);

    auto head_joint = sdk::get_transform_joint_by_hash(transform, head_hash);

    const auto normal_dir = *camera_rotation * Vector3f{0, 0, 1};
    auto flattened_dir = *camera_rotation * Vector3f{0, 0, 1};
    flattened_dir.y = 0.0f;
    flattened_dir = glm::normalize(flattened_dir);

    const auto original_head_pos = Vector3f{utility::re_transform::calculate_tpose_pos_world(*transform, head_joint, 4)} + (flattened_dir * (glm::abs(normal_dir.y) * -0.1f)) + (flattened_dir * 0.025f);
    const auto diff_to_camera = Vector4f{(Vector3f{*camera_pos} - original_head_pos), 1.0f};

    //ik_leg:call("set_CenterJointName", "Hip")
    ik_leg_set_center_offset->call(sdk::get_thread_context(), ik_leg, &diff_to_camera);
    ik_leg_set_center_adjust->call(sdk::get_thread_context(), ik_leg, 0);
    ik_leg_set_center_position_ctrl->call(sdk::get_thread_context(), ik_leg, 2); // world offset
    ik_leg_set_ground_contact_up_distance->call(sdk::get_thread_context(), ik_leg, 0.0f); // Fixes the whole player being jarringly moved upwards.
    //ik_leg:call("set_UpdateTiming", 2) -- ConstraintsBegin
}

void RE8VR::update_player_gestures() {
    auto& vr = VR::get();

    if (m_player == nullptr || !vr->is_using_controllers()) {
        m_wants_block = false;
        m_wants_heal = false;
        m_heal_gesture.was_grip_down = false;
        m_heal_gesture.was_trigger_down = false;
        
        return;
    }

    const auto& controllers = vr->get_controllers();

    const auto hmd = vr->get_transform(0);
    const auto left_hand = vr->get_transform(controllers[0]);
    const auto right_hand = vr->get_transform(controllers[1]);

    m_hmd_delta_to_left = left_hand[3] - hmd[3];
    m_hmd_delta_to_right = right_hand[3] - hmd[3];

    m_hmd_dir_to_left = glm::normalize(m_hmd_delta_to_left);
    m_hmd_dir_to_right = glm::normalize(m_hmd_delta_to_right);

    update_block_gesture();
    update_heal_gesture();
}

void RE8VR::fix_player_camera(::REManagedObject* player_camera) {
    auto& vr = VR::get();

    m_in_re8_end_game_event = false;

    if (!vr->is_hmd_active()) {
        // so the camera doesn't go wacky
        if (m_camera_data.last_hmd_active_state) {
            // disables the body IK component
            update_body_ik(nullptr, nullptr);

            vr->set_gui_rotation_offset(glm::identity<glm::quat>());
            vr->recenter_view();

            m_camera_data.last_hmd_active_state = false;
        }

        // Restore the vertical camera movement after taking headset off/not using controllers
        if (m_camera_data.was_vert_limited) {
           auto base_transform_solver = sdk::get_object_field<::REManagedObject*>(player_camera, "BaseTransSolver");

            if (base_transform_solver != nullptr && *base_transform_solver != nullptr) {
                auto camera_controller = sdk::get_object_field<::REManagedObject*>(*base_transform_solver, "CurrentController");
    
                if (camera_controller == nullptr) {
                    camera_controller = sdk::get_object_field<::REManagedObject*>(*base_transform_solver, "<CurrentController>k__BackingField");
                }
    
                if (camera_controller != nullptr && *camera_controller != nullptr) {
                    *sdk::get_object_field<bool>(*camera_controller, "IsVerticalRotateLimited") = false;
                }
            }

           m_camera_data.was_vert_limited = false;
        }

        return;
    }

#ifdef RE8
    /*
    Check whether we're in the event at the end of RE8
    and return early if we are.
    */
    if (m_is_in_cutscene && m_game_event_action_controller != nullptr) {
        auto event_action = sdk::get_object_field<::REManagedObject*>(m_game_event_action_controller, "_GameEventAction");

        if (event_action != nullptr && *event_action != nullptr) {
            auto event_name = sdk::get_object_field<::SystemString*>(*event_action, "_EventName");

            if (event_name != nullptr && *event_name != nullptr) {
                if (s_re8_end_game_events.contains(utility::re_string::get_string(*event_name))) {
                    m_in_re8_end_game_event = true;
                    return;
                }
            }
        }
    }
#endif

    m_camera_data.last_hmd_active_state = true;

    auto base_transform_solver = sdk::get_object_field<::REManagedObject*>(player_camera, "BaseTransSolver");
    auto is_maximum_controllable = true;

    if (base_transform_solver != nullptr && *base_transform_solver != nullptr) {
#ifdef RE8
        auto current_type_obj = *sdk::get_object_field<::REManagedObject*>(*base_transform_solver, "<currentType>k__BackingField");
        auto current_type = *sdk::get_object_field<int>(current_type_obj, "Value");

        auto vehicle = sdk::get_object_field<::REGameObject*>(player_camera, "RideVehicleObject");
        m_has_vehicle = vehicle != nullptr && *vehicle != nullptr;

        // Fixes special cutscene near the end of the game.
        if (m_has_vehicle && m_is_arm_jacked) {
            return;
        }
#else
        auto current_type = *sdk::get_object_field<int>(*base_transform_solver, "<currentType>k__BackingField");
#endif

        if (current_type != 0 && !m_has_vehicle) { // MaximumOperatable
            m_is_in_cutscene = true;
            is_maximum_controllable = false;
            m_camera_data.last_time_not_maximum_controllable = std::chrono::steady_clock::now();
        } else {
            if (std::chrono::steady_clock::now() - m_camera_data.last_time_not_maximum_controllable <= std::chrono::seconds(1)) {
                m_is_in_cutscene = true;
            }

            if (m_has_vehicle) {
                m_is_in_cutscene = false;
            }
        }
    }

    auto wants_recenter = false;

    if (m_is_in_cutscene && !m_camera_data.last_cutscene_state) {
        // force the gui to be recentered when we exit the cutscene
        m_camera_data.last_gui_forced_slerp = std::chrono::steady_clock::now();
        m_camera_data.last_gui_quat = glm::identity<glm::quat>();
        wants_recenter = true;

        vr->recenter_gui(m_camera_data.last_gui_quat);
    } else if (!m_is_in_cutscene && m_camera_data.last_cutscene_state) {
        m_camera_data.last_gui_forced_slerp = std::chrono::steady_clock::now();
        m_camera_data.last_gui_quat = glm::inverse(glm::quat{vr->get_rotation(0)});
        wants_recenter = true;

        vr->recenter_gui(glm::quat{vr->get_rotation(0)});
    }

    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    auto camera_gameobject = sdk::call_object_func_easy<::REGameObject*>(camera, "get_GameObject");
    if (camera_gameobject == nullptr) {
        return;
    }

    auto camera_transform = camera_gameobject->transform;
    if (camera_transform == nullptr) {
        return;
    }

    auto camera_rot = sdk::get_transform_rotation(camera_transform);
    auto camera_pos = sdk::get_transform_position(camera_transform);

    // fix camera position.
    if (is_maximum_controllable && vr->is_using_controllers()) {
        auto param_container = sdk::get_object_field<::REManagedObject*>(player_camera, "_CurrentParamContainer");

        if (param_container == nullptr) {
            param_container = sdk::get_object_field<::REManagedObject*>(player_camera, "CurrentParamContainer");
        }

        if (param_container != nullptr && *param_container != nullptr) {
            auto posture_param = sdk::get_object_field<::REManagedObject*>(*param_container, "PostureParam");

            if (posture_param != nullptr && *posture_param != nullptr) {
                auto current_camera_offset_ptr = sdk::get_object_field<glm::vec4>(*posture_param, "CameraOffset");

                if (current_camera_offset_ptr != nullptr) {
                    auto current_camera_offset = *current_camera_offset_ptr;
                    current_camera_offset.y = 0.0f;
                    camera_pos += camera_rot * current_camera_offset;
                    camera_pos.w = 1.0f;
                }
            }
        }
    }

    auto camera_rot_pre_hmd = camera_rot;
    auto camera_pos_pre_hmd = camera_pos;

    auto camera_rot_no_shake_field = sdk::get_object_field<glm::quat>(player_camera, "<CameraRotation>k__BackingField");

    if (camera_rot_no_shake_field == nullptr) {
        camera_rot_no_shake_field = sdk::get_object_field<glm::quat>(player_camera, "<cameraRotation>k__BackingField");
    }

    auto camera_rot_no_shake = *camera_rot_no_shake_field;

    Vector4f zero_v4{0.0f, 0.0f, 0.0f, 0.0f};
    vr->apply_hmd_transform(camera_rot_no_shake, zero_v4);
    vr->apply_hmd_transform(camera_rot, camera_pos);
    camera_pos.w = 1.0f;

    auto camera_joint = utility::re_transform::get_joint(*camera_transform, 0);

    if (camera_joint == nullptr) {
        return;
    }

    // Transform is used for things like Ethan's light
    // and determining where the player is looking
    sdk::set_transform_position(camera_transform, camera_pos);
    sdk::set_transform_rotation(camera_transform, camera_rot);
    
    // Joint is used for the actual final rendering of the game world
    if (m_is_in_cutscene) {
        sdk::set_joint_position(camera_joint, camera_pos_pre_hmd);
        sdk::set_joint_rotation(camera_joint, camera_rot_pre_hmd);
    } else {
        const auto rot_delta = glm::inverse(camera_rot_pre_hmd) * camera_rot;

        auto forward = rot_delta * Vector3f{0.0f, 0.0f, 1.0f};
        forward = glm::normalize(Vector3f{forward.x, 0.0, forward.z});

        sdk::set_joint_position(camera_joint, camera_pos_pre_hmd);
        sdk::set_joint_rotation(camera_joint, camera_rot_pre_hmd * utility::math::to_quat(forward));
    }

    update_body_ik(&camera_rot, &camera_pos);

    glm::quat slerp_quat{};

    if (m_is_in_cutscene) {
        slerp_quat = camera_rot_pre_hmd * glm::inverse(camera_rot);
    } else {
        slerp_quat = glm::inverse(glm::quat{vr->get_rotation(0)});
    }

    slerp_gui(slerp_quat);

    static auto neg_forward_identity = glm::quat{Matrix4x4f{-1, 0, 0, 0,
                                                            0, 1, 0, 0,
                                                            0, 0, -1, 0,
                                                            0, 0, 0, 1}};

    const auto fixed_dir = glm::normalize((neg_forward_identity * camera_rot_no_shake) * Vector3f{0.0f, 0.0f, -1.0f});
    const auto fixed_rot = utility::math::to_quat(fixed_dir);

     // RE8 pre oct14 update
    auto camera_rotation_field = sdk::get_object_field<glm::quat>(player_camera, "<CameraRotation>k__BackingField");

    if (camera_rotation_field == nullptr) {
        camera_rotation_field = sdk::get_object_field<glm::quat>(player_camera, "<cameraRotation>k__BackingField");
    }

    *camera_rotation_field = fixed_rot;

     // RE8 pre oct14 update
    auto camera_position_field = sdk::get_object_field<glm::vec4>(player_camera, "<CameraPosition>k__BackingField");

    if (camera_position_field == nullptr) {
        camera_position_field = sdk::get_object_field<glm::vec4>(player_camera, "<cameraPosition>k__BackingField");
    }

    *camera_position_field = camera_pos;

#ifdef RE8
    // RE8 pre oct14 update
    auto fixed_aim_rotation_field = sdk::get_object_field<glm::quat>(player_camera, "FixedAimRotation");

    if (fixed_aim_rotation_field == nullptr) {
        fixed_aim_rotation_field = sdk::get_object_field<glm::quat>(player_camera, "<fixedAimRotation>k__BackingField");
    }

    *fixed_aim_rotation_field = fixed_rot;
#endif

    auto camera_rotation_with_movement_shake_field = sdk::get_object_field<glm::quat>(player_camera, "CameraRotationWithMovementShake");

    if (camera_rotation_with_movement_shake_field == nullptr) {
        camera_rotation_with_movement_shake_field = sdk::get_object_field<glm::quat>(player_camera, "cameraRotationWithMovementShake");
    }

    auto camera_rotation_with_camera_shake_field = sdk::get_object_field<glm::quat>(player_camera, "CameraRotationWithCameraShake");

    if (camera_rotation_with_camera_shake_field == nullptr) {
        camera_rotation_with_camera_shake_field = sdk::get_object_field<glm::quat>(player_camera, "cameraRotationWithCameraShake");
    }

    auto prev_camera_rotation_field = sdk::get_object_field<glm::quat>(player_camera, "PrevCameraRotation");

    if (prev_camera_rotation_field == nullptr) {
        prev_camera_rotation_field = sdk::get_object_field<glm::quat>(player_camera, "PrevcameraRotation"); // ???? why
    }

    *camera_rotation_with_movement_shake_field = fixed_rot;
    *camera_rotation_with_camera_shake_field = fixed_rot;
    *prev_camera_rotation_field = fixed_rot;

    auto camera_controller_param = sdk::get_object_field<::REManagedObject*>(player_camera, "CameraCtrlParam");

    if (camera_controller_param != nullptr) {
        *sdk::get_object_field<glm::quat>(*camera_controller_param, "CameraRotation") = fixed_rot;
    }

    if (base_transform_solver != nullptr && *base_transform_solver != nullptr) {
        auto camera_controller = sdk::get_object_field<::REManagedObject*>(*base_transform_solver, "CurrentController");

        if (camera_controller == nullptr) {
            camera_controller = sdk::get_object_field<::REManagedObject*>(*base_transform_solver, "<CurrentController>k__BackingField");
        }

        if (camera_controller != nullptr && *camera_controller != nullptr) {
            auto camera_controller_rot = glm::identity<glm::quat>();

            if (m_is_in_cutscene) {
#ifdef RE7
                camera_controller_rot = *sdk::get_object_field<glm::quat>(*camera_controller, "<rotation>k__BackingField");
#else
                camera_controller_rot = *sdk::get_object_field<glm::quat>(*camera_controller, "<Rotation>k__BackingField");
#endif
            } else {
                camera_controller_rot = fixed_rot;
            }

            camera_controller_rot = utility::math::flatten(camera_controller_rot);
            
            if (!m_is_in_cutscene || is_maximum_controllable) {
                if (!m_is_in_cutscene) {
                    vr->recenter_view();
                }
#ifdef RE7
                *sdk::get_object_field<glm::quat>(*camera_controller, "<rotation>k__BackingField") = camera_controller_rot;
#else
                *sdk::get_object_field<glm::quat>(*camera_controller, "<Rotation>k__BackingField") = camera_controller_rot;
#endif
            }

            *sdk::get_object_field<glm::quat>(*base_transform_solver, "<rotation>k__BackingField") = camera_controller_rot;
            *sdk::get_object_field<bool>(*camera_controller, "IsVerticalRotateLimited") = is_maximum_controllable;

            m_camera_data.was_vert_limited = true;
        }
    }

    struct Ray {
        glm::vec4 from;
        glm::vec4 dir;
    };

    auto look_ray = sdk::get_object_field<Ray>(player_camera, "LookRay");
    auto shoot_ray = sdk::get_object_field<Ray>(player_camera, "ShootRay");

    if (look_ray != nullptr) {
        look_ray->from = glm::vec4{camera_pos.x, camera_pos.y, camera_pos.z, 1.0f};
        look_ray->dir = glm::vec4{fixed_dir.x, fixed_dir.y, fixed_dir.z, 1.0f};
    }

    if (shoot_ray != nullptr) {
        if (!m_has_vehicle && vr->is_using_controllers() && m_weapon != nullptr) {
            const auto pos = m_last_muzzle_pos + (m_last_muzzle_forward * 0.02f);

            shoot_ray->from = glm::vec4{pos.x, pos.y, pos.z, 1.0f};
            shoot_ray->dir = glm::vec4{m_last_muzzle_forward.x, m_last_muzzle_forward.y, m_last_muzzle_forward.z, 1.0f};
        } else {
            m_last_shoot_pos = camera_pos;
            m_last_shoot_dir = fixed_dir;

            shoot_ray->from = glm::vec4{camera_pos.x, camera_pos.y, camera_pos.z, 1.0f};
            shoot_ray->dir = glm::vec4{fixed_dir.x, fixed_dir.y, fixed_dir.z, 1.0f};
        }
    }

    m_camera_data.last_cutscene_state = m_is_in_cutscene;
}

void RE8VR::slerp_gui(const glm::quat& new_gui_quat) {
    if (m_movement_speed_rate > 0.0f) {
        m_camera_data.last_gui_forced_slerp = std::chrono::steady_clock::now() - std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<float>(1.0f - m_movement_speed_rate));
    }
    

    m_camera_data.last_gui_dot = glm::dot(m_camera_data.last_gui_quat, new_gui_quat);
    const auto dot_dist = 1.0f - std::abs(m_camera_data.last_gui_dot);
    const auto dot_ang = std::acos(std::abs(m_camera_data.last_gui_dot)) * (180.0f / glm::pi<float>());
    m_camera_data.last_gui_dot = dot_ang;

    auto now = std::chrono::steady_clock::now();

    // trigger gui slerp
    if (dot_ang >= 20.0f || m_is_in_cutscene) {
        m_camera_data.last_gui_forced_slerp = now;
    }

    const auto slerp_time_diff = std::chrono::duration<float>(now - m_camera_data.last_gui_forced_slerp).count();

    if (slerp_time_diff <= GUI_MAX_SLERP_TIME) {
        if (dot_ang >= 10.0f) {
            m_camera_data.last_gui_forced_slerp = now;
        }

        m_camera_data.last_gui_quat = glm::slerp(m_camera_data.last_gui_quat, new_gui_quat, dot_dist * std::max((GUI_MAX_SLERP_TIME - slerp_time_diff) * m_delta_time, 0.0f));
    }

    if (m_is_in_cutscene) {
        VR::get()->recenter_gui(m_camera_data.last_gui_quat);
    } else {
        VR::get()->recenter_gui(m_camera_data.last_gui_quat * glm::inverse(new_gui_quat));
    }
}

void RE8VR::fix_player_shadow() {
    if (m_player == nullptr || m_player->transform == nullptr) {
        return;
    }

    auto& vr = VR::get();

    if (!vr->is_hmd_active()) {
        return;
    }

    static auto app_player_mesh_controller = sdk::find_type_definition("app.PlayerMeshController");

#ifdef RE8
    if (m_updater == nullptr) {
        return;
    }

    auto mesh_controller = sdk::call_object_func_easy<::REManagedObject*>(m_updater, "get_playerMeshController");

    if (mesh_controller == nullptr && m_order != nullptr) {
        mesh_controller = sdk::call_object_func_easy<::REManagedObject*>(m_order, "get_playerMeshController");
    }
#else
    static auto app_player_mesh_controller_type = app_player_mesh_controller->get_type();
    auto mesh_controller = utility::re_component::find<::REManagedObject>(m_player->transform, app_player_mesh_controller_type);
#endif

    if (mesh_controller == nullptr) {
        return;
    }

    static auto upper_body_mesh_field = app_player_mesh_controller->get_field("UpperBodyMesh");
    static auto lower_body_mesh_field = app_player_mesh_controller->get_field("LowerBodyMesh");
    static auto l_arm_mesh_field = app_player_mesh_controller->get_field("LArmMesh");
    static auto r_arm_mesh_field = app_player_mesh_controller->get_field("RArmMesh");

    static auto upper_body_shadow_mesh_field = app_player_mesh_controller->get_field("UpperBodyShadowMesh");
    static auto lower_body_shadow_mesh_field = app_player_mesh_controller->get_field("LowerBodyShadowMesh");
    static auto l_arm_shadow_mesh_field = app_player_mesh_controller->get_field("LArmShadowMesh");
    static auto r_arm_shadow_mesh_field = app_player_mesh_controller->get_field("RArmShadowMesh");
    static auto head_shadow_mesh_field = app_player_mesh_controller->get_field("HeadShadowMesh");

    static auto via_render_mesh = sdk::find_type_definition("via.render.Mesh");
    static auto set_draw_shadow_cast_method = via_render_mesh->get_method("set_DrawShadowCast");
    static auto set_enabled_method = via_render_mesh->get_method("set_Enabled");
    static auto set_draw_default_method = via_render_mesh->get_method("set_DrawDefault");

    auto toggle_shadow = [&](sdk::REField* field, bool state) {
        if (field == nullptr) {
            return;
        }

        auto data = (::REManagedObject**)field->get_data_raw(mesh_controller);

        if (data == nullptr) {
            return;
        }

        auto mesh = *data;

        if (mesh == nullptr) {
            return;
        }

        set_draw_shadow_cast_method->call(sdk::get_thread_context(), mesh, state);
    };

    auto toggle_enabled = [&](sdk::REField* field, bool state) {
        if (field == nullptr) {
            return;
        }

        auto data = (::REManagedObject**)field->get_data_raw(mesh_controller);

        if (data == nullptr) {
            return;
        }

        auto mesh = *data;

        if (mesh == nullptr) {
            return;
        }

        set_draw_default_method->call(sdk::get_thread_context(), mesh, state);
    };

    auto copy_joint = [&](uint32_t hash, ::RETransform* src, ::RETransform* dest) {
        if (src == nullptr || dest == nullptr) {
            return;
        }

        auto src_joint = sdk::get_transform_joint_by_hash(src, hash);
        auto dst_joint = sdk::get_transform_joint_by_hash(dest, hash);

        if (src_joint == nullptr || dst_joint == nullptr) {
            return;
        }

        sdk::set_joint_position(dst_joint, sdk::get_joint_position(src_joint));
        sdk::set_joint_rotation(dst_joint, sdk::get_joint_rotation(src_joint));
    };

    auto upper_mesh_ptr = (::REComponent**)upper_body_shadow_mesh_field->get_data_raw(mesh_controller);
    auto head_mesh_ptr = (::REComponent**)head_shadow_mesh_field->get_data_raw(mesh_controller);

    auto copy_mesh = [&](::REComponent** meshcomp) {
        if (meshcomp != nullptr) {
            auto mesh = *meshcomp;

            if (mesh != nullptr) {
                auto mesh_gameobject = mesh->ownerGameObject;

                if (mesh_gameobject != nullptr && mesh_gameobject->transform != nullptr) {
                    static auto head_hash = sdk::murmur_hash::calc32("Head");
                    static auto neck_hash = sdk::murmur_hash::calc32("Neck");
                    static auto neck_1_hash = sdk::murmur_hash::calc32("Neck_1");
                    static auto neck_0_hash = sdk::murmur_hash::calc32("Neck_0");
                    static auto chest_hash = sdk::murmur_hash::calc32("Chest");
                    
                    // Must be done in reverse order to preserve the head position.
                    copy_joint(chest_hash, m_player->transform, mesh_gameobject->transform);
                    copy_joint(neck_hash, m_player->transform, mesh_gameobject->transform);
                    copy_joint(neck_0_hash, m_player->transform, mesh_gameobject->transform);
                    copy_joint(neck_1_hash, m_player->transform, mesh_gameobject->transform);
                    copy_joint(head_hash, m_player->transform, mesh_gameobject->transform);
                }
            }
        }
    };

    copy_mesh(upper_mesh_ptr);
    copy_mesh(head_mesh_ptr);

    // Fix the head joint of the shadow mesh.
    if (upper_mesh_ptr != nullptr) {
        auto upper_mesh = *upper_mesh_ptr;

        if (upper_mesh != nullptr) {
            auto mesh_gameobject = upper_mesh->ownerGameObject;

            if (mesh_gameobject != nullptr && mesh_gameobject->transform != nullptr) {
                static auto head_hash = sdk::murmur_hash::calc32("Head");
                static auto neck_hash = sdk::murmur_hash::calc32("Neck");
                static auto neck_1_hash = sdk::murmur_hash::calc32("Neck_1");
                static auto neck_0_hash = sdk::murmur_hash::calc32("Neck_0");
                static auto chest_hash = sdk::murmur_hash::calc32("Chest");
                
                // Must be done in reverse order to preserve the head position.
                copy_joint(chest_hash, m_player->transform, mesh_gameobject->transform);
                copy_joint(neck_hash, m_player->transform, mesh_gameobject->transform);
                copy_joint(neck_0_hash, m_player->transform, mesh_gameobject->transform);
                copy_joint(neck_1_hash, m_player->transform, mesh_gameobject->transform);
                copy_joint(head_hash, m_player->transform, mesh_gameobject->transform);
            }
        }
    }

    const auto in_cutscene = m_is_in_cutscene || !m_can_use_hands || m_is_grapple_aim || m_has_vehicle;
    const auto using_controllers = vr->is_using_controllers();

    const auto wants_hide_upper_body = !using_controllers || m_hide_upper_body->value() || (in_cutscene && m_hide_upper_body_cutscenes->value());
    const auto wants_hide_lower_body = !using_controllers || m_hide_lower_body->value() || (in_cutscene && m_hide_lower_body_cutscenes->value());
    const auto wants_hide_arms = m_hide_arms->value();

    // These are the meshes for the real player body.
    toggle_shadow(upper_body_mesh_field, true);
    toggle_shadow(lower_body_mesh_field, true);
    toggle_shadow(l_arm_mesh_field, true);
    toggle_shadow(r_arm_mesh_field, true);

    // This is the fake player shadow meshes.
    toggle_shadow(upper_body_shadow_mesh_field, false);
    toggle_shadow(lower_body_shadow_mesh_field, false);
    toggle_shadow(l_arm_shadow_mesh_field, false);
    toggle_shadow(r_arm_shadow_mesh_field, false);

    // Disable drawing of the player body if the user wants it.
    toggle_enabled(upper_body_mesh_field, !wants_hide_upper_body);
    toggle_enabled(lower_body_mesh_field, !wants_hide_lower_body);
    toggle_enabled(l_arm_mesh_field, !wants_hide_arms);
    toggle_enabled(r_arm_mesh_field, !wants_hide_arms);
}

::REManagedObject* RE8VR::get_localplayer() const {
#ifdef RE7
    auto object_man = sdk::get_managed_singleton<::REManagedObject>("app.ObjectManager");

    if (object_man == nullptr) {
        return nullptr;
    }

    static auto field = sdk::find_type_definition("app.ObjectManager")->get_field("PlayerObj");

    return field->get_data<::REManagedObject*>(object_man);
#else
    auto propsman = sdk::get_managed_singleton<::REManagedObject>("app.PropsManager");

    if (propsman == nullptr) {
        return nullptr;
    }

    static auto field = sdk::find_type_definition("app.PropsManager")->get_field("<Player>k__BackingField");

    return field->get_data<::REManagedObject*>(propsman);
#endif
}

::REManagedObject* RE8VR::get_weapon_object(::REGameObject* player) const {
#ifdef RE7
    static auto equip_manager_type = sdk::find_type_definition("app.EquipManager")->get_type();
    auto equip_manager = utility::re_component::find<::REManagedObject>(player->transform, equip_manager_type);

    if (equip_manager == nullptr) {
        return nullptr;
    }

    static auto get_equip_weapon_right_method = sdk::find_method_definition("app.EquipManager", "get_equipWeaponRight");

    return get_equip_weapon_right_method->call<::REManagedObject*>(sdk::get_thread_context(), equip_manager);
#else
    if (m_updater == nullptr) {
        return nullptr;
    }

    auto find_fps_method = [](std::string_view tname, std::string_view method_name) -> sdk::REMethodDefinition* {
        auto t = sdk::find_type_definition(tname);

        if (t == nullptr) {
            t = sdk::find_type_definition(std::string{ tname } + "FPS");
        }

        if (t == nullptr) {
            return nullptr;
        }

        return t->get_method(method_name);
    };

    if (m_player_type == PlayerType::ETHAN) {
        static auto get_player_gun_method = find_fps_method("app.PlayerUpdater", "get_playerGun");

        auto player_gun = get_player_gun_method->call<::REGameObject*>(sdk::get_thread_context(), m_updater);

        if (player_gun == nullptr) {
            return nullptr;
        }

        static auto get_equip_weapon_object_method = sdk::find_method_definition("app.PlayerGun", "get_equipWeaponObject");

        return get_equip_weapon_object_method->call<::REManagedObject*>(sdk::get_thread_context(), player_gun);
    } else if (m_player_type == PlayerType::CHRIS_MERC) {
        static auto get_player_gun_method = find_fps_method("app.PlayerUpdaterPl2001", "get_playerGun");

        auto player_gun = get_player_gun_method->call<::REGameObject*>(sdk::get_thread_context(), m_updater);

        if (player_gun == nullptr) {
            return nullptr;
        }

        static auto get_equip_weapon_object_method = sdk::find_method_definition("app.PlayerGunPl2001", "get_equipWeaponObject");

        return get_equip_weapon_object_method->call<::REManagedObject*>(sdk::get_thread_context(), player_gun);
    }

    return nullptr;
#endif
}

bool RE8VR::update_pointers() {
    m_player_downcast = get_localplayer();

    if (m_player == nullptr) {
        reset_data();
        return false;
    }
    
    m_transform = m_player->transform;

    if (m_transform == nullptr) {
        reset_data();
        return false;
    }

    m_delta_time = sdk::Application::get()->get_delta_time();

    auto get_ambiguous_re_type = [](std::string_view name) -> ::REType* {
        auto tdef = sdk::find_type_definition(name);

        if (tdef == nullptr) {
            tdef = sdk::find_type_definition(std::string{ name } + "FPS");
        }

        if (tdef == nullptr) {
            return nullptr;
        }

        return tdef->get_type();
    };

    auto assign_component = [this](::REManagedObject*& a, ::REType* t) {
        if (t == nullptr) {
            a = nullptr;
            return;
        }

        a = utility::re_component::find<::REManagedObject>(m_player->transform, t);
    };

    static auto hand_touch_type = get_ambiguous_re_type("app.PlayerHandTouch");
    assign_component(m_hand_touch, hand_touch_type);

    static auto updater_type = get_ambiguous_re_type("app.PlayerUpdater");
    assign_component(m_updater, updater_type);

#ifdef RE8
    if (m_updater == nullptr) {
        static auto updater_type_chris = get_ambiguous_re_type("app.PlayerUpdaterPl2001");
        assign_component(m_updater, updater_type_chris);

        if (m_updater != nullptr) {
            m_player_type = PlayerType::CHRIS_MERC;
        }
    } else {
        m_player_type = PlayerType::ETHAN;
    }
#else
    m_player_type = PlayerType::ETHAN;
#endif

    static auto order_type = get_ambiguous_re_type("app.PlayerOrder");
    assign_component(m_order, order_type);

#ifdef RE8
    if (m_order == nullptr) {
        static auto order_type_chris = get_ambiguous_re_type("app.PlayerOrderPl2001");
        assign_component(m_order, order_type_chris);
    }
#endif

    static auto event_action_controller_type = get_ambiguous_re_type("app.EventActionController");
    assign_component(m_event_action_controller, event_action_controller_type);

    static auto game_event_action_controller_type = get_ambiguous_re_type("app.GameEventActionController");
    assign_component(m_game_event_action_controller, game_event_action_controller_type);

#ifdef RE7
    static auto hit_controller_type = get_ambiguous_re_type("app.Collision.HitController");
    assign_component(m_hit_controller, hit_controller_type);
#else
    // TODO
#endif

#ifdef RE8
    if (m_game_event_action_controller != nullptr) {
        m_is_motion_play = *sdk::get_object_field<bool>(m_game_event_action_controller, "_isMotionPlay");
    }
#else 
    m_is_motion_play = false;
#endif

    if (m_order != nullptr) {
        m_is_grapple_aim = *sdk::get_object_field<bool>(m_order, "IsGrappleAimEnable");
    }

    m_weapon = get_weapon_object(m_player);

    static auto inventory_type = get_ambiguous_re_type("app.Inventory");

#ifdef RE7
    assign_component(m_inventory, inventory_type);

    static auto status_type = get_ambiguous_re_type("app.PlayerStatus");
    assign_component(m_status, status_type);
#else
    if (m_updater != nullptr) {
        m_status = sdk::call_object_func_easy<::REManagedObject*>(m_updater, "get_playerstatus");

        auto container = sdk::get_object_field<::REManagedObject*>(m_updater, "playerContainer");

        if (container == nullptr ) {
            container = sdk::get_object_field<::REManagedObject*>(m_updater, "<playerContainer>k__BackingField");
        }
        
        if (container != nullptr && *container != nullptr) {
            m_inventory = sdk::call_object_func_easy<::REManagedObject*>(*container, "get_inventory");
        } else {
            m_inventory = nullptr;
        }
    } else {
        m_status = nullptr;
        m_inventory = nullptr;
    }
#endif

    if (m_status != nullptr) {
        m_is_reloading = sdk::call_object_func_easy<bool>(m_status, "get_isReload");
    }

    update_ik_pointers();
    return true;
}

bool RE8VR::update_ik_pointers() {
    auto reset_hand_ik = [&]() {
        m_right_hand_ik = nullptr;
        m_left_hand_ik = nullptr;
        m_right_hand_ik_object = nullptr;
        m_left_hand_ik_object = nullptr;
        m_right_hand_ik_transform = nullptr;
        m_left_hand_ik_transform = nullptr;
    };

    if (m_hand_touch == nullptr) {
        reset_hand_ik();
        return false;
    }

    auto hand_ik = *sdk::get_object_field<sdk::SystemArray*>(m_hand_touch, "HandIK");

    if (hand_ik == nullptr || hand_ik->size() < 2) {
        spdlog::info("[RE8VR] HandIK is null or empty");
        reset_hand_ik();
        return false;
    }

    m_right_hand_ik = hand_ik->get_element(0);
    m_left_hand_ik = hand_ik->get_element(1);

    if (m_right_hand_ik != nullptr && m_left_hand_ik != nullptr) {
        m_right_hand_ik_object = *sdk::get_object_field<::REGameObject*>(m_right_hand_ik, "TargetGameObject");
        m_left_hand_ik_object = *sdk::get_object_field<::REGameObject*>(m_left_hand_ik, "TargetGameObject");
        m_right_hand_ik_transform = *sdk::get_object_field<::RETransform*>(m_right_hand_ik, "Target");
        m_left_hand_ik_transform = *sdk::get_object_field<::RETransform*>(m_left_hand_ik, "Target");
    } else {
        return false;
    }

    return true;
}

void RE8VR::update_block_gesture() {
    auto& vr = VR::get();

    const auto& controllers = vr->get_controllers();
    const auto left_hand = vr->get_transform(controllers[0]);
    const auto right_hand = vr->get_transform(controllers[1]);
    const auto hmd_forward = vr->get_transform(0)[2];

    const auto left_hand_dot_raw = glm::dot(Vector3f{hmd_forward}, m_hmd_dir_to_left);
    const auto right_hand_dot_raw = glm::dot(Vector3f{hmd_forward}, m_hmd_dir_to_right);
    const auto left_hand_dot = glm::abs(left_hand_dot_raw);
    const auto right_hand_dot = glm::abs(right_hand_dot_raw);

    const auto left_hand_in_front = left_hand_dot >= 0.8f;
    const auto right_hand_in_front = right_hand_dot >= 0.8f;

    const auto first_test = left_hand_in_front && right_hand_in_front;

    if (!first_test) {
        m_wants_block = false;
        return;
    }

    // now we need to check if the hands are facing up
    const auto left_hand_up_dot = glm::abs(glm::dot(hmd_forward, left_hand[0]));
    const auto right_hand_up_dot = glm::abs(glm::dot(hmd_forward, right_hand[0]));

    const auto left_hand_up = left_hand_up_dot >= 0.5f;
    const auto right_hand_up = right_hand_up_dot >= 0.5f;

    m_wants_block = left_hand_up && right_hand_up;
}

HookManager::PreHookResult RE8VR::pre_shadow_late_update(std::vector<uintptr_t>& args, std::vector<sdk::RETypeDefinition*>& arg_tys) {
    auto& vr = VR::get();
    auto& re8vr = RE8VR::get();

    if (re8vr->m_player == nullptr || re8vr->m_transform == nullptr) {
        return HookManager::PreHookResult::CALL_ORIGINAL;
    }

    if (!vr->is_using_controllers()) {
        return HookManager::PreHookResult::CALL_ORIGINAL;
    }

    if (!re8vr->m_is_in_cutscene && re8vr->m_can_use_hands && !re8vr->m_is_grapple_aim) {
        return HookManager::PreHookResult::SKIP_ORIGINAL;
    }

    return HookManager::PreHookResult::CALL_ORIGINAL;
}

void RE8VR::post_shadow_late_update(uintptr_t& ret_val, sdk::RETypeDefinition* ret_ty) {
}

void RE8VR::update_heal_gesture() {
#ifdef RE7
    if (m_inventory == nullptr) {
#else
    if (m_inventory == nullptr || m_updater == nullptr) {
#endif
        m_wants_heal = false;
        m_heal_gesture.was_grip_down = false;
        m_heal_gesture.was_trigger_down = false;

        return;
    }

    auto& vr = VR::get();

    const auto hmd_forward = vr->get_transform(0)[2];
    const auto flattened_forward = glm::normalize(Vector3f{hmd_forward.x, 0.0f, hmd_forward.z});

    const auto right_hand_dot_flat_raw = glm::dot(flattened_forward, m_hmd_dir_to_right);
    const auto right_hand_behind = right_hand_dot_flat_raw >= 0.2f;

    const auto right_joystick = vr->get_right_joystick();
    const auto action_trigger = vr->get_action_trigger();
    const auto action_grip = vr->get_action_grip();
    const auto is_trigger_down = vr->is_action_active(action_trigger, right_joystick);
    const auto is_grip_down = vr->is_action_active(action_grip, right_joystick);

#ifdef RE8
    static auto app_medicine_core = sdk::find_type_definition("app.MedicineCore");

    auto items_list = *sdk::get_object_field<::REManagedObject*>(m_inventory, "<items>k__BackingField");
    auto weapon_change = sdk::call_object_func_easy<::REManagedObject*>(m_updater, "get_playerWeaponChange");
    auto mesh_controller = sdk::call_object_func_easy<::REManagedObject*>(m_updater, "get_playerMeshController");

    if (mesh_controller == nullptr && m_order != nullptr) {
        mesh_controller =  sdk::call_object_func_easy<::REManagedObject*>(m_order, "get_playerMeshController");
    }
#else
    static auto app_player_weapon_change = sdk::find_type_definition("app.PlayerWeaponChange");
    static auto app_player_mesh_controller = sdk::find_type_definition("app.PlayerMeshController");
    static auto app_player_weapon_change_type = app_player_weapon_change->get_type();
    static auto app_player_mesh_controller_type = app_player_mesh_controller->get_type();

    auto items_list = *sdk::get_object_field<::REManagedObject*>(m_inventory, "_ItemList");
    auto weapon_change = utility::re_component::find<::REManagedObject>(m_player->transform, app_player_weapon_change_type);
    auto mesh_controller = utility::re_component::find<::REManagedObject>(m_player->transform, app_player_mesh_controller_type);
#endif

    if (items_list == nullptr || weapon_change == nullptr || mesh_controller == nullptr) {
        spdlog::info("[RE8VR] Could not find inventory, weapon change, or mesh controller");
        return;
    }

    const auto now = std::chrono::steady_clock::now();
    auto items = *sdk::get_object_field<sdk::SystemArray*>(items_list, "mItems");

    if (items == nullptr) {
        spdlog::info("[RE8VR] mItems is null");
        return;
    }

    ::REManagedObject* medicine_item = nullptr;

    for (auto i = 0; i < items->size(); i++) {
        auto item = items->get_element(i);

        if (item == nullptr) {
            continue;
        }

#ifdef RE8
        const auto is_medicine = utility::re_managed_object::get_type_definition(item)->is_a(app_medicine_core);
#else
        bool is_medicine = false;
        auto item_internal = *sdk::get_object_field<::REManagedObject*>(item, "Item");

        if (item_internal != nullptr) {
            auto item_name = *sdk::get_object_field<::SystemString*>(item_internal, "ItemDataID");

            if (item_name != nullptr) {
                is_medicine = utility::re_string::get_string(item_name).find("Remedy") != std::string::npos;
            }
        }
#endif

        if (is_medicine) {
            medicine_item = item;
            break;
        }
    }

    if (medicine_item == nullptr) {
        m_wants_heal = false;
        m_heal_gesture.was_grip_down = false;
        m_heal_gesture.was_trigger_down = false;

        return;
    }

#ifdef RE7
    auto item_internal = *sdk::get_object_field<::REManagedObject*>(medicine_item, "Item");
    auto owner = *sdk::get_object_field<::REGameObject*>(medicine_item, "Owner");
#else
    auto owner = *sdk::get_object_field<::REGameObject*>(medicine_item, "<owner>k__BackingField");
#endif

    if (owner == nullptr) {
        m_wants_heal = false;
        m_heal_gesture.was_grip_down = false;
        m_heal_gesture.was_trigger_down = false;

        spdlog::info("[RE8VR] Medicine has no owner");

        return;
    }

    static auto via_render_mesh = sdk::find_type_definition("via.render.Mesh");
    static auto via_render_mesh_type = via_render_mesh->get_type();

    auto current_mesh = *sdk::get_object_field<::REManagedObject*>(mesh_controller, "WeaponMesh");
    auto item_mesh = utility::re_component::find<::REManagedObject>(owner->transform, via_render_mesh_type);

    const auto is_same_mesh = current_mesh == item_mesh;

    if (current_mesh == nullptr) {
        m_heal_gesture.was_grip_down = false;
        m_heal_gesture.was_trigger_down = false;
    }

    auto dequip_item = [&]() {
#ifdef RE7
        auto equip_manager = utility::re_component::find<::REManagedObject>(m_player->transform, "app.EquipManager");
        if (equip_manager == nullptr) {
            return;
        }

        auto item_weapon = *sdk::get_object_field<::REManagedObject*>(item_internal, "<weapon>k__BackingField");

        sdk::call_object_func_easy<void*>(weapon_change, "removeWeapon");
        sdk::call_object_func_easy<void*>(mesh_controller, "onEquipWeaponChanged", nullptr, item_weapon);

        auto current_equipped_right = *sdk::get_object_field<::REManagedObject*>(equip_manager, "<equipWeaponRight>k__BackingField");

        if (current_equipped_right == item_weapon) {
           sdk::call_object_func_easy<void*>(equip_manager, "set_equipWeaponRight", nullptr);
        }
#endif
    };

    if (!is_same_mesh) {
        if (!is_trigger_down && (
            (right_hand_behind && !m_heal_gesture.was_grip_down && glm::length(m_hmd_delta_to_right) <= 0.35f) 
            || (now - m_heal_gesture.last_grab_time) < std::chrono::milliseconds(500))) 
        {
            vr->trigger_haptic_vibration(0.0f, 0.1f, 1.0f, 5.0f, right_joystick);

            if (is_grip_down) {
#ifdef RE8
                auto equip_manager = sdk::call_object_func_easy<::REManagedObject*>(m_updater, "get_equipController");

                if (equip_manager == nullptr && m_order != nullptr) {
                    equip_manager = sdk::call_object_func_easy<::REManagedObject*>(m_order, "get_equipController");
                }

                sdk::call_object_func_easy<void*>(medicine_item, "setActive", true);
                sdk::call_object_func_easy<void*>(weapon_change, "removeWeaponWithNoAction");

                if (equip_manager != nullptr) {
                    sdk::call_object_func_easy<void*>(equip_manager, "equipObject", owner);
                }

                m_heal_gesture.last_grab_time = now;
#else
                auto equip_manager = utility::re_component::find<::REManagedObject>(m_player->transform, "app.EquipManager");

                if (equip_manager == nullptr) {
                    spdlog::info("[RE8VR] No equip manager found");
                    return;
                }

                auto current_equipped_right = *sdk::get_object_field<::REManagedObject*>(equip_manager, "<equipWeaponRight>k__BackingField");
                auto item_weapon = *sdk::get_object_field<::REManagedObject*>(item_internal, "<weapon>k__BackingField");

                sdk::call_object_func_easy<void*>(weapon_change, "removeWeapon");
                
                try {
                    sdk::call_object_func_easy<void*>(equip_manager, "equipWeapon(app.Weapon, app.CharacterDefine.Hand)", item_weapon, 0);
                } catch(...) {}

                try {
                    sdk::call_object_func_easy<void*>(weapon_change, "equipWeapon", item_internal, item_weapon);
                } catch(...) {}

                if (m_weapon != nullptr) {
                    sdk::call_object_func_easy<void*>(mesh_controller, "onEquipWeaponChanged", item_weapon, m_weapon);
                }

                if (current_equipped_right != item_weapon) {
                    sdk::call_object_func_easy<void*>(equip_manager, "set_equipWeaponRight", item_weapon);
                }

                m_heal_gesture.last_grab_time = now;
#endif
            }
        }
    } else if (is_trigger_down) {
        if (!m_heal_gesture.was_trigger_down) {
#ifdef RE7
            auto item_weapon = *sdk::get_object_field<::REManagedObject*>(item_internal, "<weapon>k__BackingField");

            dequip_item();
            sdk::call_object_func_easy<void*>(weapon_change, "useItem", item_internal, item_weapon);
#else
            sdk::call_object_func_easy<void*>(weapon_change, "requestUseItem", medicine_item, false, false);
#endif
        }
    } else if (!is_grip_down) {
        if (is_same_mesh) {
#ifdef RE7
            auto item_weapon = *sdk::get_object_field<::REManagedObject*>(item_internal, "<weapon>k__BackingField");
            sdk::call_object_func_easy<void*>(weapon_change, "removeWeapon");
            sdk::call_object_func_easy<void*>(mesh_controller, "onEquipWeaponChanged", nullptr, item_weapon);
#else
            sdk::call_object_func_easy<void*>(weapon_change, "removeWeaponWithNoAction");
            sdk::call_object_func_easy<void*>(medicine_item, "setActive", false);
#endif
        }
    }

    m_heal_gesture.was_grip_down = is_grip_down && m_heal_gesture.last_grip_weapon == m_weapon; 
    m_heal_gesture.was_trigger_down = is_trigger_down && m_heal_gesture.last_grip_weapon == m_weapon;
    m_heal_gesture.last_grip_weapon = m_weapon;

#ifdef RE8
    static auto common_use_remedy_action = *sdk::get_static_field<::REManagedObject*>("app.PlayerDefineEnumLikeArray.UpperActionID", "CommonUseRemedy");

    const auto is_syringe = utility::re_string::get_string(owner->name) == "ri1022_Inventory";
    const auto upper_action_id = *sdk::get_object_field<::REManagedObject*>(m_status, "<upperActionID>k__BackingField");
    const auto using_effect = *sdk::get_object_field<::REManagedObject*>(medicine_item, "usingEffect") != nullptr
                                || upper_action_id == common_use_remedy_action;



    // In RE8 the medicine is rotated all weird.
    if (!is_trigger_down && !using_effect) {
        if (!is_syringe) {
            sdk::call_object_func_easy<void*>(owner->transform, "set_LocalRotation", &m_heal_gesture.re8_medicine_rotation);
        } else {
            sdk::call_object_func_easy<void*>(owner->transform, "set_LocalRotation", &m_heal_gesture.re8_syringe_rotation);
        }
    } else if (is_syringe && using_effect) {
        glm::quat zero_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        sdk::call_object_func_easy<void*>(owner->transform, "set_LocalRotation", &zero_rotation);
    }
#endif
}
#endif
