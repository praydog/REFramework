#include <optional>
#include <tuple>

#include <imgui.h>
#include <ImGuizmo.h>

#include "REFramework.hpp"
#include "utility/ImGui.hpp"
#include "sdk/SceneManager.hpp"
#include "sdk/RETypeDB.hpp"
#include "sdk/REManagedObject.hpp"
#include "sdk/Renderer.hpp"
#include "sdk/REGameObject.hpp"

#if TDB_VER < 69
#include "sdk/regenny/re3/via/motion/Chain.hpp"
#include "sdk/regenny/re3/via/motion/ChainCollisions.hpp"
#elif TDB_VER == 69
#include "sdk/regenny/re8/via/motion/Chain.hpp"
#include "sdk/regenny/re8/via/motion/ChainCollisionTop.hpp"
#include "sdk/regenny/re8/via/motion/ChainCollisions.hpp"
#elif TDB_VER >= 70
#if defined(MHRISE)
#if TDB_VER == 70
#define MHRISE_CHAIN70
#include "sdk/regenny/mhrise/via/motion/Chain.hpp"
#include "sdk/regenny/mhrise/via/motion/ChainCollisions.hpp"
#else
#include "sdk/regenny/mhrise_tdb71/via/motion/Chain.hpp"
#include "sdk/regenny/mhrise_tdb71/via/motion/ChainCollisions.hpp"
#include "sdk/regenny/mhrise_tdb71/via/motion/ChainCollisionTop.hpp"
#endif
#else

#if defined(RE4)
#include "sdk/regenny/re4/via/motion/Chain.hpp"
#include "sdk/regenny/re4/via/motion/ChainCollisions.hpp"
#include "sdk/regenny/re4/via/motion/ChainCollisionTop.hpp"
#elif defined (SF6)
#include "sdk/regenny/sf6/via/motion/Chain.hpp"
#include "sdk/regenny/sf6/via/motion/ChainCollisions.hpp"
#include "sdk/regenny/sf6/via/motion/ChainCollisionTop.hpp"
#else
#include "sdk/regenny/re2_tdb70/via/motion/Chain.hpp"
#include "sdk/regenny/re2_tdb70/via/motion/ChainCollisionTop.hpp"
#include "sdk/regenny/re2_tdb70/via/motion/ChainCollisions.hpp"
#endif

#endif

#endif

#include "ObjectExplorer.hpp"
#include "ChainViewer.hpp"

std::optional<std::string> ChainViewer::on_initialize() {

    // OK
    return Mod::on_initialize();
}

void ChainViewer::on_config_load(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }
}

void ChainViewer::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}


void ChainViewer::on_draw_dev_ui() {
    ImGui::SetNextItemOpen(false, ImGuiCond_::ImGuiCond_Once);

    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    if (m_enabled->draw("Enabled") && !m_enabled->value()) {
        // todo
    }
}

void ChainViewer::on_frame() {
    if (!m_enabled->value()) {
        return;
    }

    auto scene = sdk::get_current_scene();

    if (scene == nullptr) {
        return;
    }

    auto context = sdk::get_thread_context();

    static auto scene_def = sdk::find_type_definition("via.Scene");
    auto first_transform = sdk::call_native_func_easy<RETransform*>(scene, scene_def, "get_FirstTransform");

    if (first_transform == nullptr) {
        return;
    }

    m_delta_time.update();

    static auto transform_def = utility::re_managed_object::get_type_definition(first_transform);
    static auto folder_def = sdk::find_type_definition("via.Folder");
    static auto gameobject_def = sdk::find_type_definition("via.GameObject");
    static auto next_transform_method = transform_def->get_method("get_Next");
    static auto child_transform_method = transform_def->get_method("get_Child");
    static auto get_gameobject_method = transform_def->get_method("get_GameObject");
    static auto get_folder_path_method = folder_def->get_method("get_Path");
    static auto get_folder_method = gameobject_def->get_method("get_Folder");

    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    auto camera_gameobject = get_gameobject_method->call<::REGameObject*>(sdk::get_thread_context(), camera);

    if (camera_gameobject == nullptr) {
        return;
    }

    auto camera_transform = camera_gameobject->transform;

    if (camera_transform == nullptr) {
        return;
    }

    auto camera_joints = sdk::call_object_func_easy<sdk::SystemArray*>(camera_transform, "get_Joints");

    if (camera_joints == nullptr) {
        return;
    }

    auto camera_joint = (::REJoint*)camera_joints->get_element(0);

    if (camera_joint == nullptr) {
        return;
    }

    Matrix4x4f proj{}, view{};

    const auto camera_origin = sdk::get_transform_position(camera_transform);

    sdk::call_object_func<void*>(camera, "get_ProjectionMatrix", &proj, context, camera);
    sdk::call_object_func<void*>(camera, "get_ViewMatrix", &view, context, camera);

    /*view = view * Matrix4x4f {
        -1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, -1, 0,
        0, 0, 0, 1
    };*/

    IMGUIZMO_NAMESPACE::SetImGuiContext(ImGui::GetCurrentContext());
    IMGUIZMO_NAMESPACE::SetDrawlist(ImGui::GetBackgroundDrawList());
    IMGUIZMO_NAMESPACE::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

    ImGui::Begin("Chains");

    static auto chain_type = sdk::find_type_definition("via.motion.Chain");
    static auto chain_runtime_type = chain_type->get_runtime_type();
    static auto chain_re_type = chain_type->get_type();

    auto attempt_display_chains = [&](RETransform* transform) {
        auto chain = utility::re_component::find<regenny::via::motion::Chain>(transform, chain_re_type);
        bool made = false;

        if (chain == nullptr) {
            return;
        }

        auto owner = get_gameobject_method->call<REGameObject*>(context, transform);

        if (owner == nullptr) {
            return;
        }

        auto owner_name = utility::re_game_object::get_name(owner);

        if (owner_name.empty()) {
            owner_name = "";
        }

        ImGui::PushID(chain);
        made = ImGui::TreeNode(chain, owner_name.data());

        const auto is_hovering_node = ImGui::IsItemHovered();
        auto col = ImVec4(66.0f / 255.0f, 105.0f / 255.0f, 245.0f / 255.0f, 0.25f);

        // If we're hovering over the node, highlight it red and pulsate the alpha value
        if (is_hovering_node) {
            m_pulse_time += m_delta_time;

            col.w = glm::abs(glm::cos(m_pulse_time * glm::pi<float>()));
        }

        const auto owner_folder = get_folder_method->call<::REManagedObject*>(context, owner);

        if (owner_folder != nullptr) {
            const auto folder_path = get_folder_path_method->call<::SystemString*>(context, owner_folder);

            if (folder_path != nullptr) {       
                ImGui::SameLine();
                ImGui::TextColored(ImVec4{100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 255 / 255.0f}, " [%s]", utility::re_string::get_string(folder_path).data());
            }
        }

        if (made) {
            ObjectExplorer::get()->handle_address(chain);
        }

        if (chain != nullptr && chain->CollisionData.num > 0 && chain->CollisionData.collisions != nullptr) {
            for (auto i = 0; i < chain->CollisionData.num; ++i) {
                #if TDB_VER >= 69 && !defined(MHRISE_CHAIN70)
                const auto& collider_top = chain->CollisionData.collisions[i];

                for (auto j = 0; j < collider_top.num_collisions; ++j) {
                    auto& collider = collider_top.collisions[j];
                #else
                    auto& collider = chain->CollisionData.collisions[i];
                #endif
                    auto adjusted_pos1 = collider.pair_joint == nullptr ? *(Vector3f*)&collider.sphere.pos : *(Vector3f*)&collider.capsule.p0;
                    auto adjusted_pos2 = collider.pair_joint == nullptr ? Vector3f{} : *(Vector3f*)&collider.capsule.p1;

                    const auto joint_pos = collider.joint != nullptr ? (Vector3f)sdk::get_joint_position((::REJoint*)collider.joint) : Vector3f{};
                    const auto joint_rot = collider.joint != nullptr ? sdk::get_joint_rotation((::REJoint*)collider.joint) : glm::identity<glm::quat>();
                    const auto pair_joint_pos = collider.pair_joint != nullptr ? (Vector3f)sdk::get_joint_position((::REJoint*)collider.pair_joint) : Vector3f{};
                    const auto predicted_pos = joint_pos + (joint_rot * *(Vector3f*)&collider.offset);
                    const auto offset_length = glm::length(*(Vector3f*)&collider.offset);

                    if (offset_length != 0.0f && collider.joint != nullptr && glm::length(predicted_pos - adjusted_pos1) >= (offset_length * 2.0f)) {
                        if (collider.pair_joint != nullptr) {
                            const auto rot = sdk::get_joint_rotation((::REJoint*)collider.joint);
                            const auto rot2 = sdk::get_joint_rotation((::REJoint*)collider.pair_joint);
                            adjusted_pos1 = (Vector3f)sdk::get_transform_position(sdk::get_joint_owner((::REJoint*)collider.joint)) + (*(Vector3f*)&collider.capsule.p0);
                            adjusted_pos2 = (Vector3f)sdk::get_transform_position(sdk::get_joint_owner((::REJoint*)collider.pair_joint)) + (*(Vector3f*)&collider.capsule.p1);
                        } else {
                            adjusted_pos1 = (Vector3f)sdk::get_transform_position(sdk::get_joint_owner((::REJoint*)collider.joint)) + (*(Vector3f*)&collider.sphere.pos);
                        }
                    }

                    if (!ImGui::GetIO().MouseDown[0]) {
                        ImGuizmo::Enable(false);
                        ImGuizmo::Enable(true);
                    }

                    const auto additional_rad = 2.0f;

                    // Draw spheres/capsules and imguizmo widgets
                    if (collider.pair_joint == nullptr) {
                        imgui::draw_sphere(adjusted_pos1, collider.sphere.r, ImGui::GetColorU32(col), true);

                        Matrix4x4f mat = glm::scale(Vector3f{collider.sphere.r, collider.sphere.r, collider.sphere.r});
                        mat[3] = Vector4f{adjusted_pos1, 1.0f};

                        const auto screen_pos1 = sdk::renderer::world_to_screen(adjusted_pos1);
                        const auto screen_pos1_top = sdk::renderer::world_to_screen(adjusted_pos1 + Vector3f{0.0f, collider.sphere.r, 0.0f});
                        const auto cursor_pos = *(Vector2f*)&ImGui::GetIO().MousePos;
                        const auto can_use1 = (screen_pos1 && screen_pos1_top && glm::length(cursor_pos - *screen_pos1) <= glm::abs(screen_pos1_top->y - screen_pos1->y) * additional_rad) || ImGuizmo::IsUsing();

                        using OP = ImGuizmo::OPERATION;

                        if (can_use1) {
                            ImGuizmo::SetID((intptr_t)&collider.sphere);
                            if (ImGuizmo::Manipulate((float*)&view, (float*)&proj, OP::TRANSLATE | OP::SCALEU, ImGuizmo::MODE::WORLD, (float*)&mat)) {
                                const auto delta = *(Vector3f*)&mat[3] - *(Vector3f*)&collider.sphere.pos;
                                *(Vector3f*)&collider.offset += glm::inverse(sdk::get_joint_rotation((::REJoint*)collider.joint)) * delta;
                                collider.radius += ((glm::length(mat[0]) + glm::length(mat[1]) + glm::length(mat[2])) / 3.0f) - collider.sphere.r;
                            }
                        }
                    } else {
                        // Capsule
                        imgui::draw_capsule(adjusted_pos1, adjusted_pos2, collider.capsule.r, ImGui::GetColorU32(col), true);

                        const auto screen_pos1 = sdk::renderer::world_to_screen(adjusted_pos1);
                        const auto screen_pos1_top = sdk::renderer::world_to_screen(adjusted_pos1 + Vector3f{0.0f, collider.capsule.r, 0.0f});
                        const auto cursor_pos = *(Vector2f*)&ImGui::GetIO().MousePos;
                        const auto can_use1 = (screen_pos1 && screen_pos1_top && glm::length(cursor_pos - *screen_pos1) <= glm::abs(screen_pos1_top->y - screen_pos1->y) * additional_rad) || ImGuizmo::IsUsing();

                        Matrix4x4f mat = glm::scale(Vector3f{collider.capsule.r, collider.capsule.r, collider.capsule.r});
                        using OP = ImGuizmo::OPERATION;

                        if (can_use1) {
                            mat[3] = Vector4f{adjusted_pos1, 1.0f};

                            ImGui::PushID(&collider.capsule.p0);
                            ImGuizmo::SetID((intptr_t)&collider.capsule.p0);
                            if (ImGuizmo::Manipulate((float*)&view, (float*)&proj, OP::TRANSLATE | OP::SCALEU, ImGuizmo::MODE::WORLD, (float*)&mat)) {
                                const auto delta = *(Vector3f*)&mat[3] - adjusted_pos1;
                                *(Vector3f*)&collider.offset += glm::inverse(sdk::get_joint_rotation((::REJoint*)collider.joint)) * delta;
                                //collider.radius = (glm::length(mat[0]) + glm::length(mat[1]) + glm::length(mat[2])) / 3.0f;
                                collider.radius += ((glm::length(mat[0]) + glm::length(mat[1]) + glm::length(mat[2])) / 3.0f) - collider.capsule.r;
                            }
                            ImGui::PopID();
                        }

                        const auto screen_pos2 = sdk::renderer::world_to_screen(adjusted_pos2);
                        const auto screen_pos2_top = sdk::renderer::world_to_screen(adjusted_pos2 + Vector3f{0.0f, collider.capsule.r, 0.0f});
                        const auto can_use2 = (screen_pos2 && screen_pos2_top && glm::length(cursor_pos - *screen_pos2) <= glm::abs(screen_pos2_top->y - screen_pos2->y) * additional_rad) || ImGuizmo::IsUsing();

                        if (can_use2) {
                            mat[3] = Vector4f{adjusted_pos2, 1.0f};

                            ImGui::PushID(&collider.capsule.p1);
                            ImGuizmo::SetID((intptr_t)&collider.capsule.p1);
                            if (ImGuizmo::Manipulate((float*)&view, (float*)&proj, OP::TRANSLATE | OP::SCALEU, ImGuizmo::MODE::WORLD, (float*)&mat)) {
                                const auto delta = *(Vector3f*)&mat[3] - adjusted_pos2;
                                *(Vector3f*)&collider.pair_offset += glm::inverse(sdk::get_joint_rotation((::REJoint*)collider.pair_joint)) * delta;
                                //collider.radius = (glm::length(mat[0]) + glm::length(mat[1]) + glm::length(mat[2])) / 3.0f;
                                collider.radius += ((glm::length(mat[0]) + glm::length(mat[1]) + glm::length(mat[2])) / 3.0f) - collider.capsule.r;
                            }
                            ImGui::PopID();
                        }
                    }

                    //world_to_screen_methods[1]->call<void*>(&screen_pos, context, &pos, &view, &proj, &screen_size);
                    //draw_list->AddText(ImVec2(screen_pos.x, screen_pos.y), ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), "Collision");

                    if (made) {
                        ImGui::PushID(&collider);
                        
                        ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);

                    #if TDB_VER >= 69 && !defined(MHRISE_CHAIN70)
                        if (ImGui::TreeNode(&collider, "Collision %d %d", i, j)) {
                    #else
                        if (ImGui::TreeNode(&collider, "Collision %d", i)) {
                    #endif
                            auto made_joint_node = ImGui::TreeNode(&collider.joint, "Joint");

                            const auto col = ImVec4{100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 255 / 255.0f};

                            if (collider.joint != nullptr) {
                                ImGui::SameLine();
                                ImGui::TextColored(col, "%s", sdk::get_joint_name((::REJoint*)collider.joint).c_str());
                            }

                            if (made_joint_node) {
                                ObjectExplorer::get()->handle_address(collider.joint);
                                ImGui::TreePop();
                            }

                            made_joint_node = ImGui::TreeNode(&collider.pair_joint, "Pair Joint");

                            if (collider.pair_joint != nullptr) {
                                ImGui::SameLine();
                                ImGui::TextColored(col, "%s", sdk::get_joint_name((::REJoint*)collider.pair_joint).c_str());
                            }

                            if (made_joint_node) {
                                ObjectExplorer::get()->handle_address(collider.pair_joint);
                                ImGui::TreePop();
                            }

                            ImGui::DragFloat("Radius", (float*)&collider.radius, 0.01f, 0.0f, 0.0f);
                            ImGui::DragInt("Flags", (int*)&collider.flags, 1, 0, 0);

                            ImGui::DragFloat3("Offset", (float*)&collider.offset, 0.01f, 0.0f, 0.0f);
                            ImGui::DragFloat3("Pair Offset", (float*)&collider.pair_offset, 0.01f, 0.0f, 0.0f);
                            ImGui::TreePop();
                        }

                        ImGui::PopID();
                    }
            #if TDB_VER >= 69 && !defined(MHRISE_CHAIN70)
                }
            #endif
            }
        }

        if (made) {
            ImGui::TreePop();
        }

        ImGui::PopID();
    };

    static auto scene_find_components_method = scene_def->get_method("findComponents(System.Type)");

    if (scene_find_components_method != nullptr && chain_runtime_type != nullptr) {
        const auto components = scene_find_components_method->call<sdk::SystemArray*>(context, scene, chain_runtime_type);
        
        if (components != nullptr && !components->empty()) {
            for (auto v : *components) {
                if (v == nullptr) {
                    continue;
                }

                const auto component = (::REComponent*)v;
                const auto owner = get_gameobject_method->call<::REGameObject*>(context, component);

                if (owner == nullptr) {
                    continue;
                }

                const auto transform = owner->transform;

                if (transform == nullptr) {
                    continue;
                }

                attempt_display_chains(transform);
            }
        }
    } else {
        for (auto transform = first_transform; 
            transform != nullptr; 
            transform = next_transform_method->call<RETransform*>(context, transform)) 
        {
            attempt_display_chains(transform);

            for (auto t = child_transform_method->call<RETransform*>(context, transform); t != nullptr; t = next_transform_method->call<RETransform*>(context, t)) {
                attempt_display_chains(t);

                for (auto t2 = child_transform_method->call<RETransform*>(context,t); t2 != nullptr; t2 = next_transform_method->call<RETransform*>(context, t2)) {
                    attempt_display_chains(t2);
                }
            }
        }
    }

    ImGui::End();
}

