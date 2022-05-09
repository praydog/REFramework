#if defined(RE7) || defined(RE8)
#include <sdk/SceneManager.hpp>
#include <sdk/MurmurHash.hpp>
#include "../../../mods/VR.hpp"

#include "RE8VR.hpp"

std::shared_ptr<RE8VR>& RE8VR::get() {
    static std::shared_ptr<RE8VR> inst{};

    if (inst == nullptr) {
        inst = std::make_shared<RE8VR>();
    }

    return inst;
}

std::optional<std::string> RE8VR::on_initialize() {
    return std::nullopt;
}

void RE8VR::on_lua_state_created(sol::state& lua) {
    lua.new_usertype<RE8VR>("RE8VR",
        "player", &RE8VR::m_player_downcast,
        "inventory", &RE8VR::m_inventory,
        "updater", &RE8VR::m_updater,
        "weapon", &RE8VR::m_weapon,
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
        "was_gripping_weapon", &RE8VR::m_was_gripping_weapon,
        "is_holding_left_grip", &RE8VR::m_is_holding_left_grip,
        "is_in_cutscene", &RE8VR::m_is_in_cutscene,
        "is_grapple_aim", &RE8VR::m_is_grapple_aim,
        "is_reloading", &RE8VR::m_is_reloading,
        "can_use_hands", &RE8VR::m_can_use_hands,
        "wants_block", &RE8VR::m_wants_block,
        "wants_heal", &RE8VR::m_wants_heal,
        "set_hand_joints_to_tpose", &RE8VR::set_hand_joints_to_tpose,
        "update_hand_ik", &RE8VR::update_hand_ik,
        "update_body_ik", &RE8VR::update_body_ik,
        "update_player_gestures", &RE8VR::update_player_gestures);

    lua["re8vr"] = this;
}

void RE8VR::on_lua_state_destroyed(sol::state& lua) {
    
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
    static auto motion_get_joint_index_by_name_hash = sdk::find_type_definition("via.motion.Motion")->get_method("getJointIndexByNameHash");
    static auto motion_get_world_position = sdk::find_type_definition("via.motion.Motion")->get_method("getWorldPosition");
    static auto motion_get_world_rotation = sdk::find_type_definition("via.motion.Motion")->get_method("getWorldRotation");
    static auto motion_typedef = sdk::find_type_definition("via.motion.Motion");
    static auto motion_type = motion_typedef->get_type();
    static uint32_t head_hash = 0;

    auto vr = VR::get();
    
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

    if (!m_can_use_hands) {
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

    static auto via_motion_ik_leg = sdk::find_type_definition("via.motion.IkLeg");
    static auto via_motion_ik_leg_type = via_motion_ik_leg->get_type();
    static auto via_motion_motion = sdk::find_type_definition("via.motion.Motion");
    static auto via_motion_motion_type = via_motion_motion->get_type();
    static auto ik_leg_set_center_offset = via_motion_ik_leg->get_method("set_CenterOffset");
    static auto ik_leg_set_center_adjust = via_motion_ik_leg->get_method("setCenterAdjust");
    static auto ik_leg_set_center_position_ctrl = via_motion_ik_leg->get_method("set_CenterPositionCtrl");
    static auto ik_leg_set_ground_contact_up_distance = via_motion_ik_leg->get_method("set_GroundContactUpDistance");
    static auto ik_leg_set_enabled = via_motion_ik_leg->get_method("set_Enabled");

    auto vr = VR::get();
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

    if (m_is_in_cutscene || !vr->is_hmd_active() || !vr->is_using_controllers()) {
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
    auto vr = VR::get();

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

void RE8VR::update_block_gesture() {
    auto vr = VR::get();

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

    auto vr = VR::get();

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
            if (current_equipped_right != nullptr) {
                utility::re_managed_object::release(current_equipped_right);
            }

            *sdk::get_object_field<::REManagedObject*>(equip_manager, "<equipWeaponRight>k__BackingField") = nullptr;
        }
#endif
    };

    if (!is_same_mesh) {
        if ((right_hand_behind && !m_heal_gesture.was_grip_down && glm::length(m_hmd_delta_to_right) <= 1.0f) 
            || (now - m_heal_gesture.last_grab_time) < std::chrono::milliseconds(500)) 
        {
            vr->trigger_haptic_vibration(0.0f, 0.1f, 1.0f, 5.0f, right_joystick);

            if (is_grip_down) {
#ifdef RE8
                auto equip_manager = sdk::call_object_func_easy<::REManagedObject*>(m_updater, "get_equipController");

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
                sdk::call_object_func_easy<void*>(equip_manager, "equipWeapon(app.Weapon, app.CharacterDefine.Hand)", item_weapon, 0);
                sdk::call_object_func_easy<void*>(weapon_change, "equipWeapon", item_internal, item_weapon);

                if (m_weapon != nullptr) {
                    sdk::call_object_func_easy<void*>(mesh_controller, "onEquipWeaponChanged", item_weapon, m_weapon);
                }

                if (current_equipped_right != item_weapon) {
                    if (current_equipped_right != nullptr) {
                        utility::re_managed_object::release(current_equipped_right);
                    }

                    *sdk::get_object_field<::REManagedObject*>(equip_manager, "<equipWeaponRight>k__BackingField") = item_weapon;
                    utility::re_managed_object::add_ref(item_weapon);
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
    // In RE8 the medicine is rotated all weird.
    if (!is_trigger_down && *sdk::get_object_field<::REManagedObject*>(medicine_item, "usingEffect") == nullptr) {
        sdk::call_object_func_easy<void*>(owner->transform, "set_LocalRotation", &m_heal_gesture.re8_medicine_rotation);
    }
#endif
}
#endif