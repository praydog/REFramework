#include "reframework/API.hpp"
#include "sdk/SceneManager.hpp"

#include "../VR.hpp"
#include "../Camera.hpp"

#include "HookManager.hpp"
#include "CameraDuplicator.hpp"

namespace vrmod {
void CameraDuplicator::on_pre_application_entry(void* entry, const char* name, size_t hash) {
    if (hash == "LockScene"_fnv) {
        if (!VR::get()->is_hmd_active()) {
            return;
        }

        if (VR::get()->get_rendering_technique() != VR::RenderingTechnique::MULTIPASS) {
            return;
        }

        clone_camera();
        find_new_camera();
    }

}

void CameraDuplicator::fix_in_control() {

    if (!m_camerasystem_fixed)
        return;

    auto new_camera_go = utility::re_component::get_game_object((REComponent*)m_new_camera);
    auto camera_go = utility::re_component::get_game_object((REComponent*)m_old_camera);

    auto& globals = *reframework::get_globals();
    RopewayCameraSystem* m_camera_system = globals.get<RopewayCameraSystem>(game_namespace("camera.CameraSystem"));
    auto m_gui_master = globals.get<REBehavior>(game_namespace("gui.GUIMaster"));
    const auto gui_state = *sdk::get_object_field<int32_t>(m_gui_master, "<State_>k__BackingField");
    const auto is_paused = gui_state == (int32_t)app::ropeway::gui::GUIMaster::GuiState::PAUSE ||
                           gui_state == (int32_t)app::ropeway::gui::GUIMaster::GuiState::INVENTORY;

    auto camera_type = utility::re_managed_object::get_field<app::ropeway::camera::CameraControlType>(m_camera_system, "BusyCameraType");

    const auto is_player_camera = camera_type == app::ropeway::camera::CameraControlType::PLAYER;
    const auto is_switching_camera = utility::re_managed_object::get_field<bool>(m_camera_system->mainCameraController, "SwitchingCamera");
    const auto is_player_in_control = (is_player_camera && !is_switching_camera && !is_paused);

    if (is_player_in_control && m_camera_was_not_in_control)
    {
        m_camera_was_not_in_control = false;
        return;
    }

    new_camera_go->shouldUpdate = !is_player_in_control;
    m_camera_was_not_in_control = !is_player_in_control;

    
    const auto new_transform = new_camera_go->transform;
    const auto old_transform = camera_go->transform;

    
    if (new_transform != nullptr && old_transform != nullptr) {

        sdk::set_transform_position(new_transform, sdk::get_transform_position(old_transform));
        sdk::set_transform_rotation(new_transform, sdk::get_transform_rotation(old_transform));
        new_transform->worldTransform = old_transform->worldTransform;

    }

}

void CameraDuplicator::on_application_entry(void* entry, const char* name, size_t hash) {
    // Probably the best place to do it, it doesn't waste unnecessary time
    // after waiting for the render thread to finish, it does it before waiting
    if (hash == "LockScene"_fnv) {
        if (!VR::get()->is_hmd_active()) {
            return;
        }

        if (VR::get()->get_rendering_technique() != VR::RenderingTechnique::MULTIPASS) {
            return;
        }

        fix_camera_system();
        fix_in_control();
        copy_camera_properties(true);
    }
}

void CameraDuplicator::fix_camera_system() {

    if (m_new_camera == nullptr)
        return;

    auto& globals = *reframework::get_globals();
    RopewayCameraSystem* m_camera_system = globals.get<RopewayCameraSystem>(game_namespace("camera.CameraSystem"));

    if (m_camera_system != nullptr) {

        auto camera_go = utility::re_component::get_game_object((REComponent*)m_old_camera);
        auto new_camera_go = utility::re_component::get_game_object((REComponent*)m_new_camera);


        m_camerasystem_fixed = true;

        m_camera_system->mainCamera = m_old_camera;
        m_camera_system->cameraGameObject = camera_go;

        m_camera_system->mainCameraController->updateCamera = true;
        m_camera_system->mainCameraController->mainCamera = m_old_camera;
        m_camera_system->mainCameraController->mainCameraObject = camera_go;

   
    }
}

void CameraDuplicator::on_draw_ui() {
    if (ImGui::TreeNode("Camera Duplicator")) {
        ImGui::Checkbox("Copy camera properties", &m_copy_camera);
        for (auto& descriptor : m_wanted_components) {
            ImGui::Checkbox(descriptor.name.c_str(), &descriptor.allowed);
        }
        ImGui::TreePop();
    }
}

void CameraDuplicator::clone_camera() {
    // Untested on older than TDB 69 (RE8)
#if TDB_VER >= 69
    if (m_new_camera != nullptr || m_called_activate) {
        return;
    }

    const auto existing_cameras = get_all_cameras();

    if (existing_cameras == nullptr) {
        return;
    }

    size_t num_main_cameras = 0;

    for (::REManagedObject* camera : *existing_cameras) {
        if (camera == nullptr) {
            continue;
        }

        const auto gameobject = utility::re_component::get_game_object((REComponent*)camera);

        if (gameobject == nullptr) {
            continue;
        }

        const auto name = utility::re_string::get_view(gameobject->name);

        if (name == L"Main Camera" || name == L"MainCamera") {
            if (num_main_cameras == 0) {
                m_old_camera = (RECamera*)camera;
            } else {
                m_new_camera = (RECamera*)camera;
            }

            ++num_main_cameras;
        }
    }

    spdlog::info("Found {} main cameras", num_main_cameras);

    if (num_main_cameras > 1) {
        spdlog::info("Not creating new camera because there are already {} main cameras", num_main_cameras);
        m_called_activate = true;
        return;
    }

    if (m_old_camera == nullptr) {
        spdlog::info("Not creating new camera yet because there is no main camera");
        return;
    }

    const auto old_camera_gameobject = utility::re_component::get_game_object((REComponent*)m_old_camera);

    if (old_camera_gameobject == nullptr) {
        return;
    }

    auto old_camera_folder = old_camera_gameobject->folder;

    if (old_camera_folder == nullptr) {
        return;
    }

    // Setting active to false lets us "activate" the folder again, cloning the camera perfectly
    constexpr auto ACTIVE_OFFSET = sizeof(::REManagedObject) + 5;
    *(bool*)((uint8_t*)old_camera_folder + ACTIVE_OFFSET) = false;

    sdk::call_object_func_easy<void*>(old_camera_folder, "activate");
    m_called_activate = true;
#endif
}

void CameraDuplicator::find_new_camera() {
#if TDB_VER >= 69
    if (!m_called_activate || m_new_camera != nullptr) {
        return;
    }

    const auto existing_cameras = get_all_cameras();

    if (existing_cameras == nullptr) {
        return;
    }

    for (::REManagedObject* camera : *existing_cameras) {
        if (camera == nullptr) {
            continue;
        }

        const auto gameobject = utility::re_component::get_game_object((REComponent*)camera);

        if (gameobject == nullptr) {
            continue;
        }

        const auto name = utility::re_string::get_view(gameobject->name);

        if (name == L"Main Camera" || name == L"MainCamera") {
            if ((RECamera*)camera != m_old_camera) {


                // FIX: the new camera becomes the main one
                m_new_camera = m_old_camera;
                m_old_camera = (RECamera*)camera;
                copy_camera_properties(false);


                spdlog::info("Found new camera: {:x}", (uintptr_t)camera);

                const auto new_transform = m_new_camera->ownerGameObject->transform;
                const auto old_transform = m_old_camera->ownerGameObject->transform;

                if (new_transform != nullptr && old_transform != nullptr) {

                    sdk::set_transform_position(old_transform, sdk::get_transform_position(new_transform));
                    sdk::set_transform_rotation(old_transform, sdk::get_transform_rotation(new_transform));
                    old_transform->worldTransform = new_transform->worldTransform;
                }

                break;
            }
        }
    }

    if (m_new_camera != nullptr) {
        const auto sceneview_t = sdk::find_type_definition("via.SceneView");
        const auto get_primary_camera_fn = sceneview_t->get_method("get_PrimaryCamera");
        spdlog::info("Hooking getPrimaryCamera: {:x}", (uintptr_t)get_primary_camera_fn);

        g_hookman.add(get_primary_camera_fn, [this](std::vector<uintptr_t>& args, std::vector<sdk::RETypeDefinition*>& arg_tys) {
            if (m_new_camera == nullptr) {
                return HookManager::PreHookResult::CALL_ORIGINAL;
            }

            constexpr auto PRIORITY_OFFSET = 0x48;
            *(int32_t*)((uint8_t*)m_new_camera + PRIORITY_OFFSET) = -1; // causes the game to NOT use the new camera as the main camera
            
            return HookManager::PreHookResult::CALL_ORIGINAL;
        }, 
        [](uintptr_t& ret_val, sdk::RETypeDefinition* ret_ty) {


        });
    }
#endif
}

void CameraDuplicator::copy_camera_properties(bool invert) {
#if TDB_VER >= 69
    if (!m_copy_camera) {
        return;
    }

    if (m_old_camera == nullptr || m_new_camera == nullptr) {
        return;
    }

    auto original_camera = invert ? m_old_camera : m_new_camera;
    auto new_camera = invert ? m_new_camera : m_old_camera;

    const auto old_camera_gameobject = utility::re_component::get_game_object((REComponent*)original_camera);
    const auto new_camera_gameobject = utility::re_component::get_game_object((REComponent*)new_camera);

    if (old_camera_gameobject == nullptr || new_camera_gameobject == nullptr) {
        return;
    }
    


    static auto via_camera = sdk::find_type_definition("via.Camera");
    static auto get_near_clip_plane_method = via_camera->get_method("get_NearClipPlane");
    static auto set_near_clip_plane_method = via_camera->get_method("set_NearClipPlane");
    static auto get_far_clip_plane_method = via_camera->get_method("get_FarClipPlane");
    static auto set_far_clip_plane_method = via_camera->get_method("set_FarClipPlane");
    static auto get_fov_method = via_camera->get_method("get_FOV");
    static auto set_fov_method = via_camera->get_method("set_FOV");
    static auto get_vertical_enable_method = via_camera->get_method("get_VerticalEnable");
    static auto set_vertical_enable_method = via_camera->get_method("set_VerticalEnable");
    static auto set_aspect_ratio_method = via_camera->get_method("set_AspectRatio");
    static auto get_aspect_ratio_method = via_camera->get_method("get_AspectRatio");

    if (get_near_clip_plane_method != nullptr && set_near_clip_plane_method != nullptr) {
        const auto old_near_clip_plane = get_near_clip_plane_method->call<float>(sdk::get_thread_context(), original_camera);
        set_near_clip_plane_method->call<void>(sdk::get_thread_context(), new_camera, old_near_clip_plane);
    }

    if (get_far_clip_plane_method != nullptr && set_far_clip_plane_method != nullptr) {
        const auto old_far_clip_plane = get_far_clip_plane_method->call<float>(sdk::get_thread_context(), original_camera);
        set_far_clip_plane_method->call<void>(sdk::get_thread_context(), new_camera, old_far_clip_plane);
    }

    if (get_fov_method != nullptr && set_fov_method != nullptr) {
        const auto old_fov = get_fov_method->call<float>(sdk::get_thread_context(), original_camera);
        set_fov_method->call<void>(sdk::get_thread_context(), new_camera, old_fov);
    }

    if (get_vertical_enable_method != nullptr && set_vertical_enable_method != nullptr) {
        const auto old_vertical_enable = get_vertical_enable_method->call<bool>(sdk::get_thread_context(), original_camera);
        set_vertical_enable_method->call<void>(sdk::get_thread_context(), new_camera, old_vertical_enable);
    }

    if (get_aspect_ratio_method != nullptr && set_aspect_ratio_method != nullptr) {
        const auto old_aspect_ratio = get_aspect_ratio_method->call<float>(sdk::get_thread_context(), original_camera);
        set_aspect_ratio_method->call<void>(sdk::get_thread_context(), new_camera, old_aspect_ratio);
    }

    for (const auto& descriptor : m_wanted_components) {
        if (!descriptor.allowed) {
            continue;
        }

        const auto old_component = utility::re_component::find<REComponent>(old_camera_gameobject->transform, descriptor.name);
        const auto new_component = utility::re_component::find<REComponent>(new_camera_gameobject->transform, descriptor.name);

        if (old_component == nullptr || new_component == nullptr) {
            continue;
        }

        const auto t1 = utility::re_managed_object::get_type_definition(old_component);
        const auto t2 = utility::re_managed_object::get_type_definition(new_component);

        const auto t_name_fnv = utility::hash(t1->get_full_name());

        if (!m_getter_setters.contains(t1)) {
            const auto methods = t1->get_methods();

            for (auto& method : methods) {
                const auto name = std::string{method.get_name()};

                if (name.find("get_") == 0) {
                    spdlog::info("Found getter {}", name);

                    const auto name_front = name.substr(4);
                    m_getter_setters[t1][name_front].getter = &method;
                } else if (name.find("set_") == 0) {
                    spdlog::info("Found setter {}", name);

                    const auto name_front = name.substr(4);
                    m_getter_setters[t1][name_front].setter = &method;
                }
            }

            // Add property jobs, the copying will be spread out over multiple frames as it is a lot of work
            // Call the various getters and setters
            /*
            for (const auto& [name, methods] : m_getter_setters[t1]) {
                if (methods.getter == nullptr || methods.setter == nullptr) {
                    continue;
                }

                const auto result_type = methods.getter->get_return_type();
                const auto should_pass_result_ptr = result_type != nullptr && result_type->is_value_type() && (result_type->get_valuetype_size() > sizeof(void*) || (!result_type->is_primitive() && !result_type->is_enum()));
                
                m_property_jobs.push_back([allowed = &descriptor.allowed, old_component, new_component, getter = methods.getter, setter = methods.setter, result_type, should_pass_result_ptr]() {
                    if (!*allowed) {
                        return;
                    }
                    
                    const auto result = getter->invoke(old_component, {});

                    if (result_type == nullptr) {
                        setter->invoke(new_component, {result.ptr});
                    } else {
                        if (should_pass_result_ptr) {
                            setter->invoke(new_component, {(void*)result.bytes.data()});
                        } else {
                            const auto current_value = getter->invoke(new_component, {});

                            if (current_value.ptr != result.ptr) {
                                setter->invoke(new_component, {result.ptr});
                            }
                        }
                    }
                });
            }*/
        }

        // Call the various getters and setters
        for (const auto& [name, methods] : m_getter_setters[t1]) {
            if (methods.getter != nullptr && methods.setter != nullptr) {
                const auto result = methods.getter->invoke(old_component, {});

                if (result.ptr != nullptr) {
                    const auto result_type = methods.getter->get_return_type();

                    if (result_type == nullptr) {
                        methods.setter->invoke(new_component, {result.ptr});
                    } else {
                        const auto should_pass_result_ptr =
                            result_type->is_value_type() && result_type->get_valuetype_size() > sizeof(void*);

                        if (should_pass_result_ptr) {
                            methods.setter->invoke(new_component, {(void*)result.bytes.data()});
                        } else {
                            methods.setter->invoke(new_component, {result.ptr});
                        }
                    }
                }
            }
        }

        auto ctx = sdk::get_thread_context();

        // Properties that need to be manually set
        if (t_name_fnv == "via.render.LDRPostProcess"_fnv) {
            static auto t = t1;
            static auto get_color_correct = t->get_method("get_ColorCorrect");

            if (get_color_correct != nullptr) {
                const auto color_correct_old = get_color_correct->call<::REManagedObject*>(ctx, old_component);
                const auto color_correct_new = get_color_correct->call<::REManagedObject*>(ctx, new_component);

                if (color_correct_old != nullptr && color_correct_new != nullptr) {
                    static auto color_t = utility::re_managed_object::get_type_definition(color_correct_old);
                    static auto get_enabled = color_t->get_method("get_Enabled");

                    if (get_enabled != nullptr) {
                        const auto enabled_old = get_enabled->call<bool>(ctx, color_correct_old);
                        const auto enabled_new = get_enabled->call<bool>(ctx, color_correct_new);

                        if (enabled_old != enabled_new) {
                            static auto set_enabled = color_t->get_method("set_Enabled");
                            set_enabled->call(ctx, color_correct_new, enabled_old);
                        }
                    }
                }
            }
        } else if (t_name_fnv == "via.render.SoftBloom"_fnv) {
            static auto t = t1;
            static auto get_algorithm = t->get_method("getAlgorithm");
            static auto set_algorithm = t->get_method("setAlgorithm");

            if (get_algorithm != nullptr && set_algorithm != nullptr) {
                const auto algorithm_old = get_algorithm->call<uint32_t>(ctx, old_component);
                const auto algorithm_new = get_algorithm->call<uint32_t>(ctx, new_component);

                if (algorithm_old != algorithm_new) {
                    set_algorithm->call(ctx, new_component, algorithm_old);
                }
            }

            static auto get_dirt_mask_blend = t->get_method("getDirtMaskBlend");
            static auto set_dirt_mask_blend = t->get_method("setDirtMaskBlend");

            if (get_dirt_mask_blend != nullptr && set_dirt_mask_blend != nullptr) {
                const auto dirt_mask_old = get_dirt_mask_blend->call<void*>(ctx, old_component);
                const auto dirt_mask_new = get_dirt_mask_blend->call<void*>(ctx, new_component);

                if (dirt_mask_old != dirt_mask_new) {
                    set_dirt_mask_blend->call(ctx, new_component, dirt_mask_old);
                }
            }

            static auto get_size_scale_count = t->get_method("getSizeScaleCount");
            static auto set_size_scale_count = t->get_method("setSizeScaleCount");

            if (get_size_scale_count != nullptr && set_size_scale_count != nullptr) {
                const auto size_scale_count_old = get_size_scale_count->call<int>(sdk::get_thread_context(), old_component);
                const auto size_scale_count_new = get_size_scale_count->call<int>(sdk::get_thread_context(), new_component);

                if (size_scale_count_old != size_scale_count_new) {
                    set_size_scale_count->call(ctx, new_component, size_scale_count_old);

                    static auto get_size_scale = t->get_method("getSizeScale");
                    static auto set_size_scale = t->get_method("setSizeScale");

                    if (get_size_scale != nullptr && set_size_scale != nullptr) {
                        for (auto i = 0; i < size_scale_count_old; i++) {
                            const auto size_scale_old = get_size_scale->call<float>(ctx, old_component, i);
                            const auto size_scale_new = get_size_scale->call<float>(ctx, new_component, i);

                            if (size_scale_old != size_scale_new) {
                                set_size_scale->call(ctx, new_component, i, size_scale_old);
                            }
                        }
                    }
                }
            }
        } else if (t_name_fnv == "via.render.ToneMapping"_fnv) {
            static auto t = t1;
            static auto get_vignetting = t->get_method("getVignetting");
            static auto set_vignetting = t->get_method("setVignetting");

            if (get_vignetting != nullptr && set_vignetting != nullptr) {
                const auto vignetting_old = get_vignetting->call<uint32_t>(ctx, old_component);
                const auto vignetting_new = get_vignetting->call<uint32_t>(ctx, new_component);
                const auto is_disable_vignette = Camera::get()->is_disable_vignette();

                if (!is_disable_vignette) {
                    if (vignetting_old != vignetting_new) {
                        set_vignetting->call(ctx, new_component, vignetting_old);
                    }
                } else {
                    set_vignetting->call(ctx, new_component, via::render::ToneMapping::Vignetting::Disable);
                }
            }

            static auto get_auto_exposure = t->get_method("getAutoExposure");
            static auto set_auto_exposure = t->get_method("setAutoExposure");

            if (get_auto_exposure != nullptr && set_auto_exposure != nullptr) {
                const auto auto_exposure_old = get_auto_exposure->call<uint32_t>(ctx, old_component);
                const auto auto_exposure_new = get_auto_exposure->call<uint32_t>(ctx, new_component);

                if (auto_exposure_old != auto_exposure_new) {
                    set_auto_exposure->call(ctx, new_component, auto_exposure_old);
                }
            }

            static auto get_neighborhood_clamp = t->get_method("getNeighborhoodClamp");
            static auto set_neighborhood_clamp = t->get_method("setNeighborhoodClamp");

            if (get_neighborhood_clamp != nullptr && set_neighborhood_clamp != nullptr) {
                const auto neighborhood_clamp_old = get_neighborhood_clamp->call<uint32_t>(ctx, old_component);
                const auto neighborhood_clamp_new = get_neighborhood_clamp->call<uint32_t>(ctx, new_component);

                if (neighborhood_clamp_old != neighborhood_clamp_new) {
                    set_neighborhood_clamp->call(ctx, new_component, neighborhood_clamp_old);
                }
            }

            static auto get_temporal_aa = t->get_method("getTemporalAA");
            static auto set_temporal_aa = t->get_method("setTemporalAA");

            if (get_temporal_aa != nullptr && set_temporal_aa != nullptr) {
                const auto temporal_aa_old = get_temporal_aa->call<uint32_t>(ctx, old_component);
                const auto temporal_aa_new = get_temporal_aa->call<uint32_t>(ctx, new_component);

                if (temporal_aa_old != temporal_aa_new) {
                    set_temporal_aa->call(ctx, new_component, temporal_aa_old);
                }
            }

            static auto get_temporal_aa_algorithm =  t->get_method("getTemporalAAAlgorithm");
            static auto set_temporal_aa_algorithm =  t->get_method("setTemporalAAAlgorithm");

            if (get_temporal_aa_algorithm != nullptr && set_temporal_aa_algorithm != nullptr) {
                const auto temporal_aa_algorithm_old = get_temporal_aa_algorithm->call<uint32_t>(ctx, old_component);
                const auto temporal_aa_algorithm_new = get_temporal_aa_algorithm->call<uint32_t>(ctx, new_component);

                if (temporal_aa_algorithm_old != temporal_aa_algorithm_new) {
                    set_temporal_aa_algorithm->call(ctx, new_component, temporal_aa_algorithm_old);
                }
            }
        }
    }

    // Run the jobs in segments
    // m_property_jobs_index should increase
    constexpr size_t jobs_per_segment = 25;
    const auto jobs_to_run = std::min(jobs_per_segment, m_property_jobs.size() - m_property_jobs_index);

    for (auto i = 0; i < jobs_to_run; i++) {
        m_property_jobs[m_property_jobs_index + i]();
    }

    m_property_jobs_index += jobs_to_run;

    if (m_property_jobs_index >= m_property_jobs.size()) {
        m_property_jobs_index = 0;
    }
#endif
}

sdk::SystemArray* CameraDuplicator::get_all_cameras() {
    const auto scene = sdk::get_current_scene();

    if (scene == nullptr) {
        return nullptr;
    }

    const auto camera_typeof = sdk::find_type_definition("via.Camera")->get_runtime_type();
    const auto existing_cameras = sdk::call_object_func_easy<sdk::SystemArray*>(scene, "findComponents(System.Type)", camera_typeof);

    if (existing_cameras == nullptr) {
        return nullptr;
    }

    return existing_cameras;
}
}
