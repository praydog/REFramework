#include "reframework/API.hpp"
#include "sdk/SceneManager.hpp"

#include "../VR.hpp"
#include "../Camera.hpp"

#include "HookManager.hpp"
#include "CameraDuplicator.hpp"

namespace vrmod {
constexpr auto PRIORITY_OFFSET = 0x48;

void CameraDuplicator::on_pre_application_entry(void* entry, const char* name, size_t hash) {
    {
        std::scoped_lock _{ m_camera_mutex };
        std::vector<REComponent*> cameras_to_remove{};
        
        for (auto camera : m_new_cameras) {
            //*(int32_t*)((uint8_t*)camera + PRIORITY_OFFSET) = *(int32_t*)((uint8_t*)get_old_camera_counterpart(camera) + PRIORITY_OFFSET) + 1; // causes the game to NOT use the new camera as the main camera
            *(int32_t*)((uint8_t*)camera + PRIORITY_OFFSET) = -1;

            if (camera->referenceCount == 1) {
                cameras_to_remove.push_back(camera);
                spdlog::info("Removing camera {:x}", (uintptr_t)camera);
            } else {
                static const auto game_object_t = sdk::find_type_definition("via.GameObject");
                static const auto get_Valid = game_object_t != nullptr ? game_object_t->get_method("get_Valid") : nullptr;
                auto owner = utility::re_component::get_game_object(camera);

                // if the owning game object was destroyed then well... destroy the camera
                if (owner == nullptr || (get_Valid != nullptr && !get_Valid->call<bool>(sdk::get_thread_context(), owner))) {
                    cameras_to_remove.push_back(camera);
                    spdlog::info("Removing camera (destroyed gameobject) {:x}", (uintptr_t)camera);
                }
            }
        }

        for (auto camera : cameras_to_remove) {
            m_new_cameras.erase((RECamera*)camera);
            m_old_cameras.erase((RECamera*)camera);
            m_seen_cameras.erase((RECamera*)camera);
            m_old_to_new_camera.erase((RECamera*)camera);
            m_new_to_old_camera.erase((RECamera*)camera);

            // Remove anything from the map that has the value of the camera, not just the key.
            std::erase_if(m_old_to_new_camera, [camera](const auto& pair) {
                return pair.second == camera;
            });

            std::erase_if(m_new_to_old_camera, [camera](const auto& pair) {
                return pair.second == camera;
            });

            utility::re_managed_object::release(camera);
        }
    }

    if (hash == "LockScene"_fnv) {
        if (!VR::get()->is_hmd_active() || VR::get()->get_rendering_technique() != VR::RenderingTechnique::MULTIPASS) {
            std::scoped_lock _{ m_camera_mutex };
            for (auto camera : m_new_cameras) {
                auto game_object = utility::re_component::get_game_object((REComponent*)camera);

                if (game_object != nullptr) {
                    game_object->shouldDraw = false;
                    game_object->shouldUpdate = false;
                }
            }

            return;
        }

        hook_get_primary_camera();
        clone_camera();
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

        copy_camera_properties();
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

std::vector<sdk::renderer::layer::Scene*> CameraDuplicator::get_relevant_scene_layers() {
    const auto output_layer = sdk::renderer::get_output_layer();

    if (output_layer == nullptr) {
        return {};
    }

    auto scene_layers = output_layer->find_fully_rendered_scene_layers();

    if (scene_layers.size() < 2) {
        return scene_layers;
    }

    const auto primary_camera = sdk::get_primary_camera();

    if (primary_camera == nullptr) {
        return scene_layers;
    }

    const auto secondary_camera = get_new_camera_counterpart(primary_camera);

    if (secondary_camera == nullptr) {
        return scene_layers;
    }

    for (auto i = 0; i < scene_layers.size(); i++) {
        const auto current_camera = scene_layers[i]->get_camera();

        if (current_camera == primary_camera) {
            if (i != 0) {
                std::swap(scene_layers[0], scene_layers[i]);
            }

            continue;
        }

        if (current_camera == secondary_camera) {
            if (i != 1) {
                std::swap(scene_layers[1], scene_layers[i]);
            }

            continue;
        }
    }

    return scene_layers;
}

void CameraDuplicator::clone_camera() {
    // Untested on older than TDB 69 (RE8)
#if TDB_VER >= 69
    if (m_waiting_for_identify) {
        return;
    }

    size_t num_main_cameras = 0;

    const auto main_camera = sdk::get_primary_camera();

    if (main_camera == nullptr) {
        return;
    }

    std::scoped_lock _{ m_camera_mutex };
    
    const auto seen_contains = m_seen_cameras.contains(main_camera);
    const auto old_to_new_contains = m_old_to_new_camera.contains(main_camera);

    if (seen_contains && old_to_new_contains) {
        return;
    }

    if (seen_contains && !old_to_new_contains) {
        spdlog::info("Existing camera had a clone destroyed, re-cloning it");
    }

    const auto camera_gameobject = utility::re_component::get_game_object((REComponent*)main_camera);

    if (camera_gameobject == nullptr) {
        return;
    }

    const auto current_main_camera_name = utility::re_string::get_view(camera_gameobject->name);

    const auto existing_cameras = get_all_cameras();

    if (existing_cameras == nullptr) {
        return;
    }

    spdlog::info("Found {} existing cameras", existing_cameras->size());
    spdlog::info("Cloning {} @ 0x{:x}", utility::narrow(current_main_camera_name), (uintptr_t)main_camera);

    for (::REManagedObject* camera : *existing_cameras) {
        if (camera == nullptr) {
            continue;
        }

        const auto gameobject = utility::re_component::get_game_object((REComponent*)camera);

        if (gameobject == nullptr) {
            continue;
        }

        if (!m_seen_cameras.contains((RECamera*)camera)) {
            utility::re_managed_object::add_ref(camera);
            m_seen_cameras.insert((RECamera*)camera);
            m_old_cameras.insert((RECamera*)camera);
        }

        const auto name = utility::re_string::get_view(gameobject->name);

        if (name == current_main_camera_name) {
            ++num_main_cameras;
        }
    }

    spdlog::info("Found {} main cameras", num_main_cameras);

    const auto old_camera_gameobject = utility::re_component::get_game_object((REComponent*)main_camera);

    if (old_camera_gameobject == nullptr) {
        return;
    }

    // Setting active to false lets us "activate" the folder again, cloning the camera perfectly
    // ADDENDUM: We don't do this anymore, and instead opt for manually recreating the components
    // as it causes a bunch of issues like the game thinks the new camera is the main one and other things.
    //constexpr auto ACTIVE_OFFSET = sizeof(::REManagedObject) + 5;
    //*(bool*)((uint8_t*)old_camera_folder + ACTIVE_OFFSET) = false;
    //sdk::call_object_func_easy<void*>(old_camera_folder, "activate");

    // Manually create the game object and duplicate the components
    static const auto gameobject_t = sdk::find_type_definition("via.GameObject");
    static const auto create_gameobject_fn = gameobject_t->get_method("create(System.String)");
    static const auto create_component_fn = gameobject_t->get_method("createComponent(System.Type)");

    // Create new gameobject
    const auto adjusted_name = std::wstring{current_main_camera_name} + L" (Clone)";
    const auto new_camera_gameobject = (::REGameObject*)create_gameobject_fn->call<sdk::ManagedObject*>(sdk::get_thread_context(), sdk::VM::create_managed_string(adjusted_name));
    new_camera_gameobject->shouldDraw = false;
    new_camera_gameobject->shouldUpdate = false;
    //utility::re_managed_object::add_ref(new_camera_gameobject);

    spdlog::info("Created new gameobject @ 0x{:x}", (uintptr_t)new_camera_gameobject);

    // Parent new gameobject to old gameobject
    const auto new_transform = new_camera_gameobject->transform;
    const auto old_transform = old_camera_gameobject->transform;

    if (new_transform != nullptr && old_transform != nullptr) {
        sdk::call_object_func_easy<void*>(new_transform, "setParent(via.Transform, System.Boolean)", old_transform, false);
        spdlog::info("Parented new camera to old camera");
    }

    REComponent* component = old_camera_gameobject->transform->childComponent;

    std::unordered_set<std::string> illegal_components {
        "via.render.ExperimentalRayTrace",
        "via.motion.MotionCamera", // Unnecessarily takes control of the camera transform which we don't want
        "via.motion.ActorMotionCamera", // Unnecessarily takes control of the camera transform which we don't want
        "via.wwise.WwiseListener",
        "via.physics.Colliders",
        // todo
    };

    std::unordered_set<std::string> game_framework_components {
        "worldtour.bWTCameraController", // this causes the camera to become the "main" camera which is NOT what we want, it breaks the real main camera
        "camera.MainCameraController", // also breaks the real main camera
        "camera.CameraSystem", // also breaks the real main camera
        "bPostProcessController",
        "MainCameraDestroyChecker",
    };

    // Add new components to illegal_components with the game project name prepended to the game_framework_components
    for (auto& fwcomp : game_framework_components) {
        illegal_components.insert(game_namespace((fwcomp)));
    }

    while (component != nullptr) {
        const auto tdef = utility::re_managed_object::get_type_definition(component);
        const auto runtime_type = tdef->get_runtime_type();

        if (illegal_components.contains(tdef->get_full_name())) {
            spdlog::info("Skipping illegal component {}", tdef->get_full_name());
            component = component->childComponent;
            continue;
        }

        const auto full_name = tdef->get_full_name();

        // TODO: other game framework prefixes or maybe app is just okay?
        // snow is for MHRise, app is for most of the other games
        if (full_name.starts_with("app.") || full_name.starts_with("snow.")) {
            spdlog::info("Skipping app component {}", tdef->get_full_name());
            component = component->childComponent;
            continue;
        }

        const auto new_comp = create_component_fn->call<sdk::ManagedObject*>(sdk::get_thread_context(), new_camera_gameobject, runtime_type);

        if (tdef->get_full_name() == "via.render.RenderOutput") {
            sdk::call_object_func_easy<void*>(new_comp, "set_RenderOutputID(via.render.RenderOutputID)", 3);
        }

        if (tdef->get_full_name() == "via.Camera") {
            m_seen_cameras.insert((RECamera*)new_comp);
            m_new_cameras.insert((RECamera*)new_comp);
            m_old_to_new_camera[(RECamera*)component] = (RECamera*)new_comp;
            m_new_to_old_camera[(RECamera*)new_comp] = (RECamera*)component;
            //*(int32_t*)((uint8_t*)new_comp + PRIORITY_OFFSET) = *(int32_t*)((uint8_t*)get_old_camera_counterpart((RECamera*)new_comp) + PRIORITY_OFFSET) + 1; // causes the game to NOT use the new camera as the main camera
            *(int32_t*)((uint8_t*)new_comp + PRIORITY_OFFSET) = -1;

            utility::re_managed_object::add_ref(new_comp);
        }

        spdlog::info("Created new component {} @ 0x{:x}", tdef->get_full_name(), (uintptr_t)new_comp);

        component = component->childComponent;
    }

    new_camera_gameobject->shouldDraw = true; // YES draw by default, this is the counterpart to the main camera

    m_called_activate = true;
    m_identified_new_cameras = true;
    /*m_identified_new_cameras = false;
    m_waiting_for_identify = true;

    find_new_cameras();*/
#endif
}

void CameraDuplicator::hook_get_primary_camera() {
    if (m_hooked_get_primary_camera) {
        return;
    }

    m_hooked_get_primary_camera = true;

    const auto sceneview_t = sdk::find_type_definition("via.SceneView");
    const auto get_primary_camera_fn = sceneview_t->get_method("get_PrimaryCamera");
    spdlog::info("Hooking getPrimaryCamera: {:x}", (uintptr_t)get_primary_camera_fn);

    g_hookman.add(get_primary_camera_fn, [this](std::vector<uintptr_t>& args, std::vector<sdk::RETypeDefinition*>& arg_tys, uintptr_t ret_addr) {
        if (m_new_cameras.empty()) {
            return HookManager::PreHookResult::CALL_ORIGINAL;
        }

        std::scoped_lock _{ m_camera_mutex };

        for (auto new_camera : m_new_cameras) {
            //*(int32_t*)((uint8_t*)new_camera + PRIORITY_OFFSET) = *(int32_t*)((uint8_t*)get_old_camera_counterpart(new_camera) + PRIORITY_OFFSET) + 1; // causes the game to NOT use the new camera as the main camera
            *(int32_t*)((uint8_t*)new_camera + PRIORITY_OFFSET) = -1;
            *(int32_t*)((uint8_t*)new_camera + PRIORITY_OFFSET + 8) = (int32_t)via::CameraType::Debug;
        }
        
        return HookManager::PreHookResult::CALL_ORIGINAL;
    }, 
    [this](uintptr_t& ret_val, sdk::RETypeDefinition* ret_ty, uintptr_t ret_addr) {
        std::scoped_lock _{ m_camera_mutex };

        for (auto new_camera : m_new_cameras) {
            *(int32_t*)((uint8_t*)new_camera + PRIORITY_OFFSET + 8) = (int32_t)via::CameraType::Game;
        }
    });
}


void CameraDuplicator::copy_camera_properties() {
#if TDB_VER >= 69
    if (!m_copy_camera) {
        return;
    }

    const auto old_camera = sdk::get_primary_camera();

    if (old_camera == nullptr) {
        return;
    }

    std::scoped_lock _{ m_camera_mutex };

    if (!m_old_to_new_camera.contains((RECamera*)old_camera)) {
        return;
    }

    auto new_camera = m_old_to_new_camera[(RECamera*)old_camera];

    if (new_camera == nullptr) {
        return;
    }

    const auto old_camera_gameobject = utility::re_component::get_game_object((REComponent*)old_camera);
    const auto new_camera_gameobject = utility::re_component::get_game_object((REComponent*)new_camera);

    if (old_camera_gameobject == nullptr || new_camera_gameobject == nullptr) {
        return;
    }

    for (auto camera : m_new_cameras) {
        const auto camera_gameobject = utility::re_component::get_game_object((REComponent*)camera);

        if (camera_gameobject == nullptr) {
            continue;
        }

        camera_gameobject->shouldDraw = camera_gameobject == new_camera_gameobject;
        camera_gameobject->shouldUpdate = false;
    }

    // Do not allow the camera components to update. We will do the update ourselves via the copying of properties
    new_camera_gameobject->shouldUpdate = false;

    // If a new camera is created, we need to reset the property jobs as components may be different
    if ((RECamera*)old_camera != m_last_primary_camera) {
        m_getter_setters.clear();
        m_property_jobs.clear();
        m_property_jobs_index = 0;
    }

    m_last_primary_camera = (RECamera*)old_camera;

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
        const auto old_near_clip_plane = get_near_clip_plane_method->call<float>(sdk::get_thread_context(), old_camera);
        set_near_clip_plane_method->call<void>(sdk::get_thread_context(), new_camera, old_near_clip_plane);
    }

    if (get_far_clip_plane_method != nullptr && set_far_clip_plane_method != nullptr) {
        const auto old_far_clip_plane = get_far_clip_plane_method->call<float>(sdk::get_thread_context(), old_camera);
        set_far_clip_plane_method->call<void>(sdk::get_thread_context(), new_camera, old_far_clip_plane);
    }

    if (get_fov_method != nullptr && set_fov_method != nullptr) {
        const auto old_fov = get_fov_method->call<float>(sdk::get_thread_context(), old_camera);
        set_fov_method->call<void>(sdk::get_thread_context(), new_camera, old_fov);
    }

    if (get_vertical_enable_method != nullptr && set_vertical_enable_method != nullptr) {
        const auto old_vertical_enable = get_vertical_enable_method->call<bool>(sdk::get_thread_context(), old_camera);
        set_vertical_enable_method->call<void>(sdk::get_thread_context(), new_camera, old_vertical_enable);
    }

    if (get_aspect_ratio_method != nullptr && set_aspect_ratio_method != nullptr) {
        const auto old_aspect_ratio = get_aspect_ratio_method->call<float>(sdk::get_thread_context(), old_camera);
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
                        setter->invoke(new_component, result.ptr);
                    } else {
                        if (should_pass_result_ptr) {
                            setter->invoke(new_component, (void*)result.bytes.data());
                        } else {
                            const auto current_value = getter->invoke(new_component, {});

                            if (current_value.ptr != result.ptr) {
                                setter->invoke(new_component, result.ptr);
                            }
                        }
                    }
                });
            }
        }

        // Call the various getters and setters
        /*for (const auto& [name, methods] : m_getter_setters[t1]) {
            if (methods.getter != nullptr && methods.setter != nullptr) {
                const auto result = methods.getter->invoke(old_component, {});

                if (result.ptr != nullptr) {
                    const auto result_type = methods.getter->get_return_type();

                    if (result_type == nullptr) {
                        methods.setter->invoke(new_component, {result.ptr});
                    } else {
                        const auto should_pass_result_ptr = result_type->is_value_type() && result_type->get_valuetype_size() > sizeof(void*);

                        if (should_pass_result_ptr) {
                            methods.setter->invoke(new_component, {(void*)result.bytes.data()});
                        } else {
                            methods.setter->invoke(new_component, {result.ptr});
                        }
                    }
                }
            }
        }*/

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
