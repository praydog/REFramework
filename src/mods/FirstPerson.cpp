#include <unordered_set>

#include <spdlog/spdlog.h>
#include <imgui.h>

#include "utility/Scan.hpp"
#include "REFramework.hpp"
#include "sdk/REMath.hpp"
#include "sdk/MurmurHash.hpp"
#include "sdk/Application.hpp"

#include "VR.hpp"
#include "FirstPerson.hpp"

#if defined(RE2) || defined(RE3)

FirstPerson* g_first_person = nullptr;

std::shared_ptr<FirstPerson>& FirstPerson::get() {
    static std::shared_ptr<FirstPerson> inst{};

    if (inst == nullptr) {
        inst = std::make_shared<FirstPerson>();
    }

    return inst;
}

FirstPerson::FirstPerson() {
    // thanks imgui
    g_first_person = this;
    m_attach_bone_imgui.reserve(256);

#ifdef RE3
    // Carlos
    m_attach_offsets["pl0000"] = Vector4f{ 0.0f, 0.667f, 1.1f, 0.0f };
    // Jill (it looks better with 0?)
    m_attach_offsets["pl2000"] = Vector4f{ 0.0f, 0.5f, 0.776f, 0.0f };
    //m_attach_offsets["pl2000"] = Vector4f{ -0.23f, 0.4f, 1.0f, 0.0f };
#else
    // Specific player model configs
    // Leon
    m_attach_offsets["pl0000"] = Vector4f{ -0.26f, 0.435f, 1.0f, 0.0f };
    // Claire
    m_attach_offsets["pl1000"] = Vector4f{ -0.23f, 0.4f, 1.0f, 0.0f };
    // Sherry
    m_attach_offsets["pl3000"] = Vector4f{ -0.278f, 0.435f, 0.945f, 0.0f };
    // Hunk
    m_attach_offsets["pl4000"] = Vector4f{ -0.26f, 0.435f, 1.0f, 0.0f };
    // Kendo
    m_attach_offsets["pl5000"] = Vector4f{ -0.24f, 0.4f, 1.0f, 0.0f };
    // Forgotten Soldier
    m_attach_offsets["pl5600"] = Vector4f{ -0.316, 0.556f, 1.02f, 0.0f };
    // Elizabeth
    m_attach_offsets["pl6400"] = Vector4f{ -0.316, 0.466f, 0.79f, 0.0f };
#endif
}

void FirstPerson::toggle() {
    if (!m_enabled->toggle()) {
        on_disabled();
    }
}

std::optional<std::string> FirstPerson::on_initialize() {
    /*auto vignetteCode = utility::scan(g_framework->getModule().as<HMODULE>(), "8B 87 3C 01 00 00 89 83 DC 00 00 00");

    if (!vignetteCode) {
        return "Failed to find Disable Vignette pattern";
    }

    // xor eax, eax
    m_disable_vignettePatch = Patch::create(*vignetteCode, { 0x31, 0xC0, 0x90, 0x90, 0x90, 0x90 }, false);*/

    return Mod::on_initialize();
}

void FirstPerson::on_frame() {
}

void FirstPerson::on_draw_ui() {
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);

    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    std::lock_guard _{ m_frame_mutex };

    /*auto last_controller_matrix = glm::extractMatrixRotation(Matrix4x4f{ m_last_controller_rotation });

    // Create 4 DragFloat4's for the 4x4 matrix
    for (int i = 0; i < 4; i++) {
        auto elem = last_controller_matrix[i];
        ImGui::DragFloat4("##", (float*)&elem, 1.0f, -1.0f, 1.0f);
    }*/

    if (m_enabled->draw("Enabled")) {
        // Disable fov and camera light changes
        m_wants_disable = !m_enabled->value();
    }

    ImGui::SameLine();

    // Revert the updateCamera value to normal
    if (m_show_in_cutscenes->draw("Show In Cutscenes") && m_camera_system != nullptr && m_camera_system->mainCameraController != nullptr) {
        m_camera_system->mainCameraController->updateCamera = true;
    }

    ImGui::Separator();
    ImGui::Text("VR Specific Settings");

    m_smooth_xz_movement->draw("Smooth XZ Movement (VR)");
    m_smooth_y_movement->draw("Smooth Y Movement (VR)");
    m_roomscale->draw("Roomscale Movement (VR)");

    static bool adjust_hand_offset{false};
    ImGui::Checkbox("Adjust Hand Offset", &adjust_hand_offset);

    if (adjust_hand_offset) {
        auto vr = VR::get();
        const auto left_axis = vr->get_left_stick_axis();
        const auto right_axis = vr->get_right_stick_axis();
        const auto right_joystick = vr->get_right_joystick();
        const auto left_joystick = vr->get_left_joystick();
        const auto action_grip = vr->get_action_grip();
        const auto action_trigger = vr->get_action_trigger();

        const auto is_right_grip_active = vr->is_action_active(action_grip, right_joystick);
        const auto is_left_grip_active = vr->is_action_active(action_grip, left_joystick);
        const auto is_right_trigger_active = vr->is_action_active(action_trigger, right_joystick);
        const auto is_left_trigger_active = vr->is_action_active(action_trigger, left_joystick);

        // adjust the rotation offset based on how the user is moving the controller
        if (!is_right_trigger_active) {
            if (!is_right_grip_active) {
                m_right_hand_rotation_offset.x = m_right_hand_rotation_offset.x + (right_axis.y * 0.001);
                m_right_hand_rotation_offset.y = m_right_hand_rotation_offset.y + (right_axis.x * 0.001);
            }
            else {
                m_right_hand_rotation_offset.z = m_right_hand_rotation_offset.z + ((right_axis.y + right_axis.x) * 0.001);
            }
        } else {
            if (!is_right_grip_active) {
                m_right_hand_position_offset.x = m_right_hand_position_offset.x + (right_axis.y * 0.001);
                m_right_hand_position_offset.y = m_right_hand_position_offset.y + (right_axis.x * 0.001);
            }
            else {
                m_right_hand_position_offset.z = m_right_hand_position_offset.z + ((right_axis.y + right_axis.x) * 0.001);
            }
        }

        if (!is_left_trigger_active) {
            if (!is_left_grip_active) {
                m_left_hand_rotation_offset.x = m_left_hand_rotation_offset.x + (left_axis.y * 0.001);
                m_left_hand_rotation_offset.y = m_left_hand_rotation_offset.y + (left_axis.x * 0.001);
            } else {
                m_left_hand_rotation_offset.z = m_left_hand_rotation_offset.z + ((left_axis.y + left_axis.x) * 0.001);
            }
        } else {
            if (!is_left_grip_active) {
                m_left_hand_position_offset.x = m_left_hand_position_offset.x + (left_axis.y * 0.001);
                m_left_hand_position_offset.y = m_left_hand_position_offset.y + (left_axis.x * 0.001);
            } else {
                m_left_hand_position_offset.z = m_left_hand_position_offset.z + ((left_axis.y + left_axis.x) * 0.001);
            }
        }
    }

    ImGui::DragFloat4("Scale Debug", (float*)&m_scale_debug.x, 1.0f, -1.0f, 1.0f);
    ImGui::DragFloat4("Scale Debug 2", (float*)&m_scale_debug2.x, 1.0f, -1.0f, 1.0f);
    ImGui::DragFloat4("Offset Debug", (float*)&m_offset_debug.x, 1.0f, -1.0f, 1.0f);
    ImGui::DragFloat("VR Scale", (float*)&m_vr_scale, 0.01f, 0.01f, 1.0f);

    ImGui::DragFloat3("Controller rotation (Left)", (float*)&m_left_hand_rotation_offset, 0.1f, -360.0f, 360.0f);
    ImGui::DragFloat3("Controller rotation (Right)", (float*)&m_right_hand_rotation_offset, 0.1f, -360.0f, 360.0f);
    ImGui::DragFloat3("Controller position (Left)", (float*)&m_left_hand_position_offset, 0.01f, -2.0f, 2.0f);
    ImGui::DragFloat3("Controller position (Right)", (float*)&m_right_hand_position_offset, 0.01f, -2.0f, 2.0f);

    ImGui::DragFloat3("Controller 1", (float*)&m_last_controller_euler[0].x, 1.0f, -360.0f, 360.0f);
    ImGui::DragFloat3("Controller 2", (float*)&m_last_controller_euler[1].x, 1.0f, -360.0f, 360.0f);

    ImGui::Separator();
    ImGui::Text("General Settings");

    m_disable_light_source->draw("Disable Camera Light");
    m_hide_mesh->draw("Hide Joint Mesh");

    ImGui::SameLine();
    m_rotate_mesh->draw("Force Rotate Joint");
    m_rotate_body->draw("Rotate Body");

    if (m_disable_vignette->draw("Disable Vignette") && m_disable_vignette->value() == false) {
        set_vignette(via::render::ToneMapping::Vignetting::KerarePlus);
    }

    m_toggle_key->draw("Change Toggle Key");

    ImGui::SliderFloat3("CameraOffset", (float*)&m_attach_offsets[m_player_name], -2.0f, 2.0f, "%.3f", 1.0f);

    m_camera_scale->draw("CameraSpeed");
    m_bone_scale->draw("CameraShake");

    if (m_camera_system != nullptr) {
        if (m_fov_offset->draw("FOVOffset")) {
            update_fov(m_camera_system->cameraController);
        }

        if (m_fov_mult->draw("FOVMultiplier")) {
            update_fov(m_camera_system->cameraController);
            m_last_fov_mult = m_fov_mult->value();
        }

        m_current_fov->value() = m_camera_system->cameraController->activeCamera->fov;
        m_current_fov->draw("CurrentFOV");
    }

    m_body_rotate_speed->draw("BodyRotateSpeed");

    if (ImGui::InputText("Joint", m_attach_bone_imgui.data(), 256)) {
        m_attach_bone = std::wstring{ std::begin(m_attach_bone_imgui), std::end(m_attach_bone_imgui) };
    }

    static auto list_box_handler = [](void* data, int idx, const char** out_text) -> bool {
        return g_first_person->list_box_handler_attach(data, idx, out_text);
    };

    if (ImGui::ListBox("Joints", &m_attach_selected, list_box_handler, &m_attach_names, (int32_t)m_attach_names.size())) {
        m_attach_bone_imgui = m_attach_names[m_attach_selected];
        m_attach_bone = std::wstring{ std::begin(m_attach_names[m_attach_selected]), std::end(m_attach_names[m_attach_selected]) };
    }
}

void FirstPerson::on_lua_state_created(sol::state& state) {
    state.new_usertype<FirstPerson>("FirstPerson",
        "new", sol::no_constructor,
        "is_enabled", &FirstPerson::is_enabled,
        "will_be_used", &FirstPerson::will_be_used,
        "on_pre_flashlight_apply_transform", &FirstPerson::on_pre_flashlight_apply_transform
    );

    state["firstpersonmod"] = this;

    // this may seem counterintuitive
    // but it's the easiest way to do this.
    // it will fix the positioning and rotation of the flashlight's light
    // when using VR controllers
    // one of the main reasons for this is to allow
    // compatibility with Lua's sdk.hook which supports multiple hook callbacks under one hook
#ifdef RE2
    try {
        state.do_string(R"(
            -- re3 doesnt have handheld flashlights
            if reframework:get_game_name() ~= "re2" then return end

            local function on_pre_apply_transform(args)
                local flashlight = sdk.to_managed_object(args[2])

                if not firstpersonmod:on_pre_flashlight_apply_transform(flashlight) then
                    return sdk.PreHookResult.SKIP_ORIGINAL
                end
            end

            local function on_post_apply_transform(retval)
                return retval
            end

            sdk.hook(sdk.find_type_definition(sdk.game_namespace("FlashLight")):get_method("applyTransform"), on_pre_apply_transform, on_post_apply_transform)
        )");
    } catch (const std::exception& e) {
        spdlog::info("Error while trying to hook FlashLight.applyTransform: {}", e.what());
    }
#endif
}

void FirstPerson::on_config_load(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }

    m_last_fov_mult = m_fov_mult->value();

    // turn the patch on
    if (m_disable_vignette->value()) {
        //m_disable_vignettePatch->toggle(m_disable_vignette->value());
    }

    if (VR::get()->is_openxr_loaded()) {
        m_right_hand_rotation_offset = Vector3f{ 0.28f, -2.982f, -1.495f };
        m_left_hand_rotation_offset = Vector3f{ m_right_hand_rotation_offset.x + 0.2f, -(m_right_hand_rotation_offset.y + 0.1f), -m_right_hand_rotation_offset.z };
        m_right_hand_position_offset = Vector4f{ 0.052f, 0.084f, 0.02f, 0.0f };
        m_left_hand_position_offset = Vector4f{ -m_right_hand_position_offset.x, m_right_hand_position_offset.y, m_right_hand_position_offset.z, 0.0f };
    }
}

void FirstPerson::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}

thread_local bool g_in_player_transform = false;
thread_local bool g_first_time = true;
thread_local glm::quat g_old_rotation{};

void FirstPerson::on_pre_update_transform(RETransform* transform) {
    if (!m_enabled->value() || m_camera == nullptr || m_camera->ownerGameObject == nullptr) {
        return;
    }

    if (m_player_camera_controller == nullptr || m_player_transform == nullptr || m_camera_system == nullptr || m_camera_system->cameraController == nullptr) {
        return;
    }

    // We need to lock a mutex because these UpdateTransform functions
    // are called from multiple threads
    if (transform == m_player_transform) {
        if (!is_first_person_allowed()) {
            return;
        }

        g_in_player_transform = true;
        g_first_time = true;
        m_matrix_mutex.lock();

        // Update this beforehand so we don't see the player's head disappear when using the inventory
        const auto gui_state = *sdk::get_object_field<int32_t>(m_gui_master, "<State_>k__BackingField");
        const auto is_paused = gui_state == (int32_t)app::ropeway::gui::GUIMaster::GuiState::PAUSE || gui_state == (int32_t)app::ropeway::gui::GUIMaster::GuiState::INVENTORY;

        m_last_pause_state = is_paused;
        m_last_camera_type = utility::re_managed_object::get_field<app::ropeway::camera::CameraControlType>(m_camera_system, "BusyCameraType");

        m_cached_bone_matrix = nullptr;

        update_player_vr(m_player_transform);
        update_player_body_rotation(m_player_transform);
    }
    // This is because UpdateTransform recursively calls UpdateTransform on its children,
    // and the player transform (topmost) is the one that actually updates the bone matrix,
    // and all the child transforms operate on the bones that it updated
    else if (g_in_player_transform) {
        if (!is_first_person_allowed()) {
            return;
        }

        /*if (g_first_time) {
            g_old_rotation = *(glm::quat*)&m_player_transform->angles;
        }*/

        //update_player_transform(m_player_transform);
        update_player_bones(m_player_transform);
    }
}

void FirstPerson::on_update_transform(RETransform* transform) {
    // Do this first before anything else.
    if (g_in_player_transform && transform == m_player_transform) {
        // By also updating this in here, it fixes the out of sync issue sometimes seen for one frame
        // where the player body appears slightly ahead of the camera
        // This should especially help with frametime fluctuations
        if (m_camera_system != nullptr && m_camera_system->mainCamera != nullptr && m_camera_system->mainCamera->ownerGameObject != nullptr) {
            update_camera_transform(m_camera_system->mainCamera->ownerGameObject->transform);
        }

        update_joint_names();
        //update_player_transform(transform);

        //transform->angles = *(Vector4f*)&g_old_rotation;

        g_in_player_transform = false;
        m_matrix_mutex.unlock();
    }

    if (m_disable_vignette->value() && m_post_effect_controller != nullptr && transform == m_post_effect_controller->ownerGameObject->transform) {
        set_vignette(via::render::ToneMapping::Vignetting::Disable);
    }

    if (!m_enabled->value()) {
        return;
    }

    if (m_sweet_light_manager == nullptr || m_gui_master == nullptr) {
        return;
    }

    if (m_camera == nullptr || m_camera->ownerGameObject == nullptr) {
        return;
    }

    if (m_player_camera_controller == nullptr || m_player_transform == nullptr || m_camera_system == nullptr || m_camera_system->cameraController == nullptr) {
        return;
    }

    // Remove the camera light if specified
    if (transform == m_sweet_light_manager->ownerGameObject->transform) {
        // SweetLight
        update_sweet_light_context(utility::ropeway_sweetlight_manager::get_context(m_sweet_light_manager, 0));
        // EnvironmentSweetLight
        update_sweet_light_context(utility::ropeway_sweetlight_manager::get_context(m_sweet_light_manager, 1));
    }

    if (transform == m_camera_system->mainCamera->ownerGameObject->transform) {
        update_camera_transform(transform);
        update_fov(m_camera_system->cameraController);
    }
}

void FirstPerson::on_update_camera_controller(RopewayPlayerCameraController* controller) {
    if (!m_enabled->value() || m_player_transform == nullptr || controller != m_camera_system->cameraController) {
        return;
    }

    std::lock_guard _{ m_matrix_mutex };

    auto nudged_mtx = m_last_camera_matrix;
    //nudged_mtx[3] += VR::get()->get_current_offset();

    // The following code fixes inaccuracies between the rotation set by the game and what's set in updateCameraTransform
    //controller->worldPosition = nudged_mtx[3];
    //*(glm::quat*)&controller->worldRotation = glm::quat{ nudged_mtx  };

    //m_last_controller_pos = controller->worldPosition;
    m_last_controller_rotation = *(glm::quat*)&controller->worldRotation;

    update_camera_transform(m_camera->ownerGameObject->transform);

    if (!will_be_used() && !m_last_pause_state) {
        return;
    }

    //m_camera->ownerGameObject->transform->worldTransform = nudged_mtx;
    //m_camera->ownerGameObject->transform->angles = *(Vector4f*)&controller->worldRotation;
}

void FirstPerson::on_update_camera_controller2(RopewayPlayerCameraController* controller) {
    if (!m_enabled->value() || m_player_transform == nullptr || controller != m_camera_system->cameraController) {
        return;
    }

    std::lock_guard _{ m_matrix_mutex };

    const auto allowed = is_first_person_allowed();

    if (!allowed && !m_ignore_next_player_angles) {
        m_last_controller_angles = Vector3f{ controller->pitch, controller->yaw, 0.0f };
        m_last_controller_pos = controller->worldPosition;
        m_last_controller_rotation = *(glm::quat*) & controller->worldRotation;
        return;
    }

    if (allowed) {
        // Just update the FOV in here. Whatever.
        update_fov(controller);
    }

    if (m_camera_system->cameraController == m_player_camera_controller) {
        m_ignore_next_player_angles = m_ignore_next_player_angles || utility::re_managed_object::get_field<bool>(m_camera_system->mainCameraController, "SwitchingCamera");

        if (m_ignore_next_player_angles) {
            // keep ignoring player input until no longer switching cameras
            if (m_ignore_next_player_angles) {
                m_ignore_next_player_angles = false;
            }

            *(glm::quat*)&controller->worldRotation = m_last_controller_rotation;
            controller->pitch = m_last_controller_angles.x;
            controller->yaw = m_last_controller_angles.y;
        }
        else {
            m_ignore_next_player_angles = false;
        }

        m_last_controller_angles = Vector3f{ controller->pitch, controller->yaw, 0.0f };
    }

    m_last_controller_pos = controller->worldPosition;
    m_last_controller_rotation = *(glm::quat*) & controller->worldRotation;
}

void FirstPerson::on_pre_application_entry(void* entry, const char* name, size_t hash) {
    switch (hash) {
        case "UpdateBehavior"_fnv:
            on_pre_update_behavior(entry);
            break;
        case "LateUpdateBehavior"_fnv:
            on_pre_late_update_behavior(entry);
            break;
        case "UnlockScene"_fnv:
            on_pre_unlock_scene(entry);
            break;
        default:
            break;
    }
}

void FirstPerson::on_application_entry(void* entry, const char* name, size_t hash) {
    switch (hash) {
        case "UpdateMotion"_fnv:
            on_post_update_motion(entry, true); // fixes like literally every problem ever to exist
            break;
        case "LateUpdateBehavior"_fnv:
            on_post_late_update_behavior(entry);
            break;
        default:
            break;
    }
}

void FirstPerson::on_pre_update_behavior(void* entry) {
    if ((m_toggle_key->is_key_down_once() && !m_enabled->toggle()) || m_wants_disable) {
        on_disabled();
    }

    if (!m_enabled->value()) {
        return;
    }

    update_pointers();
}

void FirstPerson::on_pre_late_update_behavior(void* entry) {
    on_post_update_motion(entry);
}

void FirstPerson::on_pre_unlock_scene(void* entry) {
    update_player_roomscale(m_player_transform);
}

void FirstPerson::on_post_late_update_behavior(void* entry) {
    
}

void FirstPerson::on_post_update_motion(void* entry, bool true_motion) {
    if (!will_be_used()) {
        m_was_gripping_weapon = false;
        return;
    }

    // check it every time i guess becuase who knows what's going to happen.
    if (!update_pointers()) {
        m_was_gripping_weapon = false;
        return;
    }

    if (m_player_transform != nullptr) {
        if (m_camera_system != nullptr && m_camera_system->mainCamera != nullptr && m_camera_system->mainCamera->ownerGameObject != nullptr) {
            auto player_object = m_player_transform->ownerGameObject;

            if (player_object != nullptr) {
                update_player_vr(m_player_transform, true_motion);
            }
        }
    }
}

bool FirstPerson::on_pre_flashlight_apply_transform(::REManagedObject* flashlight_component) {
    if (!will_be_used()) {
        return true;
    }

    auto& vr_mod = VR::get();

    if (!vr_mod->is_hmd_active() || !vr_mod->is_using_controllers()) {
        return true;
    }

    if (m_was_gripping_weapon) {
        return true;
    }

    static auto via_render_mesh = sdk::find_type_definition("via.render.Mesh");
    static auto via_render_mesh_enabled = via_render_mesh->get_method("get_Enabled");

    auto flashlight_go = ((::REComponent*)flashlight_component)->ownerGameObject;
    if (flashlight_go == nullptr) {
        return true;
    }

    auto flashlight_transform = flashlight_go->transform;
    if (flashlight_transform == nullptr) {
        return true;
    }

    auto flashlight_mesh = utility::re_component::find(flashlight_transform, via_render_mesh->get_type());
    if (flashlight_mesh == nullptr) {
        return true;
    }

    const auto is_mesh_visible = via_render_mesh_enabled->call<bool>(sdk::get_thread_context(), flashlight_mesh);

    // if the mesh isn't being drawn, let the game handle it
    // this will happen if the player is holding a two handed weapon and aiming it
    if (!is_mesh_visible) {
        return true;
    }

    auto ies_light = *sdk::get_object_field<::REComponent*>(flashlight_component, "<IESLight>k__BackingField");
    if (ies_light == nullptr) {
        return true;
    }

    auto ies_light_go = ies_light->ownerGameObject;
    if (ies_light_go == nullptr) {
        return true;
    }

    auto ies_light_transform = ies_light_go->transform;
    if (ies_light_transform == nullptr) {
        return true;
    }

    static auto root_hash = sdk::murmur_hash::calc32("root");
    static auto via_transform = sdk::find_type_definition("via.Transform");
    static auto via_transform_get_joint_by_hash = via_transform->get_method("getJointByHash");

    auto root_joint = via_transform_get_joint_by_hash->call<::REJoint*>(sdk::get_thread_context(), flashlight_transform, root_hash);
    if (root_joint == nullptr) {
        return true;
    }

    Vector4f light_direction{};
    sdk::call_object_func<Vector4f*>(ies_light, "get_Direction", &light_direction, sdk::get_thread_context(), ies_light);

    const auto flashlight_position = sdk::get_joint_position(root_joint);
    const auto flashlight_rotation = sdk::get_joint_rotation(root_joint);
    const auto light_offset = flashlight_rotation * *sdk::get_object_field<Vector4f>(flashlight_component, "_IESLightOffset");
    const auto light_rot_offset = utility::math::to_quat(light_direction);

    sdk::set_transform_position(ies_light_transform, flashlight_position + light_offset);
    sdk::set_transform_rotation(ies_light_transform, flashlight_rotation * light_rot_offset);

    // do not call the original function
    return false;
}

void FirstPerson::reset() {
    m_rotation_offset = glm::identity<decltype(m_rotation_offset)>();
    m_interpolated_bone = glm::identity<decltype(m_interpolated_bone)>();
    m_last_camera_matrix = glm::identity<decltype(m_last_camera_matrix)>();
    m_last_camera_matrix_pre_vr = glm::identity<decltype(m_last_camera_matrix_pre_vr)>();
    m_last_headset_rotation_pre_cutscene = glm::identity<decltype(m_last_headset_rotation_pre_cutscene)>();
    m_last_bone_matrix = glm::identity<decltype(m_last_bone_matrix)>();
    m_last_controller_pos = Vector4f{};
    m_last_controller_rotation = glm::quat{};
    m_last_controller_rotation_vr = glm::quat{};
    m_cached_bone_matrix = nullptr;

    std::lock_guard _{ m_frame_mutex };
    m_attach_names.clear();
}

void FirstPerson::set_vignette(via::render::ToneMapping::Vignetting value) {
    // Assign tone mapping controller
    if (m_tone_mapping_controller == nullptr && m_post_effect_controller != nullptr) {
        m_tone_mapping_controller = utility::re_component::find<RopewayPostEffectControllerBase>(m_post_effect_controller, game_namespace("posteffect.ToneMapController"));
    }

    if (m_tone_mapping_controller == nullptr) {
        return;
    }

    // Overwrite vignetting
    auto update_param = [&value](auto param) {
        if (param == nullptr) {
            return;
        }

        ((RopewayPostEffectToneMapping*)param)->vignetting = (int32_t)value;
    };

    update_param(m_tone_mapping_controller->param1);
    update_param(m_tone_mapping_controller->param2);
    update_param(m_tone_mapping_controller->param3);
    update_param(m_tone_mapping_controller->param4);
    update_param(m_tone_mapping_controller->filterSetting->param);
    update_param(m_tone_mapping_controller->filterSetting->currentParam);
    update_param(m_tone_mapping_controller->filterSetting->param1);
    update_param(m_tone_mapping_controller->filterSetting->param2);
}

bool FirstPerson::update_pointers() {
    // Update our global pointers
    if (m_post_effect_controller == nullptr || m_post_effect_controller->ownerGameObject == nullptr || 
        m_camera_system == nullptr || m_camera_system->ownerGameObject == nullptr || m_sweet_light_manager == nullptr || m_sweet_light_manager->ownerGameObject == nullptr
        || m_gui_master == nullptr) 
    {
        auto& globals = *reframework::get_globals();
        m_sweet_light_manager = globals.get<RopewaySweetLightManager>(game_namespace("SweetLightManager"));
        m_camera_system = globals.get<RopewayCameraSystem>(game_namespace("camera.CameraSystem"));
        m_post_effect_controller = globals.get<RopewayPostEffectController>(game_namespace("posteffect.PostEffectController"));
        m_gui_master = globals.get<REBehavior>(game_namespace("gui.GUIMaster"));

        reset();
        return false;
    }

    if (m_camera_system != nullptr && m_camera_system->ownerGameObject != nullptr) {
        if (!update_pointers_from_camera_system(m_camera_system)) {
            reset();
            return false;
        }

        return true;
    }

    reset();
    return false;
}

bool FirstPerson::update_pointers_from_camera_system(RopewayCameraSystem* camera_system) {
    if (camera_system == nullptr) {
        return false;
    }

    if (m_camera = camera_system->mainCamera; m_camera == nullptr) {
        m_player_transform = nullptr;
        return false;
    }

    auto joint = camera_system->playerJoint;
    
    if (joint == nullptr) {
        m_player_transform = nullptr;
        return false;
    }

    // Update player name and log it
    if (m_player_transform != joint->parentTransform && joint->parentTransform != nullptr) {
        if (joint->parentTransform->ownerGameObject == nullptr) {
            return false;
        }

        m_player_name = utility::re_string::get_string(joint->parentTransform->ownerGameObject->name);

        if (m_player_name.empty()) {
            return false;
        }

        spdlog::info("Found Player {:s} {:p}", m_player_name.data(), (void*)joint->parentTransform);
    }

    // Update player transform pointer
    if (m_player_transform = joint->parentTransform; m_player_transform == nullptr) {
        return false;
    }

    // Update PlayerCameraController camera pointer
    if (m_player_camera_controller == nullptr) {
        auto controller = camera_system->cameraController;

        if (controller == nullptr || controller->ownerGameObject == nullptr || controller->activeCamera == nullptr || controller->activeCamera->ownerGameObject == nullptr) {
            return false;
        }

        if (utility::re_string::equals(controller->activeCamera->ownerGameObject->name, L"PlayerCameraController")) {
            m_player_camera_controller = controller;
            spdlog::info("Found PlayerCameraController {:p}", (void*)m_player_camera_controller);
        }

        return m_player_camera_controller != nullptr;
    }

    return true;
}

void FirstPerson::update_player_vr(RETransform* transform, bool first) {
    update_player_body_ik(transform, false, first);
    update_player_muzzle_behavior(transform);

    m_was_hmd_active = VR::get()->is_hmd_active();
}

void FirstPerson::update_player_arm_ik(RETransform* transform) {
    if (!m_enabled->value() || m_camera_system == nullptr) {
        m_was_gripping_weapon = false;
        return;
    }

    // so we don't go spinning everywhere in cutscenes
    if (m_last_camera_type != app::ropeway::camera::CameraControlType::PLAYER) {
        m_was_gripping_weapon = false;
        return;
    }

    auto vr_mod = VR::get();
    auto& controllers = vr_mod->get_controllers();

    if (controllers.empty()) {
        m_was_gripping_weapon = false;
        return;
    }

    const auto is_hmd_active = vr_mod->is_hmd_active();
    const auto is_using_controllers = vr_mod->is_using_controllers();

    if (!is_hmd_active || !is_using_controllers) {
        m_was_gripping_weapon = false;
        return;
    }

    auto context = sdk::get_thread_context();

    static auto l_arm_wrist_hash = sdk::murmur_hash::calc32(L"l_arm_wrist");
    static auto r_arm_wrist_hash = sdk::murmur_hash::calc32(L"r_arm_wrist");

    static auto via_motion_def = sdk::find_type_definition("via.motion.Motion");
    const auto via_motion = utility::re_component::find<REComponent>(transform, via_motion_def->type);

    glm::quat original_left_rot_relative{glm::identity<glm::quat>()};
    Vector4f original_left_pos_relative{};

    if (via_motion != nullptr) {
        const auto left_index = sdk::call_object_func_easy<uint32_t>(via_motion, "getJointIndexByNameHash", l_arm_wrist_hash);
        const auto right_index = sdk::call_object_func_easy<uint32_t>(via_motion, "getJointIndexByNameHash", r_arm_wrist_hash);

        const auto original_left_pos = sdk::call_object_func_easy<Vector4f>(via_motion, "getWorldPosition", left_index);
        const auto original_right_pos = sdk::call_object_func_easy<Vector4f>(via_motion, "getWorldPosition", right_index);
        const auto original_left_rot = sdk::call_object_func_easy<glm::quat>(via_motion, "getWorldRotation", left_index);
        const auto original_right_rot = sdk::call_object_func_easy<glm::quat>(via_motion, "getWorldRotation", right_index);

        original_left_pos_relative = glm::inverse(original_right_rot) * (original_left_pos - original_right_pos);
        original_left_rot_relative = glm::inverse(original_right_rot) * original_left_rot;
    }

    auto calculate_joint_pos_rot = [&](uint32_t hash, int32_t controller_index) -> std::tuple<glm::quat, Vector4f> {
        auto wrist_joint = sdk::get_transform_joint_by_hash(transform, hash);

        if (wrist_joint == nullptr) {
            return std::make_tuple<glm::quat, Vector4f>(glm::identity<glm::quat>(), Vector4f{});
        }

        auto& wrist = utility::re_transform::get_joint_matrix(*transform, wrist_joint);

        /*auto look_matrix = glm::extractMatrixRotation(Matrix4x4f{ m_last_controller_rotation }) 
                                                    * Matrix4x4f{ m_scale_debug2.x, 0.0f, 0.0f, 0.0f,
                                                                    0.0f, m_scale_debug2.y, 0.0f, 0.0f,
                                                                    0.0f, 0.0f, m_scale_debug2.z, 0.0f,
                                                                    0.0f, 0.0f, 0.0f, m_scale_debug2.w };*/

        // m_last_controller_rotation_vr is for the controller rotation, but
        // the Y component is zero'd out so only the HMD can look up/down
        auto cam_rot_mat = glm::extractMatrixRotation(Matrix4x4f{ m_last_controller_rotation_vr });

        auto& cam_forward3 = *(Vector3f*)&cam_rot_mat[2];

        auto attach_offset = m_attach_offsets[m_player_name];

        if (is_hmd_active) {
            attach_offset.x = 0.0f;
            attach_offset.z = 0.0f;
        }

        auto offset = glm::extractMatrixRotation(m_last_camera_matrix) * (attach_offset * Vector4f{ -0.1f, 0.1f, 0.1f, 0.0f });
        auto final_pos = Vector3f{ m_last_bone_matrix[3] + offset };

        // what the fuck is this
        auto look_matrix = glm::extractMatrixRotation(glm::rowMajor4(glm::lookAtLH(final_pos, Vector3f{ m_last_controller_pos } + (cam_forward3 * 8192.0f), Vector3f{ 0.0f, 1.0f, 0.0f })));
        //auto look_matrix = glm::extractMatrixRotation(m_last_camera_matrix);
        look_matrix = look_matrix * Matrix4x4f{ m_scale_debug2.x, 0.0f, 0.0f, 0.0f,
                                                0.0f, m_scale_debug2.y, 0.0f, 0.0f,
                                                0.0f, 0.0f, m_scale_debug2.z, 0.0f,
                                                0.0f, 0.0f, 0.0f, m_scale_debug2.w };
        

        const auto hmd_pos = vr_mod->get_position(0);

        auto controller_offset = vr_mod->get_position(controllers[controller_index]) - hmd_pos;
        controller_offset.w = 1.0f;
        auto controller_rotation = vr_mod->get_rotation(controllers[controller_index]) 
                                                    * Matrix4x4f{ m_scale_debug.x, 0.0f, 0.0f, 0.0f,
                                                                    0.0f, m_scale_debug.y, 0.0f, 0.0f,
                                                                    0.0f, 0.0f, m_scale_debug.z, 0.0f,
                                                                    0.0f, 0.0f, 0.0f, m_scale_debug.w };
        
        const auto hand_rotation_offset = controller_index == 0 ? m_left_hand_rotation_offset : m_right_hand_rotation_offset;
        const auto hand_position_offset = controller_index == 0 ? m_left_hand_position_offset : m_right_hand_position_offset;

        const auto offset_quat = glm::normalize(glm::quat{ hand_rotation_offset });
        const auto controller_quat = glm::normalize(glm::quat{ controller_rotation });
        const auto look_quat = glm::normalize(glm::quat{ look_matrix });

        // fix up the controller_rotation by rotating it with the camera rotation (look_matrix)
        auto rotation_quat = glm::normalize(look_quat * controller_quat * offset_quat);
        
        // be sure to always multiply the MATRIX BEFORE THE VECTOR!! WHAT HTE FUCK
        auto hand_pos = look_quat * ((controller_offset * m_vr_scale));
        hand_pos += (glm::normalize(look_quat * controller_quat) * hand_position_offset);

        auto new_pos = m_last_camera_matrix_pre_vr[3] + hand_pos;
        new_pos.w = 1.0f;

        return std::make_tuple(rotation_quat, new_pos);
    };

    auto [rh_rotation, rh_pos] = calculate_joint_pos_rot(r_arm_wrist_hash, 1);
    auto [lh_rotation, lh_pos] = calculate_joint_pos_rot(l_arm_wrist_hash, 0);

    auto lh_grip_position = rh_pos + (rh_rotation * original_left_pos_relative);
    const auto lh_delta_to_rh = (lh_pos - rh_pos);
    const auto lh_grip_delta_to_rh = (lh_grip_position - rh_pos);
    const auto lh_grip_delta = (lh_grip_position - lh_pos);
    const auto lh_grip_distance = glm::length(lh_grip_delta);

    const auto vr = VR::get();
    const auto is_holding_left_grip = vr->is_action_active(vr->get_action_grip(), vr->get_left_joystick());

    static auto player_condition_def = sdk::find_type_definition(game_namespace("survivor.SurvivorCondition"));
    const auto player_condition = utility::re_component::find<REComponent>(transform, player_condition_def->get_type());
    const bool is_reloading = player_condition != nullptr ? sdk::call_object_func_easy<bool>(player_condition, "get_IsReload") : false;
    const bool is_aiming = player_condition != nullptr ? sdk::call_object_func_easy<bool>(player_condition, "get_IsHold") : false;
    
    if (is_aiming && !is_reloading && (lh_grip_distance <= 0.1 || (m_was_gripping_weapon && is_holding_left_grip))) {
        // pistol "fix"
        if (glm::length(Vector3f{original_left_pos_relative}) >= 0.1f) {
            const auto original_grip_rot = utility::math::to_quat(glm::normalize(lh_grip_delta_to_rh));
            const auto current_grip_rot = utility::math::to_quat(glm::normalize(lh_delta_to_rh));

            const auto grip_rot_delta = glm::normalize(current_grip_rot * glm::inverse(original_grip_rot));

            // Adjust the right hand rotation
            rh_rotation = glm::normalize(grip_rot_delta * rh_rotation);

            // Adjust the grip position
            lh_grip_position = rh_pos + (rh_rotation * original_left_pos_relative);
            lh_grip_position.w = 1.0f;
        }

        // Set the left hand position and rotation to the grip position
        lh_pos = lh_grip_position;
        lh_rotation = rh_rotation * original_left_rot_relative;

        m_was_gripping_weapon = true;
    } else {
        m_was_gripping_weapon = false;

        if (is_reloading) {
            lh_pos = lh_grip_position;
            lh_rotation = rh_rotation * original_left_rot_relative;
        } else {
            lh_pos = lh_pos;
            lh_rotation = lh_rotation;
        }
    }

    auto update_joint = [&](uint32_t hash, int32_t controller_index, glm::quat& rotation_quat, Vector4f& new_pos) {
        auto wrist_joint = sdk::get_transform_joint_by_hash(transform, hash);

        if (wrist_joint == nullptr) {
            return;
        }

        // Get Arm IK component
        static auto arm_fit_t = sdk::find_type_definition(game_namespace("IkArmFit"));
        auto arm_fit = utility::re_component::find<REComponent>(transform, arm_fit_t->get_type());

        // We will use the game's IK system instead of building our own because it's a pain in the ass
        // The arm fit component by default will only update the left wrist position (I don't know why, maybe the right arm is a blended animation?)
        // So we will not only abuse that, but we will repurpose it to update the right arm as well
        if (arm_fit != nullptr) {
            auto arm_fit_list_field = arm_fit_t->get_field("ArmFitList");
            auto arm_fit_list = arm_fit_list_field->get_data<REArrayBase*>(arm_fit);

            if (arm_fit_list != nullptr && arm_fit_list->numElements > 0) {
                //spdlog::info("Inside arm fit, arm fit list: {:x}", (uintptr_t)arm_fit_list);

                auto arm_fit_data = utility::re_array::get_element<REManagedObject>(arm_fit_list, 0);

                if (arm_fit_data != nullptr) {
                    //spdlog::info("First element: {:x}", (uintptr_t)first_element);

                    auto arm_fit_data_t = utility::re_managed_object::get_type_definition(arm_fit_data);
                    auto arm_fit_data_tmatrix_field = arm_fit_data_t->get_field("<TargetMatrix>k__BackingField");
                    auto& target_matrix = arm_fit_data_tmatrix_field->get_data<Matrix4x4f>(arm_fit_data);

                    auto solver_list_field = arm_fit_t->get_field("<SolverList>k__BackingField");
                    auto solver_list = solver_list_field->get_data<::DotNetGenericList*>(arm_fit);

                    // Keep track of the old joints so we can set them back after updating the IK
                    REJoint* old_apply_joint = nullptr;
                    REJoint** apply_joint_ptr = nullptr;

                    // Using the solver list, we are going to override the target joint to the current wrist joint
                    // So we can use the IK system to update both wrists
                    if (solver_list != nullptr) {
                        //spdlog::info("Solver list: {:x}", (uintptr_t)solver_list);

                        auto raw_solver_list = solver_list->data;

                        if (raw_solver_list != nullptr && raw_solver_list->numElements > 0) {
                            auto first_solver = utility::re_array::get_element<REManagedObject>(raw_solver_list, 0);

                            if (first_solver != nullptr) {
                                //spdlog::info("First solver: {:x}", (uintptr_t)first_solver);

                                auto first_solver_t = utility::re_managed_object::get_type_definition(first_solver);
                                auto apply_joint_field = first_solver_t->get_field("<ApplyJoint>k__BackingField");
                                auto& apply_joint = apply_joint_field->get_data<REJoint*>(first_solver);

                                old_apply_joint = apply_joint;
                                apply_joint_ptr = &apply_joint;
                                
                                // Set the apply joint to the wrist joint
                                apply_joint = wrist_joint;

                                auto l0_field = first_solver_t->get_field("<L0>k__BackingField");
                                auto l1_field = first_solver_t->get_field("<L1>k__BackingField");

                                auto& l0 = l0_field->get_data<float>(first_solver);
                                auto& l1 = l1_field->get_data<float>(first_solver);

                                const auto total_length = l0 + l1;

                                // Get shoulder joint by getting the parents of the wrist joint
                                auto elbow_joint = sdk::get_joint_parent(wrist_joint);
                                auto shoulder_joint = elbow_joint != nullptr ? sdk::get_joint_parent(elbow_joint) : (REJoint*)nullptr;
                                auto shoulder_parent_joint = shoulder_joint != nullptr ? sdk::get_joint_parent(shoulder_joint) : (REJoint*)nullptr;

                                if (elbow_joint != nullptr && shoulder_joint != nullptr && shoulder_parent_joint != nullptr) {
                                    const auto transform_pos = sdk::get_transform_position(transform);
                                    const auto transform_rotation = sdk::get_transform_rotation(transform);

                                    // grab the T-pose of the elbow and shoulder
                                    // then set the current position of the elbow and shoulder to the T-pose
                                    const auto original_elbow_transform = utility::re_transform::calculate_base_transform(*transform, elbow_joint);
                                    const auto original_shoulder_transform = utility::re_transform::calculate_base_transform(*transform, shoulder_joint);
                                    const auto original_shoulder_parent_transform = utility::re_transform::calculate_base_transform(*transform, shoulder_parent_joint);

                                    const auto original_elbow_pos = transform_pos + (transform_rotation * original_elbow_transform[3]);
                                    const auto original_elbow_rot = transform_rotation * glm::quat{glm::extractMatrixRotation(original_elbow_transform)};
                                    const auto original_shoulder_pos = transform_pos + (transform_rotation * original_shoulder_transform[3]);
                                    const auto original_shoulder_rot = transform_rotation * glm::quat{glm::extractMatrixRotation(original_shoulder_transform)};
                                    const auto original_shoulder_parent_pos = transform_pos + (transform_rotation * original_shoulder_parent_transform[3]);
                                    const auto original_shoulder_parent_rot = transform_rotation * glm::quat{glm::extractMatrixRotation(original_shoulder_parent_transform)};

                                    const auto current_shoulder_parent_pos = sdk::get_joint_position(shoulder_parent_joint);
                                    const auto current_shoulder_parent_rot = sdk::get_joint_rotation(shoulder_parent_joint);

                                    const auto shoulder_diff = original_shoulder_pos - original_shoulder_parent_pos;
                                    const auto elbow_diff = original_elbow_pos - original_shoulder_pos;

                                    const auto updated_shoulder_pos = current_shoulder_parent_pos + shoulder_diff;
                                    const auto updated_elbow_pos = updated_shoulder_pos + elbow_diff;

                                    sdk::set_joint_position(shoulder_joint, updated_shoulder_pos);
                                    sdk::set_joint_rotation(shoulder_joint, original_shoulder_rot);
                                    sdk::set_joint_position(elbow_joint, updated_elbow_pos);
                                    sdk::set_joint_rotation(elbow_joint, original_elbow_rot);

                                    const auto shoulder_joint_pos = sdk::get_joint_position(shoulder_joint);

                                    // Bring the new_pos back to the shoulder joint + dir to the wrist joint * total length/
                                    // This will keep the arm properly extended instead of contracting back to the original animation
                                    // When the wanted position exceeds the total IK length.
                                    // Usually that's what IK is supposed to do, but not in this game, i guess
                                    if (glm::length(new_pos - shoulder_joint_pos) > total_length) {
                                        new_pos = shoulder_joint_pos + (glm::normalize(new_pos - shoulder_joint_pos) * total_length);
                                    }
                                }
                            }
                        }
                    }
                
                    // Set the target matrix to the VR controller's position (new_pos, rotation_quat)
                    target_matrix = Matrix4x4f{ rotation_quat };
                    target_matrix[3] = new_pos;
                    target_matrix[3].w = 1.0f;

                    //spdlog::info("About to call updateIk");

                    auto blend_rate_field = arm_fit_t->get_field("BlendRateField");
                    auto& blend_rate = blend_rate_field->get_data<float>(arm_fit);

                    auto armfit_data_blend_rate_field = arm_fit_data_t->get_field("BlendRate");
                    auto& armfit_data_blend_rate = armfit_data_blend_rate_field->get_data<float>(arm_fit_data);

                    blend_rate = 1.0f;
                    armfit_data_blend_rate = 1.0f;

                    // Call the IK update function (index 0, first element)
                    sdk::call_object_func<void*>(arm_fit, "updateIk", context, arm_fit, 0);

                    // Reset the apply joint to the old value
                    if (apply_joint_ptr != nullptr) {
                        *apply_joint_ptr = old_apply_joint;
                    }
                }
            } else {
                spdlog::info("Arm fit list is empty");
            }
        } else {
            spdlog::info("Arm fit component not found");
        }

        // radians -> deg
        m_last_controller_euler[controller_index] = glm::eulerAngles(rotation_quat) * (180.0f / glm::pi<float>());
    };

    if (is_using_controllers)  {
        update_joint(l_arm_wrist_hash, 0, lh_rotation, lh_pos);
        update_joint(r_arm_wrist_hash, 1, rh_rotation, rh_pos);
    }
}

void FirstPerson::update_player_muzzle_behavior(RETransform* transform, bool restore) {
    if (!restore && !m_enabled->value()) {
        return;
    }

    if (m_camera_system == nullptr) {
        return;
    }

    if (m_last_camera_type != app::ropeway::camera::CameraControlType::PLAYER) {
        return;
    }

    auto vr_mod = VR::get();
    auto& controllers = vr_mod->get_controllers();
    const auto is_hmd_active = vr_mod->is_hmd_active();
    const auto is_using_controllers = vr_mod->is_using_controllers();

    auto context = sdk::get_thread_context();

    // user just took off their headset
    if (!is_hmd_active && m_was_hmd_active) {
        restore = true;
    }

    if (!restore) {
        if (!is_hmd_active) {
            return;
        }
    }

    // We're going to modify the player's weapon (gun) to fire from the muzzle instead of the camera
    // Luckily the game has that built-in so we don't really need to hook anything
    static auto equipment_t = sdk::find_type_definition(game_namespace("survivor.Equipment"));
    auto equipment = utility::re_component::find<REComponent>(transform, equipment_t->get_type());

    if (equipment != nullptr) {
        auto main_weapon_field = equipment_t->get_field("<EquipWeapon>k__BackingField");
        auto& main_weapon = main_weapon_field->get_data<REManagedObject*>(equipment);

        if (main_weapon != nullptr) {
            auto main_weapon_game_object = sdk::call_object_func<REGameObject*>(main_weapon, "get_GameObject", context, main_weapon);
            auto main_weapon_transform = main_weapon_game_object != nullptr ? main_weapon_game_object->transform : (RETransform*)nullptr;

            static auto implement_gun_typedef = sdk::find_type_definition(game_namespace("implement.Gun"));
            static auto implement_melee_typedef = sdk::find_type_definition(game_namespace("implement.Melee"));

            auto main_weapon_t = utility::re_managed_object::get_type_definition(main_weapon);

            if (main_weapon_game_object != nullptr && main_weapon_t != nullptr && main_weapon_t->is_a(implement_gun_typedef)) {
                auto& fire_bullet_param = *sdk::get_object_field<REManagedObject*>(main_weapon, "<FireBulletParam>k__BackingField");

                if (fire_bullet_param != nullptr) {
                    auto fire_bullet_param_t = utility::re_managed_object::get_type_definition(fire_bullet_param);
                    auto& fire_bullet_type = *sdk::get_object_field<app::ropeway::weapon::shell::ShellDefine::FireBulletType>(fire_bullet_param, "_FireBulletType");

                    // Set the fire bullet type to AlongMuzzle, which fires from the muzzle's position and rotation
                    if (is_using_controllers && !restore) {
                        static auto throw_grenade_generator_type = sdk::find_type_definition(game_namespace("weapon.generator.ThrowGrenadeGenerator"));
                        auto shell_generator = *sdk::get_object_field<REManagedObject*>(main_weapon, "<ShellGenerator>k__BackingField");
                        auto is_grenade = false;

                        if (shell_generator != nullptr) {
                            auto shell_generator_t = utility::re_managed_object::get_type_definition(shell_generator);

                            if (shell_generator_t != nullptr && shell_generator_t->is_a(throw_grenade_generator_type)) {
                                is_grenade = true;
                            }
                        }

                        if (!is_grenade) {
                            fire_bullet_type = app::ropeway::weapon::shell::ShellDefine::FireBulletType::AlongMuzzle;
                        }
                    } else {
                        if (fire_bullet_type == app::ropeway::weapon::shell::ShellDefine::FireBulletType::AlongMuzzle) {
                            fire_bullet_type = app::ropeway::weapon::shell::ShellDefine::FireBulletType::Camera;
                        }
                    }

                    auto muzzle_joint_param = *sdk::get_object_field<REManagedObject*>(fire_bullet_param, "_MuzzleJointParameter");
                    auto muzzle_joint_extra = *sdk::get_object_field<REManagedObject*>(main_weapon, "<MuzzleJoint>k__BackingField");

                    // Set the muzzle joint to the VFX muzzle position used for stuff like muzzle flashes
                    if (muzzle_joint_param != nullptr && muzzle_joint_extra != nullptr) {
                        static auto vfx_muzzle1_hash = sdk::murmur_hash::calc32(L"vfx_muzzle1");

                        auto vfx_muzzle1 = sdk::get_transform_joint_by_hash(main_weapon_transform, vfx_muzzle1_hash);
                        auto current_muzzle_joint = *sdk::get_object_field<REJoint*>(muzzle_joint_extra, "_Parent");

                        // Set the parent joint name to the VFX muzzle joint which will set _Parent later on
                        if (vfx_muzzle1 != nullptr && current_muzzle_joint != vfx_muzzle1) {
                            auto muzzle_joint_name = sdk::VM::create_managed_string(L"vfx_muzzle1");

                            // call set_ParentJointNameForm
                            sdk::call_object_func<void*>(muzzle_joint_param, "set_ParentJointNameForm", context, muzzle_joint_param, muzzle_joint_name);
                        }
                    }
                }
            } else if (main_weapon_game_object != nullptr && main_weapon_t != nullptr && main_weapon_t->is_a(implement_melee_typedef)) {
                auto collider_field = main_weapon_t->get_field("<RequestSetCollider>k__BackingField");
                auto& collider = collider_field->get_data<REManagedObject*>(main_weapon);

                if (collider != nullptr) {
                    // TODO! it's a lua script, maybe implement natively later?
                }
            }
        }
    }
}

void FirstPerson::update_player_body_ik(RETransform* transform, bool restore, bool first) {
    if (!restore && !m_enabled->value()) {
        m_was_gripping_weapon = false;
        return;
    }

    if (m_camera_system == nullptr) {
        m_was_gripping_weapon = false;
        return;
    }

    if (m_last_camera_type != app::ropeway::camera::CameraControlType::PLAYER) {
        m_was_gripping_weapon = false;
        return;
    }

    auto vr_mod = VR::get();
    auto& controllers = vr_mod->get_controllers();
    const auto is_hmd_active = vr_mod->is_hmd_active();
    const auto is_using_controllers = vr_mod->is_using_controllers();

    auto context = sdk::get_thread_context();

    // user just took headset off
    if (!is_hmd_active && m_was_hmd_active) {
        restore = true;
    }

    if (!restore) {
        if (!is_hmd_active) {
            return;
        }
    }

    static auto ik_leg_def = sdk::find_type_definition("via.motion.IkLeg");
    static auto via_motion_def = sdk::find_type_definition("via.motion.Motion");
    auto ik_leg = utility::re_component::find<REComponent>(transform, ik_leg_def->type);
    auto via_motion = utility::re_component::find<REComponent>(transform, via_motion_def->type);

    // We're going to use the leg IK to adjust the height of the player according to headset position
    if (ik_leg != nullptr && via_motion != nullptr) {
        // disabling FirstPerson triggers this
        if (restore) {
            Vector4f zero_offset{};

            sdk::call_object_func<void*>(ik_leg, "set_CenterPositionCtrl", sdk::get_thread_context(), ik_leg, via::motion::IkLeg::EffectorCtrl::None);
            sdk::call_object_func<void*>(ik_leg, "set_CenterOffset", sdk::get_thread_context(), ik_leg, &zero_offset);
            utility::re_managed_object::call_method(ik_leg, "set_CenterAdjust", via::motion::IkLeg::CenterAdjust::Center);

            return;
        }

        const auto headset_pos = vr_mod->get_position(0);
        auto standing_origin = vr_mod->get_standing_origin();
        auto hmd_offset = headset_pos - standing_origin;

        glm::quat rotation{};

        if (is_using_controllers) {
            rotation = m_last_controller_rotation_vr;
        } else {
            rotation = utility::math::remove_y_component(Matrix4x4f{glm::normalize(m_last_controller_rotation_vr)});
        }

        // Create a final offset which will keep the player's head where they want
        // while also stabilizing any undesired head movement from the original animation
        Vector4f final_offset{ rotation * hmd_offset };

        const auto smooth_xz_movement = m_smooth_xz_movement->value();
        const auto smooth_y_movement = m_smooth_y_movement->value();

        static auto cog_hash = sdk::murmur_hash::calc32(L"COG");
        static auto head_hash = sdk::murmur_hash::calc32(L"head");
        static auto root_hash = sdk::murmur_hash::calc32(L"root");

        auto center_joint = sdk::get_transform_joint_by_hash(transform, cog_hash);

        if (smooth_xz_movement || smooth_y_movement) {
            auto head_joint = sdk::get_transform_joint_by_hash(transform, head_hash);
            auto root_joint = sdk::get_transform_joint_by_hash(transform, root_hash);

            if (head_joint != nullptr && center_joint != nullptr && root_joint != nullptr) {
                const auto head_joint_index = ((sdk::Joint*)head_joint)->get_joint_index();
                const auto cog_joint_index = ((sdk::Joint*)center_joint)->get_joint_index();
                const auto root_joint_index = ((sdk::Joint*)root_joint)->get_joint_index();

                const auto base_transform_pos = sdk::get_transform_position(transform);
                const auto base_transform_rot = sdk::get_transform_rotation(transform);

                Vector4f original_head_pos{};
                sdk::call_object_func<Vector4f*>(via_motion, "getWorldPosition", &original_head_pos, sdk::get_thread_context(), via_motion, head_joint_index);

                original_head_pos = base_transform_rot * original_head_pos;

                // the reference pose for the head joint
                const auto head_base_transform = utility::re_transform::calculate_base_transform(*transform, head_joint);
                const auto reference_height = head_base_transform[3].y;

                if (smooth_xz_movement) {
                    final_offset.x -= original_head_pos.x;
                    final_offset.z -= original_head_pos.z;
                }

                if (smooth_y_movement) {
                    final_offset.y += (reference_height - original_head_pos.y);
                }
            }
        }
        
        sdk::call_object_func<void*>(ik_leg, "set_CenterPositionCtrl", sdk::get_thread_context(), ik_leg, via::motion::IkLeg::EffectorCtrl::WorldOffset);
        sdk::call_object_func<void*>(ik_leg, "set_CenterOffset", sdk::get_thread_context(), ik_leg, &final_offset);

        // this will allow the player to physically move higher than the model's standing height
        // so the head adjustment will be more accurate and smooth if the player is standing straight.
        // a small side effect is that the player can slightly float, but it's worth it.
        // not a TDB method unfortunately.
        utility::re_managed_object::call_method(ik_leg, "set_CenterAdjust", via::motion::IkLeg::CenterAdjust::None);
        update_player_arm_ik(transform);
    }
}

void FirstPerson::update_player_body_rotation(RETransform* transform) {
    if (!m_enabled->value() || m_camera_system == nullptr) {
        return;
    }

    if (m_last_camera_type != app::ropeway::camera::CameraControlType::PLAYER) {
        return;
    }

    if (!m_rotate_body->value()) {
        return;
    }

    if (is_jacked(transform)) {
        return;
    }

    auto player_matrix = glm::mat4{ *(glm::quat*)&transform->angles };

    auto player_right = *(Vector3f*)&player_matrix[0] * -1.0f;
    player_right[1] = 0.0f;
    player_right = glm::normalize(player_right);
    auto player_forward = *(Vector3f*)&player_matrix[2] * -1.0f;
    player_forward[1] = 0.0f;
    player_forward = glm::normalize(player_forward);

    auto camera_matrix = m_last_camera_matrix;

    auto cam_forward3 = *(Vector3f*)&camera_matrix[2];

    // Remove the upwards facing component of the forward vector
    cam_forward3[1] = 0.0f;
    cam_forward3 = glm::normalize(cam_forward3);

    auto angle_between = glm::degrees(glm::orientedAngle(player_forward, cam_forward3, Vector3f{ 0.0f, 1.0f, 0.0f }));

    if (std::abs(angle_between) == 0.0f) {
        return;
    }

    if (angle_between > 0) {
        player_right *= -1.0f;
    }

    auto angle_diff = std::abs(angle_between) / 720.0f;
    auto diff = (player_forward - player_right) * angle_diff * update_delta_time(transform) * m_body_rotate_speed->value();

    // Create a two-dimensional representation of the forward vector as a rotation matrix
    auto rot = glm::lookAtRH(Vector3f{}, glm::normalize(player_forward + diff), Vector3f{ 0.0f, 1.0f, 0.0f });
    camera_matrix = glm::extractMatrixRotation(glm::rowMajor4(rot));

    auto camera_quat = glm::quat{ camera_matrix };

    // Finally rotate the player transform to match the camera in a two-dimensional fashion
    transform->angles = *(Vector4f*)&camera_quat;
}

void FirstPerson::update_player_roomscale(RETransform* transform) {
    const auto now = std::chrono::steady_clock::now();

    if (transform == nullptr) {
        m_last_roomscale_failure = now;
        return;
    }

    if (!m_enabled->value() || !will_be_used() || m_last_camera_type != app::ropeway::camera::CameraControlType::PLAYER) {
        m_last_roomscale_failure = now;
        return;
    }

    auto vr = VR::get();

    if (!vr->is_hmd_active() || !m_roomscale->value()) {
        m_last_roomscale_failure = now;
        return;
    }

    if (!update_pointers()) {
        m_last_roomscale_failure = now;
        return;
    }

    if (is_jacked(transform)) {
        m_last_roomscale_failure = now;
        return;
    }

    // try to fix some weirdness after exiting cutscenes and stuff
    if ((now - m_last_roomscale_failure) < std::chrono::milliseconds(1000)) {
        return;
    }
    
    // Roomscale movement.
    const auto old_standing_origin = vr->get_standing_origin();
    const auto old_hmd_pos = vr->get_position(0);
    const auto hmd_pos = Vector4f{old_hmd_pos.x, old_standing_origin.y, old_hmd_pos.z, old_hmd_pos.w};

    if (glm::length(hmd_pos - old_standing_origin) > 0.01f) {
        const auto t = sdk::Application::get()->get_delta_time() * 0.1f;
        const auto standing_origin = glm::lerp(old_standing_origin, hmd_pos, glm::length(hmd_pos - old_standing_origin) * t);
        vr->set_standing_origin(standing_origin);

        const auto standing_diff = standing_origin - old_standing_origin;

        const auto player_pos = sdk::get_transform_position(transform);
        const auto& last_render_matrix = vr->get_last_render_matrix();
        const auto lerp_to = Vector4f{last_render_matrix[3].x, player_pos.y, last_render_matrix[3].z, player_pos.w};
        const auto new_pos = player_pos + (glm::normalize(lerp_to - player_pos) * glm::length(standing_diff));

        // BAD idea to call this without no_dirty while scene is locked. causes parent objects to get stuck.
        sdk::set_transform_position(transform, new_pos, true);
    }
}

void FirstPerson::update_camera_transform(RETransform* transform) {
    if (!m_enabled->value()) {
        return;
    }

    std::lock_guard _{ m_matrix_mutex };
    
    auto vr = VR::get();

    /*if (vr->is_hmd_active() && m_camera_system->cameraController != nullptr) {
        m_last_controller_rotation = *(glm::quat*)&m_camera_system->cameraController->worldRotation;
    }*/

    const auto gui_state = *sdk::get_object_field<int32_t>(m_gui_master, "<State_>k__BackingField");
    const auto is_paused = gui_state == (int32_t)app::ropeway::gui::GUIMaster::GuiState::PAUSE || gui_state == (int32_t)app::ropeway::gui::GUIMaster::GuiState::INVENTORY;

    m_last_camera_type = utility::re_managed_object::get_field<app::ropeway::camera::CameraControlType>(m_camera_system, "BusyCameraType");
    m_last_pause_state = is_paused;

    const auto is_player_camera = m_last_camera_type == app::ropeway::camera::CameraControlType::PLAYER && !is_paused;
    const auto is_switching_camera = utility::re_managed_object::get_field<bool>(m_camera_system->mainCameraController, "SwitchingCamera");
    const auto is_player_in_control = (is_player_camera && !is_switching_camera && !m_last_pause_state);
    const auto is_switching_to_player_camera = is_player_camera && is_switching_camera;

    auto& mtx = transform->worldTransform;

    // Don't mess with the camera if we're in a cutscene
    if (!is_first_person_allowed()) {
        if (m_camera_system->mainCameraController != nullptr && !is_paused && !is_switching_to_player_camera) {
            m_camera_system->mainCameraController->updateCamera = true;
        }

        if (is_paused) {
            return;
        }

        if (vr->is_hmd_active()) {
            //m_last_camera_matrix = vr->get_last_render_matrix();
            //m_last_camera_matrix_pre_vr = glm::inverse(vr->get_rotation(0)) * glm::extractMatrixRotation(m_last_camera_matrix);
            //m_last_camera_matrix_pre_vr[3] = m_last_camera_matrix[3];
        }
    }

    auto delta_time = update_delta_time(transform);

    auto& camera_pos = mtx[3];

    auto cam_pos3 = Vector3f{ m_last_controller_pos };

    auto camera_matrix = m_last_camera_matrix_pre_vr * Matrix4x4f{
        -1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, -1, 0,
        0, 0, 0, 1
    };

    //is_player_camera = is_player_camera && !is_switching_camera;

    // fix some crazy spinning bullshit
    if (gui_state == (int32_t)app::ropeway::gui::GUIMaster::GuiState::PAUSE) {
        //*(Matrix3x4f*)&mtx = m_last_camera_matrix_pre_vr;
    }

    const auto wanted_camera_shake = vr->is_hmd_active() ? 0.0f : m_bone_scale->value();
    const auto wanted_camera_speed = vr->is_hmd_active() ? 100.0f : m_camera_scale->value();

    m_interp_bone_scale = glm::lerp(m_interp_bone_scale, wanted_camera_shake, std::clamp(delta_time * 0.05f, 0.0f, 1.0f));
    m_interp_camera_speed = glm::lerp(m_interp_camera_speed, wanted_camera_speed, std::clamp(delta_time * 0.05f, 0.0f, 1.0f));

    if (is_switching_camera || !is_player_camera) {
        if (is_switching_camera) {
            m_interp_camera_speed = 100.0f;

            auto c = m_camera_system->mainCameraController;

            if (is_player_camera) {
                auto len = c->switchInterpolationTime;
                
                if (len == 0.0f) {
                    len = 1.0f;
                }

                m_interp_bone_scale = (5.0f / len);
                //m_interp_bone_scale = 10.0f;
            }
            else {
                m_interp_bone_scale = 100.0f;
            }
        }
        else {
            m_interp_camera_speed = 100.0f;
            m_interp_bone_scale = 50.0f;
        }
    }

    // Lets camera modification work in cutscenes/action camera etc
    if (!is_player_camera && !is_switching_camera && is_first_person_allowed()) {
        m_camera_system->mainCameraController->updateCamera = false;
    }
    else {
        m_camera_system->mainCameraController->updateCamera = true;
    }

    if (is_first_person_allowed()) {
        camera_matrix[3] = m_last_bone_matrix[3];
    }

    auto& bone_pos = camera_matrix[3];
    const auto real_headset_rotation = vr->get_rotation(0);

    // Fix rotation after returning to player control
    if (is_player_in_control && m_has_cutscene_rotation) {
        if (vr->is_hmd_active()) {
            m_last_headset_rotation_pre_cutscene = utility::math::remove_y_component(real_headset_rotation);
            m_last_controller_rotation = glm::quat{vr->get_last_render_matrix()};
        } else {
            m_last_headset_rotation_pre_cutscene = glm::identity<Matrix4x4f>();
        }

        const auto cutscene_inverse = glm::inverse(m_last_headset_rotation_pre_cutscene);
        const auto cutscene_quat = glm::quat{ cutscene_inverse };
        m_last_controller_rotation = glm::normalize((cutscene_quat) * m_last_controller_rotation);
        m_last_controller_angles = utility::math::euler_angles(Matrix4x4f{m_last_controller_rotation});

        if (m_player_camera_controller != nullptr) {
            m_player_camera_controller->worldRotation = m_camera_system->cameraController->worldRotation;

            m_player_camera_controller->pitch = m_last_controller_angles.x;
            m_player_camera_controller->yaw = m_last_controller_angles.y;
        }

        m_camera_system->mainCameraController->cameraRotation = *(Vector4f*)&m_last_controller_rotation;
        m_camera_system->cameraController->worldRotation = *(Vector4f*)&m_last_controller_rotation;

        m_has_cutscene_rotation = false;
        m_ignore_next_player_angles = true;

        m_camera_system->mainCameraController->updateCamera = false;

        //*(Matrix3x4f*)&mtx = cutscene_inverse * m_last_camera_matrix_pre_vr;

        // Do not interpolate these so the camera doesn't jump after exiting a cutscene.
        m_interp_bone_scale = 0.0f;
        m_interp_camera_speed = 100.0f;
    }

    auto bone_scale = (is_player_in_control || is_switching_to_player_camera) ? (m_interp_bone_scale * 0.01f) : 1.0f;

    auto cam_rot_mat = glm::extractMatrixRotation(Matrix4x4f{ m_last_controller_rotation });
    auto head_rot_mat = glm::extractMatrixRotation(m_last_bone_matrix) * Matrix4x4f {
        -1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, -1, 0,
        0, 0, 0, 1
    };

    auto& cam_forward3 = *(Vector3f*)&cam_rot_mat[2];

    // Zero out the Y component of the forward vector
    // When using VR so only the user can control the up/down rotation with the headset
    if (vr->is_hmd_active() && vr->is_using_controllers() && !is_paused) {
        cam_forward3[1] = 0.0f;
        cam_forward3 = glm::normalize(cam_forward3);
    }

    auto attach_offset = m_attach_offsets[m_player_name];

    if (vr->is_hmd_active()) {
        attach_offset.x = 0.0f;
        attach_offset.z = 0.0f;
    }

    auto offset = glm::extractMatrixRotation(camera_matrix) * (attach_offset * Vector4f{ -0.1f, 0.1f, 0.1f, 0.0f });
    auto final_pos = Vector3f{ bone_pos + offset };

    // Average the distance to the wanted rotation
    auto dist = (glm::distance(m_interpolated_bone[0], head_rot_mat[0])
               + glm::distance(m_interpolated_bone[1], head_rot_mat[1])
               + glm::distance(m_interpolated_bone[2], head_rot_mat[2])) / 3.0f;

    // interpolate the bone rotation (it's snappy otherwise)
    if (is_player_in_control || is_switching_to_player_camera) {
        if (vr->is_hmd_active()) {
            m_interpolated_bone = glm::identity<Matrix4x4f>();
        } else {
            m_interpolated_bone = glm::interpolate(m_interpolated_bone, head_rot_mat, delta_time * bone_scale * dist);
        }
    }
    else {
        m_interpolated_bone = head_rot_mat;
    }

    // Look at where the camera is pointing from the head position
    cam_rot_mat = glm::extractMatrixRotation(glm::rowMajor4(glm::lookAtLH(final_pos, cam_pos3 + (cam_forward3 * 8192.0f), Vector3f{ 0.0f, 1.0f, 0.0f })));
    // Follow the bone rotation, but rotate towards where the camera is looking.
    auto wanted_mat = glm::inverse(m_interpolated_bone) * cam_rot_mat;

    if (is_player_in_control || is_switching_to_player_camera) {
        if (is_player_in_control && m_interp_camera_speed >= 100.0f && bone_scale == 0.0f) {
            m_rotation_offset = wanted_mat;
        } else {
            // Average the distance to the wanted rotation
            dist = (glm::distance(m_rotation_offset[0], wanted_mat[0])
                + glm::distance(m_rotation_offset[1], wanted_mat[1])
                + glm::distance(m_rotation_offset[2], wanted_mat[2])) / 3.0f;

            m_rotation_offset = glm::interpolate(m_rotation_offset, wanted_mat, delta_time * (m_interp_camera_speed * 0.01f) * dist);
        }
    }
    else {
        m_rotation_offset = wanted_mat;
    }

    auto final_mat = is_player_camera ? (m_interpolated_bone * m_rotation_offset) : m_interpolated_bone;

    auto mtx_pre_vr = final_mat;
    mtx_pre_vr[3] = mtx[3];

    const auto final_mat_pre_vr = final_mat;
    const auto final_quat_pre_vr = glm::normalize(glm::quat{final_mat});

    //if (!is_paused && m_has_cutscene_rotation) {
        //final_mat *= glm::inverse(m_last_headset_rotation_pre_cutscene);
    //}

    if (vr->is_hmd_active()) {
        const auto last_headset_rotation = !is_player_in_control ? m_last_headset_rotation_pre_cutscene : glm::identity<Matrix4x4f>();
        const auto inverse_last_headset_rotation = !is_player_in_control ? glm::inverse(m_last_headset_rotation_pre_cutscene) : glm::identity<Matrix4x4f>();
        const auto headset_rotation = inverse_last_headset_rotation * real_headset_rotation;

        if (is_first_person_allowed()) {
            final_mat *= headset_rotation;
            vr->recenter_view(); // only affects third person/cutscenes
        } else {
            //final_mat *= Matrix4x4f{glm::normalize(vr->get_rotation_offset() * glm::quat{real_headset_rotation})};
            final_mat = vr->get_last_render_matrix();
        }
    }

    auto final_quat = glm::normalize(glm::quat{ final_mat });

    // Apply the same matrix data to other things stored in-game (positions/quaternions)
    if (is_first_person_allowed()) {
        camera_pos = Vector4f{ final_pos, 1.0f };
        m_camera_system->cameraController->worldPosition = *(Vector4f*)&camera_pos;
        m_camera_system->cameraController->worldRotation = *(Vector4f*)&final_quat;

        transform->position = *(Vector4f*)&camera_pos;
        transform->angles = *(Vector4f*)&final_quat;

        // Apply the new matrix
        *(Matrix3x4f*)&mtx = final_mat;
    }

    if (is_player_in_control || is_first_person_allowed()) {
        m_last_camera_matrix = final_mat;
        m_last_camera_matrix[3] = mtx[3];
        m_last_camera_matrix_pre_vr = mtx_pre_vr; 
    }

    // Fixes snappiness after camera switching
    if (!is_player_in_control) {
        m_last_controller_pos = m_camera_system->cameraController->worldPosition;

        if (!is_switching_to_player_camera) {
            m_last_controller_rotation = final_quat;
        }

        m_camera_system->mainCameraController->cameraPosition = m_last_controller_pos;
        m_camera_system->mainCameraController->cameraRotation = *(Vector4f*)&final_quat_pre_vr;
        m_camera_system->cameraController->worldRotation = *(Vector4f*)&final_quat_pre_vr;

        //if (!is_switching_to_player_camera) {
            //m_last_controller_angles = utility::math::euler_angles(final_mat_pre_vr);
        //}

        if (!is_switching_to_player_camera) {
            m_last_controller_angles = utility::math::euler_angles(final_mat_pre_vr);
        }

        // These are what control the real rotation, so only set it in a cutscene or something
        // If we did it all the time, the view would drift constantly
        //m_camera_system->cameraController->pitch = m_last_controller_angles.x;
        //m_camera_system->cameraController->yaw = m_last_controller_angles.y;

        if (m_player_camera_controller != nullptr) {
            m_player_camera_controller->worldPosition = m_camera_system->cameraController->worldPosition;
            m_player_camera_controller->worldRotation = m_camera_system->cameraController->worldRotation;

            /*if (m_last_camera_type == app::ropeway::camera::CameraControlType::PLAYER) {
                m_last_controller_angles.z = 0.0f;

                m_last_controller_angles += (prev_angles - m_last_controller_angles) * delta_time;
            }*/

            // Forces the game to keep the previous angles/rotation we set after exiting a cutscene
            //if (is_switching_to_player_camera) {
                m_player_camera_controller->pitch = m_last_controller_angles.x;
                m_player_camera_controller->yaw = m_last_controller_angles.y;
            //}

            m_ignore_next_player_angles = m_ignore_next_player_angles || is_switching_to_player_camera;
        }
    }

    if (transform->joints.matrices != nullptr && is_first_person_allowed()) {
        auto joint = utility::re_transform::get_joint(*transform, 0);

        if (joint != nullptr) {
            sdk::set_joint_rotation(joint, m_last_camera_matrix);
            sdk::set_joint_position(joint, m_last_camera_matrix[3]);
        }
    }

    m_last_controller_rotation_vr = glm::quat { cam_rot_mat };

    // Keep track of the last pre-cutscene headset rotation
    // so the player doesn't do something like flip 180 when a cutscene starts
    // which would be especially prevalent for a user with a large playspace
    if (!is_player_in_control && !is_switching_to_player_camera && !m_has_cutscene_rotation) {
        if (vr->is_hmd_active()) {
            m_last_headset_rotation_pre_cutscene = utility::math::remove_y_component(real_headset_rotation);
        } else {
            m_last_headset_rotation_pre_cutscene = glm::identity<Matrix4x4f>();
        }

        m_has_cutscene_rotation = true;
    } else if (is_player_in_control) {
        m_has_cutscene_rotation = false;
    }
}

void FirstPerson::update_sweet_light_context(RopewaySweetLightManagerContext* ctx) {
    if (ctx->controller == nullptr || ctx->controller->ownerGameObject == nullptr) {
        return;
    }


    // Disable the camera light source.
    ctx->controller->ownerGameObject->shouldDraw = !(m_enabled->value() &&
                                                     m_disable_light_source->value() &&
                                                     m_camera_system->cameraController == m_player_camera_controller);
}

void FirstPerson::update_player_bones(RETransform* transform) {
    if (g_first_time) {
        auto& bone_matrix = utility::re_transform::get_joint_matrix(*m_player_transform, m_attach_bone);

        m_cached_bone_matrix = &bone_matrix;
        m_last_bone_matrix = bone_matrix;
        g_first_time = false;
    }

    if (m_cached_bone_matrix == nullptr) {
        return;
    }

    auto& bone_matrix = *m_cached_bone_matrix;

    // Forcefully rotate the bone to match the camera direction
    if (m_camera_system->cameraController == m_player_camera_controller && m_rotate_mesh->value()) {
        auto wanted_mat = m_last_camera_matrix * Matrix4x4f{
            -1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, -1, 0,
            0, 0, 0, 1
        };

        *(Matrix3x4f*)&bone_matrix = wanted_mat;
    }

    // Hide the head model by moving it out of view of the camera (and hopefully shadows...)
    if (m_hide_mesh->value()) {
        bone_matrix[0] = Vector4f{ 0.0f, 0.0f, 0.0f, 0.0f };
        bone_matrix[1] = Vector4f{ 0.0f, 0.0f, 0.0f, 0.0f };
        bone_matrix[2] = Vector4f{ 0.0f, 0.0f, 0.0f, 0.0f };
    }
}

void FirstPerson::update_fov(RopewayPlayerCameraController* controller) {
    if (controller == nullptr) {
        return;
    }

    auto is_active_camera = m_camera_system != nullptr
        && m_camera_system->cameraController != nullptr
        && m_camera_system->cameraController->cameraParam != nullptr
        && m_camera_system->cameraController->activeCamera != nullptr
        && m_camera_system->mainCameraController != nullptr
        && m_camera_system->mainCameraController->mainCamera != nullptr;

    if (!is_active_camera) { 
        return; 
    }

    if (!is_first_person_allowed()) {
        return;
    }

    if (auto param = controller->cameraParam; param != nullptr) {
        auto new_value = (param->fov * m_fov_mult->value()) + m_fov_offset->value();

        if (m_last_camera_type == app::ropeway::camera::CameraControlType::PLAYER) {
            if (m_fov_mult->value() != m_last_fov_mult) {
                auto prev_value = (param->fov * m_last_fov_mult) + m_fov_offset->value();
                auto delta = prev_value - new_value;

                m_fov_offset->value() += delta;
                m_camera_system->mainCameraController->mainCamera->fov = (param->fov * m_fov_mult->value()) + m_fov_offset->value();
                controller->activeCamera->fov = m_camera_system->mainCameraController->mainCamera->fov;
            }
            else {
                m_camera_system->mainCameraController->mainCamera->fov = new_value;
                controller->activeCamera->fov = m_camera_system->mainCameraController->mainCamera->fov;
            }

            m_last_player_fov = controller->activeCamera->fov;
        }
        else {
            if (m_last_player_fov == 0.0f) {
                m_last_player_fov = 90.0f;
            }

            m_camera_system->mainCameraController->mainCamera->fov = m_last_player_fov;
            controller->activeCamera->fov = m_last_player_fov;
        }
        
        // Causes the camera to ignore the FOV inside the param
        param->useParam = !m_enabled->value();
    }
}

void FirstPerson::update_joint_names() {
    if (m_player_transform == nullptr || !m_attach_names.empty()) {
        return;
    }

    auto& joints = m_player_transform->joints;

#if !defined(RE8) && !defined(MHRISE)
    for (int32_t i = 0; joints.data != nullptr && i < joints.size; ++i) {
        auto joint = joints.data->joints[i];
#else
    for (int32_t i = 0; joints.data != nullptr && i < joints.data->numElements; ++i) {
        auto joint = utility::re_array::get_element<REJoint>(joints.data, i);
#endif

        if (joint == nullptr || joint->info == nullptr || joint->info->name == nullptr) {
            continue;
        }

        auto name = std::wstring{ joint->info->name };
        m_attach_names.push_back(utility::narrow(name).c_str());
    }
}

float FirstPerson::update_delta_time(REComponent* component) {
    return utility::re_component::get_delta_time(component);
}

bool FirstPerson::is_first_person_allowed() const {
    if (m_last_pause_state) {
        return false;
    }

    // Don't mess with the camera if we're in a cutscene
    if (m_show_in_cutscenes->value()) {
        return m_allowed_camera_types.count(m_last_camera_type) > 0;
    }
    
    return m_last_camera_type == app::ropeway::camera::CameraControlType::PLAYER;
}

bool FirstPerson::is_jacked(RETransform* transform) const {
    static auto jack_dominator_typedef = sdk::find_type_definition(game_namespace("JackDominator"));
    static auto jacked_method = jack_dominator_typedef->get_method("get_Jacked");

    auto jack_dominator = utility::re_component::find<::REComponent*>(transform, jack_dominator_typedef->get_type());

    if (jack_dominator != nullptr) {
        return jacked_method->call<bool>(sdk::get_thread_context(), jack_dominator);
    }

    return false;
}

void FirstPerson::on_disabled() {
    m_was_gripping_weapon = false;

    VR::get()->set_rotation_offset(glm::identity<glm::quat>());

    if (!update_pointers()) {
        m_wants_disable = false;
        return;
    }

    // restore the muzzle and body IK behavior
    // so we don't lean off to the side when we're not in first person
    // and not shoot out of the camera
    if (m_player_transform != nullptr) {
        update_player_muzzle_behavior(m_player_transform, true);
        update_player_body_ik(m_player_transform, true);
    }

    // Disable fov and camera light changes
    if (m_camera_system != nullptr && m_sweet_light_manager != nullptr) {
        update_fov(m_camera_system->cameraController);
        update_sweet_light_context(utility::ropeway_sweetlight_manager::get_context(m_sweet_light_manager, 0));
        update_sweet_light_context(utility::ropeway_sweetlight_manager::get_context(m_sweet_light_manager, 1));

        if (m_camera_system->mainCameraController != nullptr) {
            m_camera_system->mainCameraController->updateCamera = true;
        }
    }

    m_wants_disable = false;
}

#endif