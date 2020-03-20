#include <unordered_set>

#include <spdlog/spdlog.h>
#include <imgui/imgui.h>

#include "utility/Scan.hpp"
#include "REFramework.hpp"

#include "FirstPerson.hpp"

FirstPerson* g_first_person = nullptr;

FirstPerson::FirstPerson() {
    // thanks imgui
    g_first_person = this;
    m_attach_bone_imgui.reserve(256);

#ifdef RE3
    // Jill (it looks better with 0?)
    m_attach_offsets["pl2000"] = Vector4f{ 0.0f, 0.0f, 0.0f, 0.0f };
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
    if (m_toggle_key->is_key_down_once() && !m_enabled->toggle()) {
        on_disabled();
    }

    if (!m_enabled->value()) {
        return;
    }

    // Update our global pointers
    if (m_post_effect_controller == nullptr || m_post_effect_controller->ownerGameObject == nullptr || 
        m_camera_system == nullptr || m_camera_system->ownerGameObject == nullptr || m_sweet_light_manager == nullptr || m_sweet_light_manager->ownerGameObject == nullptr) 
    {
        auto& globals = *g_framework->get_globals();
        m_sweet_light_manager = globals.get<RopewaySweetLightManager>(game_namespace("SweetLightManager"));
        m_camera_system = globals.get<RopewayCameraSystem>(game_namespace("camera.CameraSystem"));
        m_post_effect_controller = globals.get<RopewayPostEffectController>(game_namespace("posteffect.PostEffectController"));

        reset();
        return;
    }
}

void FirstPerson::on_draw_ui() {
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);

    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    std::lock_guard _{ m_frame_mutex };

    if (m_enabled->draw("Enabled") && !m_enabled->value()) {
        // Disable fov and camera light changes
        on_disabled();
    }

    ImGui::SameLine();

    // Revert the updateCamera value to normal
    if (m_show_in_cutscenes->draw("Show In Cutscenes") && m_camera_system != nullptr && m_camera_system->mainCameraController != nullptr) {
        m_camera_system->mainCameraController->updateCamera = true;
    }

    m_disable_light_source->draw("Disable Camera Light");
    m_hide_mesh->draw("Hide Joint Mesh");

    ImGui::SameLine();
    m_rotate_mesh->draw("Force Rotate Joint");

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

void FirstPerson::on_pre_update_transform(RETransform* transform) {
    if (!m_enabled->value() || m_camera == nullptr || m_camera->ownerGameObject == nullptr) {
        return;
    }

    if (m_player_camera_controller == nullptr || m_player_transform == nullptr || m_camera_system == nullptr || m_camera_system->cameraController == nullptr) {
        return;
    }

    // can change to action camera
    if (!m_show_in_cutscenes->value() && m_camera_system->cameraController != m_player_camera_controller) {
        return;
    }

    // We need to lock a mutex because these UpdateTransform functions
    // are called from multiple threads
    if (transform == m_player_transform) {
        g_in_player_transform = true;
        g_first_time = true;
        m_matrix_mutex.lock();
    }
    // This is because UpdateTransform recursively calls UpdateTransform on its children,
    // and the player transform (topmost) is the one that actually updates the bone matrix,
    // and all the child transforms operate on the bones that it updated
    else if (g_in_player_transform) {
        update_player_bones(m_player_transform);
    }
}

void FirstPerson::on_update_transform(RETransform* transform) {
    // Do this first before anything else.
    if (g_in_player_transform && transform == m_player_transform) {
        update_joint_names();

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

    if (m_sweet_light_manager == nullptr) {
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
        // Don't mess with the camera if we're in a cutscene
        if (!m_show_in_cutscenes->value() && m_camera_system->cameraController != m_player_camera_controller) {
            return;
        }

        update_camera_transform(transform);
        update_fov(m_camera_system->cameraController);
    }
}

void FirstPerson::on_update_camera_controller(RopewayPlayerCameraController* controller) {
    if (!m_enabled->value() || m_player_transform == nullptr || controller != m_camera_system->cameraController) {
        return;
    }

    if (!m_show_in_cutscenes->value() && m_camera_system->cameraController != m_player_camera_controller) {
        return;
    }

#ifdef RE3
    // Just update the FOV in here. Whatever.
    update_fov(controller);

    // Save the original position and rotation before our modifications.
    // If we don't, the camera rotation will freeze up, because it keeps getting overwritten.
    m_last_controller_pos = controller->worldPosition;
    m_last_controller_rotation = *(glm::quat*)&controller->worldRotation;
#endif

    // The following code fixes inaccuracies between the rotation set by the game and what's set in updateCameraTransform
    controller->worldPosition = m_last_camera_matrix[3];
    *(glm::quat*)&controller->worldRotation = glm::quat{ m_last_camera_matrix };

    m_camera->ownerGameObject->transform->worldTransform = m_last_camera_matrix;
    m_camera->ownerGameObject->transform->angles = *(Vector4f*)&controller->worldRotation;
}

void FirstPerson::on_update_camera_controller2(RopewayPlayerCameraController* controller) {
    if (!m_enabled->value() || m_player_transform == nullptr || controller != m_camera_system->cameraController) {
        return;
    }

    // Just update the FOV in here. Whatever.
    update_fov(controller);

    // Save the original position and rotation before our modifications.
    // If we don't, the camera rotation will freeze up, because it keeps getting overwritten.
    m_last_controller_pos = controller->worldPosition;
    m_last_controller_rotation = *(glm::quat*)&controller->worldRotation;
}

void FirstPerson::reset() {
    m_rotation_offset = glm::identity<decltype(m_rotation_offset)>();
    m_interpolated_bone = glm::identity<decltype(m_interpolated_bone)>();
    m_last_camera_matrix = glm::identity<decltype(m_last_camera_matrix)>();
    m_last_bone_matrix = glm::identity<decltype(m_last_bone_matrix)>();
    m_last_controller_pos = Vector4f{};
    m_last_controller_rotation = glm::quat{};

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

void FirstPerson::update_camera_transform(RETransform* transform) {
    std::lock_guard _{ m_matrix_mutex };

    auto delta_time = update_delta_time(transform);

    auto& mtx = transform->worldTransform;
    auto& camera_pos = mtx[3];

    auto cam_pos3 = Vector3f{ m_last_controller_pos };

    auto camera_matrix = m_last_camera_matrix * Matrix4x4f{
        -1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, -1, 0,
        0, 0, 0, 1
    };

    auto is_player_camera = m_camera_system->cameraController == m_player_camera_controller;
    auto bone_scale = is_player_camera ? (m_bone_scale->value() * 0.01f) : 1.0f;

    // Lets camera modification work in cutscenes/action camera etc
    if (!is_player_camera) {
        m_camera_system->mainCameraController->updateCamera = false;
    }
    else {
        m_camera_system->mainCameraController->updateCamera = true;
    }

    camera_matrix[3] = m_last_bone_matrix[3];
    auto& bone_pos = camera_matrix[3];

    auto cam_rot_mat = glm::extractMatrixRotation(Matrix4x4f{ m_last_controller_rotation });
    auto head_rot_mat = glm::extractMatrixRotation(m_last_bone_matrix) * Matrix4x4f {
        -1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, -1, 0,
        0, 0, 0, 1
    };

    auto& cam_forward3 = *(Vector3f*)&cam_rot_mat[2];

    auto offset = glm::extractMatrixRotation(camera_matrix) * (m_attach_offsets[m_player_name] * Vector4f{ -0.1f, 0.1f, 0.1f, 0.0f });
    auto final_pos = Vector3f{ bone_pos + offset };

    // Average the distance to the wanted rotation
    auto dist = (glm::distance(m_interpolated_bone[0], head_rot_mat[0])
               + glm::distance(m_interpolated_bone[1], head_rot_mat[1])
               + glm::distance(m_interpolated_bone[2], head_rot_mat[2])) / 3.0f;

    // interpolate the bone rotation (it's snappy otherwise)
    m_interpolated_bone = glm::interpolate(m_interpolated_bone, head_rot_mat, delta_time * bone_scale * dist);

    // Look at where the camera is pointing from the head position
    cam_rot_mat = glm::extractMatrixRotation(glm::rowMajor4(glm::lookAtLH(final_pos, cam_pos3 + (cam_forward3 * 8192.0f), { 0.0f, 1.0f, 0.0f })));
    // Follow the bone rotation, but rotate towards where the camera is looking.
    auto wanted_mat = glm::inverse(m_interpolated_bone) * cam_rot_mat;

    // Average the distance to the wanted rotation
    dist = (glm::distance(m_rotation_offset[0], wanted_mat[0])
          + glm::distance(m_rotation_offset[1], wanted_mat[1])
          + glm::distance(m_rotation_offset[2], wanted_mat[2])) / 3.0f;

    m_rotation_offset = glm::interpolate(m_rotation_offset, wanted_mat, delta_time * (m_camera_scale->value() * 0.01f) * dist);

    auto final_mat = is_player_camera ? (m_interpolated_bone * m_rotation_offset) : m_interpolated_bone;
    auto final_quat = glm::quat{ final_mat };

    // Apply the same matrix data to other things stored in-game (positions/quaternions)
    camera_pos = Vector4f{ final_pos, 1.0f };
    m_camera_system->cameraController->worldPosition = *(Vector4f*)&camera_pos;
    m_camera_system->cameraController->worldRotation = *(Vector4f*)&final_quat;
    transform->position = *(Vector4f*)&camera_pos;
    transform->angles = *(Vector4f*)&final_quat;

    // Apply the new matrix
    *(Matrix3x4f*)&mtx = final_mat;
    m_last_camera_matrix = mtx;

    // Fixes snappiness after camera switching
    if (!is_player_camera) {
        m_last_controller_pos = m_camera_system->cameraController->worldPosition;
        m_last_controller_rotation = final_quat;

        m_camera_system->mainCameraController->cameraPosition = m_last_controller_pos;
        m_camera_system->mainCameraController->cameraRotation = *(Vector4f*)&final_quat;

        /*m_playerCameraController->ownerGameObject->transform->position = m_lastControllerPos;
        m_playerCameraController->ownerGameObject->transform->angles = *(Vector4f*)&finalQuat;
        m_playerCameraController->worldPosition = m_lastControllerPos;
        m_playerCameraController->worldRotation = *(Vector4f*)&finalQuat;*/
    }

    if (transform->joints.size >= 1 && transform->joints.matrices != nullptr) {
        transform->joints.matrices->data[0].worldMatrix = m_last_camera_matrix;
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
    auto& bone_matrix = utility::re_transform::get_joint_matrix(*m_player_transform, m_attach_bone);
    
    if (g_first_time) {
        m_last_bone_matrix = bone_matrix;
        g_first_time = false;
    }

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

    if (!m_show_in_cutscenes->value() && m_camera_system->cameraController != m_player_camera_controller) {
        return;
    }

    if (auto param = controller->cameraParam; param != nullptr) {
        auto new_value = (param->fov * m_fov_mult->value()) + m_fov_offset->value();

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
        
        // Causes the camera to ignore the FOV inside the param
        param->useParam = !m_enabled->value();
    }
}

void FirstPerson::update_joint_names() {
    if (m_player_transform == nullptr || !m_attach_names.empty()) {
        return;
    }

    auto& joints = m_player_transform->joints;

    for (int32_t i = 0; joints.data != nullptr && i < joints.size; ++i) {
        auto joint = joints.data->joints[i];

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

