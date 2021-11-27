#include <unordered_set>

#include <spdlog/spdlog.h>
#include <imgui.h>

#include "utility/Scan.hpp"
#include "REFramework.hpp"
#include "sdk/REMath.hpp"

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

    if (m_enabled->draw("Enabled") && !m_enabled->value()) {
        // Disable fov and camera light changes
        on_disabled();
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

void FirstPerson::on_config_load(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }

    m_last_fov_mult = m_fov_mult->value();

    // turn the patch on
    if (m_disable_vignette->value()) {
        //m_disable_vignettePatch->toggle(m_disable_vignette->value());
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
        m_last_camera_type = utility::re_managed_object::get_field<app::ropeway::camera::CameraControlType>(m_camera_system, "BusyCameraType");
        m_cached_bone_matrix = nullptr;

        update_player_transform(m_player_transform);
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

    if (m_camera_system != nullptr && m_camera_system->ownerGameObject != nullptr && transform == m_camera_system->ownerGameObject->transform) {
        if (!update_pointers_from_camera_system(m_camera_system)) {
            reset();
            return;
        }
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

    if (!is_first_person_allowed()) {
        return;
    }

    auto nudged_mtx = m_last_camera_matrix;
    //nudged_mtx[3] += VR::get()->get_current_offset();

    // The following code fixes inaccuracies between the rotation set by the game and what's set in updateCameraTransform
    controller->worldPosition = nudged_mtx[3];
    *(glm::quat*)&controller->worldRotation = glm::quat{ nudged_mtx  };

    m_camera->ownerGameObject->transform->worldTransform = nudged_mtx;
    m_camera->ownerGameObject->transform->angles = *(Vector4f*)&controller->worldRotation;
}

void FirstPerson::on_update_camera_controller2(RopewayPlayerCameraController* controller) {
    if (!m_enabled->value() || m_player_transform == nullptr || controller != m_camera_system->cameraController) {
        return;
    }

    // Just update the FOV in here. Whatever.
    update_fov(controller);

    if (m_camera_system->cameraController == m_player_camera_controller) {
        if (m_ignore_next_player_angles) {
            // keep ignoring player input until no longer switching cameras
            if (m_ignore_next_player_angles && !utility::re_managed_object::get_field<bool>(m_camera_system->mainCameraController, "SwitchingCamera")) {
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
        default:
            break;
    }
}

void FirstPerson::on_application_entry(void* entry, const char* name, size_t hash) {
    
}

void FirstPerson::on_pre_update_behavior(void* entry) {
    if (m_toggle_key->is_key_down_once() && !m_enabled->toggle()) {
        on_disabled();
    }

    if (!m_enabled->value()) {
        return;
    }

    // Update our global pointers
    if (m_post_effect_controller == nullptr || m_post_effect_controller->ownerGameObject == nullptr || 
        m_camera_system == nullptr || m_camera_system->ownerGameObject == nullptr || m_sweet_light_manager == nullptr || m_sweet_light_manager->ownerGameObject == nullptr
        || m_gui_master == nullptr) 
    {
        auto& globals = *g_framework->get_globals();
        m_sweet_light_manager = globals.get<RopewaySweetLightManager>(game_namespace("SweetLightManager"));
        m_camera_system = globals.get<RopewayCameraSystem>(game_namespace("camera.CameraSystem"));
        m_post_effect_controller = globals.get<RopewayPostEffectController>(game_namespace("posteffect.PostEffectController"));
        m_gui_master = globals.get<REBehavior>(game_namespace("gui.GUIMaster"));

        reset();
        return;
    }
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

void FirstPerson::update_player_transform(RETransform* transform) {
    if (!m_enabled->value() || m_camera_system == nullptr) {
        return;
    }

    // so we don't go spinning everywhere in cutscenes
    if (m_last_camera_type != app::ropeway::camera::CameraControlType::PLAYER) {
        return;
    }

    auto vr_mod = VR::get();
    auto& controllers = vr_mod->get_controllers();
    const auto is_hmd_active = vr_mod->is_hmd_active();
    const auto is_using_controllers = vr_mod->is_using_controllers();

    auto context = sdk::get_thread_context();

    if (!controllers.empty()) {
        auto update_joint = [&](std::wstring_view name, int32_t controller_index) {
            auto wrist_joint = utility::re_transform::get_joint(*transform, name);

            if (wrist_joint == nullptr) {
                return;
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

            // Get Arm IK component
            auto arm_fit = utility::re_component::find<REComponent>(transform, game_namespace("IkArmFit"));

            // We will use the game's IK system instead of building our own because it's a pain in the ass
            // The arm fit component by default will only update the left wrist position (I don't know why, maybe the right arm is a blended animation?)
            // So we will not only abuse that, but we will repurpose it to update the right arm as well
            if (arm_fit != nullptr) {
                auto arm_fit_t = utility::re_managed_object::get_type_definition(arm_fit);
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
                                    auto shoulder_joint = sdk::get_joint_parent(elbow_joint);

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
                    
                        // Set the target matrix to the VR controller's position (new_pos, rotation_quat)
                        target_matrix = Matrix4x4f{ rotation_quat };
                        target_matrix[3] = new_pos;

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
            update_joint(L"l_arm_wrist", 0);
            update_joint(L"r_arm_wrist", 1);
        }
    }

    if (is_hmd_active) {
        // We're going to modify the player's weapon (gun) to fire from the muzzle instead of the camera
        // Luckily the game has that built-in so we don't really need to hook anything
        auto equipment = utility::re_component::find<REComponent>(transform, game_namespace("survivor.Equipment"));

        if (equipment != nullptr) {
            auto equipment_t = utility::re_managed_object::get_type_definition(equipment);
            auto main_weapon_field = equipment_t->get_field("<EquipWeapon>k__BackingField");
            auto& main_weapon = main_weapon_field->get_data<REManagedObject*>(equipment);

            if (main_weapon != nullptr) {
                auto main_weapon_game_object = sdk::call_object_func<REGameObject*>(main_weapon, "get_GameObject", context, main_weapon);
                auto main_weapon_transform = main_weapon_game_object != nullptr ? main_weapon_game_object->transform : (RETransform*)nullptr;

                static auto implement_gun_typedef = sdk::RETypeDB::get()->find_type(game_namespace("implement.Gun"));
                static auto implement_melee_typedef = sdk::RETypeDB::get()->find_type(game_namespace("implement.Melee"));

                auto main_weapon_t = utility::re_managed_object::get_type_definition(main_weapon);

                if (main_weapon_game_object != nullptr && main_weapon_t != nullptr && main_weapon_t->is_a(implement_gun_typedef)) {
                    auto& fire_bullet_param = *sdk::get_object_field<REManagedObject*>(main_weapon, "<FireBulletParam>k__BackingField");

                    if (fire_bullet_param != nullptr) {
                        auto fire_bullet_param_t = utility::re_managed_object::get_type_definition(fire_bullet_param);
                        auto& fire_bullet_type = *sdk::get_object_field<app::ropeway::weapon::shell::ShellDefine::FireBulletType>(fire_bullet_param, "_FireBulletType");

                        // Set the fire bullet type to AlongMuzzle, which fires from the muzzle's position and rotation
                        if (is_using_controllers) {
                            fire_bullet_type = app::ropeway::weapon::shell::ShellDefine::FireBulletType::AlongMuzzle;
                        } else {
                            fire_bullet_type = app::ropeway::weapon::shell::ShellDefine::FireBulletType::Camera;
                        }

                        auto muzzle_joint_param = *sdk::get_object_field<REManagedObject*>(fire_bullet_param, "_MuzzleJointParameter");
                        auto muzzle_joint_extra = *sdk::get_object_field<REManagedObject*>(main_weapon, "<MuzzleJoint>k__BackingField");

                        // Set the muzzle joint to the VFX muzzle position used for stuff like muzzle flashes
                        if (muzzle_joint_param != nullptr && muzzle_joint_extra != nullptr) {
                            auto vfx_muzzle1 = utility::re_transform::get_joint(*main_weapon_transform, L"vfx_muzzle1");
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
                        // TODO!
                    }
                }
            }
        }

        static auto ik_leg_def = sdk::RETypeDB::get()->find_type("via.motion.IkLeg");
        static auto via_motion_def = sdk::RETypeDB::get()->find_type("via.motion.Motion");
        auto ik_leg = utility::re_component::find<REComponent>(transform, ik_leg_def->type);
        auto via_motion = utility::re_component::find<REComponent>(transform, via_motion_def->type);

        // We're going to use the leg IK to adjust the height of the player according to headset position
        if (ik_leg != nullptr && via_motion != nullptr) {
            const auto headset_pos = vr_mod->get_position(0);
            const auto standing_origin = vr_mod->get_standing_origin();
            const auto hmd_offset = headset_pos - standing_origin;

            // Create a final offset which will keep the player's head where they want
            // while also stabilizing any undesired head movement from the original animation
            Vector4f final_offset{ hmd_offset };

            if (!is_using_controllers) {
                const auto forward_matrix = utility::math::remove_y_component(Matrix4x4f{glm::normalize(m_last_controller_rotation_vr)});
                final_offset = forward_matrix * final_offset;
            } else {
                final_offset = m_last_controller_rotation_vr * final_offset;
            }

            const auto smooth_xz_movement = m_smooth_xz_movement->value();
            const auto smooth_y_movement = m_smooth_y_movement->value();

            if (smooth_xz_movement || smooth_y_movement) {
                auto center_joint = utility::re_transform::get_joint(*transform, L"COG");
                auto head_joint = utility::re_transform::get_joint(*transform, L"head");

                if (head_joint != nullptr && center_joint != nullptr) {
                    const auto head_joint_index = ((sdk::Joint*)head_joint)->get_joint_index();

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
        }
    }

    if (!m_rotate_body->value()) {
        return;
    }

    auto player_matrix = glm::mat4{ *(glm::quat*) & transform->angles };

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

void FirstPerson::update_camera_transform(RETransform* transform) {
    std::lock_guard _{ m_matrix_mutex };

    m_last_camera_type = utility::re_managed_object::get_field<app::ropeway::camera::CameraControlType>(m_camera_system, "BusyCameraType");

    // Don't mess with the camera if we're in a cutscene
    if (!is_first_person_allowed()) {
        return;
    }

    auto delta_time = update_delta_time(transform);

    auto& mtx = transform->worldTransform;
    auto& camera_pos = mtx[3];

    auto cam_pos3 = Vector3f{ m_last_controller_pos };

    auto camera_matrix = m_last_camera_matrix_pre_vr * Matrix4x4f{
        -1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, -1, 0,
        0, 0, 0, 1
    };

    const auto gui_state = *sdk::get_object_field<int32_t>(m_gui_master, "<State_>k__BackingField");
    const auto is_paused = gui_state == (int32_t)app::ropeway::gui::GUIMaster::GuiState::PAUSE || gui_state == (int32_t)app::ropeway::gui::GUIMaster::GuiState::INVENTORY;
    const auto is_player_camera = m_last_camera_type == app::ropeway::camera::CameraControlType::PLAYER;
    const auto is_switching_camera = utility::re_managed_object::get_field<bool>(m_camera_system->mainCameraController, "SwitchingCamera");
    const auto is_player_in_control = (is_player_camera && !is_switching_camera && !is_paused);
    const auto is_switching_to_player_camera = is_player_camera && is_switching_camera;
    //is_player_camera = is_player_camera && !is_switching_camera;

    // fix some crazy spinning bullshit
    if (is_paused) {
        *(Matrix3x4f*)&mtx = m_last_camera_matrix_pre_vr;
    }

    const auto wanted_camera_shake = VR::get()->is_hmd_active() ? 0.0f : m_bone_scale->value();
    const auto wanted_camera_speed = VR::get()->is_hmd_active() ? 100.0f : m_camera_scale->value();

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
    if (!is_player_camera && !is_switching_camera) {
        m_camera_system->mainCameraController->updateCamera = false;
    }
    else {
        m_camera_system->mainCameraController->updateCamera = true;
    }

    camera_matrix[3] = m_last_bone_matrix[3];
    auto& bone_pos = camera_matrix[3];

    // Fix rotation after returning to player control
    if (is_player_in_control && m_has_cutscene_rotation) {
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

        *(Matrix3x4f*)&mtx = cutscene_inverse * m_last_camera_matrix_pre_vr;

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
    if (VR::get()->is_using_controllers()) {
        cam_forward3[1] = 0.0f;
        cam_forward3 = glm::normalize(cam_forward3);
    }

    auto attach_offset = m_attach_offsets[m_player_name];

    if (VR::get()->is_hmd_active()) {
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
        m_interpolated_bone = glm::interpolate(m_interpolated_bone, head_rot_mat, delta_time * bone_scale * dist);
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

    const auto mtx_pre_vr = mtx;
    
    const auto last_headset_rotation = !is_player_in_control ? m_last_headset_rotation_pre_cutscene : glm::identity<Matrix4x4f>();
    const auto inverse_last_headset_rotation = !is_player_in_control ? glm::inverse(m_last_headset_rotation_pre_cutscene) : glm::identity<Matrix4x4f>();

    const auto final_mat_pre_vr = final_mat;
    const auto final_quat_pre_vr = glm::normalize(glm::quat{final_mat});

    const auto real_headset_rotation = VR::get()->get_rotation(0);
    const auto headset_rotation = inverse_last_headset_rotation * real_headset_rotation;

    //if (!is_paused && m_has_cutscene_rotation) {
        //final_mat *= glm::inverse(m_last_headset_rotation_pre_cutscene);
    //}
    
    // do not interpolate the headset rotation to reduce motion sickness
    //if (is_player_in_control || m_has_cutscene_rotation || is_paused) {
        final_mat *= headset_rotation;
    //}

    auto final_quat = glm::quat{ final_mat };

    // Apply the same matrix data to other things stored in-game (positions/quaternions)
    camera_pos = Vector4f{ final_pos, 1.0f };
    m_camera_system->cameraController->worldPosition = *(Vector4f*)&camera_pos;
    m_camera_system->cameraController->worldRotation = *(Vector4f*)&final_quat;
    transform->position = *(Vector4f*)&camera_pos;
    transform->angles = *(Vector4f*)&final_quat;

    // Apply the new matrix
    *(Matrix3x4f*)&mtx = final_mat;

    //if (is_player_in_control || !is_player_camera) {
        m_last_camera_matrix = mtx;
        m_last_camera_matrix_pre_vr = mtx_pre_vr;
    //}

    // Fixes snappiness after camera switching
    if (!is_player_in_control) {
        m_last_controller_pos = m_camera_system->cameraController->worldPosition;
        m_last_controller_rotation = final_quat_pre_vr;

        m_camera_system->mainCameraController->cameraPosition = m_last_controller_pos;
        m_camera_system->mainCameraController->cameraRotation = *(Vector4f*)&final_quat_pre_vr;
        m_camera_system->cameraController->worldRotation = *(Vector4f*)&final_quat_pre_vr;

        //if (!is_switching_to_player_camera) {
            m_last_controller_angles = utility::math::euler_angles(final_mat_pre_vr);
        //}

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
            //if (!is_switching_to_player_camera) {
                m_player_camera_controller->pitch = m_last_controller_angles.x;
                m_player_camera_controller->yaw = m_last_controller_angles.y;
            //}

            m_ignore_next_player_angles = !is_switching_to_player_camera;
        }
    }

    if (transform->joints.matrices != nullptr) {
        auto joint = utility::re_transform::get_joint(*transform, 0);

        if (joint != nullptr) {
            joint->posOffset = Vector4f{};
            *(Vector4f*)&joint->anglesOffset = Vector4f{ 0.0f, 0.0f, 0.0f, 1.0f };
        }

        transform->joints.matrices->data[0].worldMatrix = m_last_camera_matrix;
    }

    m_last_controller_rotation_vr = glm::quat { cam_rot_mat };

    // Keep track of the last pre-cutscene headset rotation
    // so the player doesn't do something like flip 180 when a cutscene starts
    // which would be especially prevalent for a user with a large playspace
    if (!is_player_in_control && !m_has_cutscene_rotation) {
        m_last_headset_rotation_pre_cutscene = utility::math::remove_y_component(real_headset_rotation);
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
    // Don't mess with the camera if we're in a cutscene
    if (m_show_in_cutscenes->value()) {
        return m_allowed_camera_types.count(m_last_camera_type) > 0;
    }
    
    return m_last_camera_type == app::ropeway::camera::CameraControlType::PLAYER;
}

void FirstPerson::on_disabled() {
    // Disable fov and camera light changes
    if (m_camera_system != nullptr && m_sweet_light_manager != nullptr) {
        update_fov(m_camera_system->cameraController);
        update_sweet_light_context(utility::ropeway_sweetlight_manager::get_context(m_sweet_light_manager, 0));
        update_sweet_light_context(utility::ropeway_sweetlight_manager::get_context(m_sweet_light_manager, 1));

        if (m_camera_system->mainCameraController != nullptr) {
            m_camera_system->mainCameraController->updateCamera = true;
        }
    }
}

#endif