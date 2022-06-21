#include <optional>
#include <tuple>

#include "REFramework.hpp"
#include "sdk/SceneManager.hpp"
#include "sdk/RETypeDB.hpp"
#include "sdk/REManagedObject.hpp"
#include "sdk/Renderer.hpp"

#if TDB_VER < 69
#include "sdk/regenny/re3/via/motion/Chain.hpp"
#include "sdk/regenny/re3/via/motion/ChainCollisions.hpp"
#elif TDB_VER == 69 || (TDB_VER == 70 && defined(MHRISE))
#include "sdk/regenny/re8/via/motion/Chain.hpp"
#include "sdk/regenny/re8/via/motion/ChainCollisionTop.hpp"
#include "sdk/regenny/re8/via/motion/ChainCollisions.hpp"
#else
#include "sdk/regenny/re2_tdb70/via/motion/Chain.hpp"
#include "sdk/regenny/re2_tdb70/via/motion/ChainCollisionTop.hpp"
#include "sdk/regenny/re2_tdb70/via/motion/ChainCollisions.hpp"
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
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_Once);

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

    static auto transform_def = utility::re_managed_object::get_type_definition(first_transform);
    static auto next_transform_method = transform_def->get_method("get_Next");
    static auto child_transform_method = transform_def->get_method("get_Child");
    static auto get_gameobject_method = transform_def->get_method("get_GameObject");

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

    const auto camera_pos = sdk::get_joint_position(camera_joint);
    //const auto camera_forward = sdk::get_joint_rotation(camera_joint) * Vector3f{ 0.0f, 0.0f, 1.0f };
    const auto camera_up = Vector3f{glm::mat4{sdk::get_joint_rotation(camera_joint)}[1]};

    ImGui::Begin("Chains");

    for (auto transform = first_transform; 
        transform != nullptr; 
        transform = next_transform_method->call<RETransform*>(context, transform)) 
    {
        auto attempt_display_chains = [&](RETransform* transform) {
            auto owner = get_gameobject_method->call<REGameObject*>(context, transform);

            if (owner == nullptr) {
                return;
            }

            auto owner_name = utility::re_string::get_string(owner->name);

            if (owner_name.empty()) {
                return;
            }

            static auto chain_type = sdk::find_type_definition("via.motion.Chain");
            static auto chain_re_type = chain_type->get_type();

            auto chain = utility::re_component::find<regenny::via::motion::Chain>(transform, chain_re_type);
            bool made = false;

            if (chain == nullptr) {
                return;
            }

            ImGui::PushID(chain);
            made = ImGui::TreeNode(chain, owner_name.data());

            if (made) {
                ObjectExplorer::get()->handle_address(chain);
            }

            if (chain != nullptr && chain->CollisionData.num > 0 && chain->CollisionData.collisions != nullptr) {
                for (auto i = 0; i < chain->CollisionData.num; ++i) {
                    #if TDB_VER >= 69
                    const auto& collider_top = chain->CollisionData.collisions[i];

                    for (auto j = 0; j < collider_top.num_collisions; ++j) {
                        const auto& collider = collider_top.collisions[j];
                    #else
                        const auto& collider = chain->CollisionData.collisions[i];
                    #endif
                        auto pos = *(Vector4f*)&collider.capsule.p0;

                        if (collider.joint != nullptr) {
                            //pos = sdk::get_joint_position((::REJoint*)collider.joint);
                        }

                        //const auto joint_pos = Vector3f{sdk::get_joint_position((::REJoint*)collider.joint)};

                        auto draw_circle = [&](const Vector3f& pos, float radius, bool outline) {
                            const auto screen_pos_center = sdk::renderer::world_to_screen(pos);

                            if (screen_pos_center) {
                                const auto pos_top = pos + (glm::normalize(camera_up) * radius);
                                const auto screen_pos_top = sdk::renderer::world_to_screen(pos_top);

                                if (screen_pos_top) {
                                    const auto radius2d = glm::length(*screen_pos_top - *screen_pos_center);

                                    // Inner
                                    ImGui::GetBackgroundDrawList()->AddCircleFilled(
                                        *(ImVec2*)&*screen_pos_center,
                                        radius2d,
                                        ImGui::GetColorU32(ImVec4(66.0f / 255.0f, 105.0f / 255.0f, 245.0f / 255.0f, 0.25f)),
                                        32
                                    );

                                    // Outline
                                    if (outline) {
                                        ImGui::GetBackgroundDrawList()->AddCircle(
                                            *(ImVec2*)&*screen_pos_center,
                                            radius2d,
                                            ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)),
                                            32
                                        );
                                    }
                                }
                            }
                        };

                        auto get_screen_radius = [&](const Vector3f& pos, float radius) -> std::optional<std::tuple<float, Vector2f>> {
                            const auto screen_pos_center = sdk::renderer::world_to_screen(pos);

                            if (screen_pos_center) {
                                const auto pos_top = pos + (glm::normalize(camera_up) * radius);
                                const auto screen_pos_top = sdk::renderer::world_to_screen(pos_top);

                                if (screen_pos_top) {
                                    const auto radius2d = glm::length(*screen_pos_top - *screen_pos_center);

                                    return std::make_tuple(radius2d, *screen_pos_center);
                                }
                            }

                            return std::nullopt;
                        };

                        if (collider.pair_joint == nullptr) {
                            draw_circle(*(Vector3f*)&collider.sphere.pos, collider.sphere.r, true);
                        } else {
                            // Capsule
                            draw_circle(*(Vector3f*)&collider.capsule.p0, collider.capsule.r, true);
                            draw_circle(*(Vector3f*)&collider.capsule.p1, collider.capsule.r, true);

                            // Get the half points of the circles and draw a filled rectangle between them
                            const auto top_screen_radius = get_screen_radius(*(Vector3f*)&collider.capsule.p0, collider.capsule.r);
                            const auto bottom_screen_radius = get_screen_radius(*(Vector3f*)&collider.capsule.p1, collider.capsule.r);

                            if (top_screen_radius && bottom_screen_radius) {
                                const auto top_radius = std::get<0>(*top_screen_radius);
                                const auto bottom_radius = std::get<0>(*bottom_screen_radius);

                                const auto top_circle_start = std::get<1>(*top_screen_radius);
                                const auto bottom_circle_start = std::get<1>(*bottom_screen_radius);

                                const auto delta = glm::normalize(bottom_circle_start - top_circle_start);
                                const auto angle = glm::atan(delta.y, delta.x) + glm::radians(90.0f);
                                
                                // Now get the halfway point(s) of the circles
                                float top_x_right = top_circle_start.x + (top_radius * std::cos(angle));
                                float top_y_right = top_circle_start.y + (top_radius * std::sin(angle));

                                float bottom_x_right = bottom_circle_start.x + (bottom_radius * std::cos(angle));
                                float bottom_y_right = bottom_circle_start.y + (bottom_radius * std::sin(angle));
                                
                                float top_x_left = top_circle_start.x + (top_radius * std::cos(angle + glm::radians(180.0f)));
                                float top_y_left = top_circle_start.y + (top_radius * std::sin(angle + glm::radians(180.0f)));

                                float bottom_x_left = bottom_circle_start.x + (bottom_radius * std::cos(angle + glm::radians(180.0f)));
                                float bottom_y_left = bottom_circle_start.y + (bottom_radius * std::sin(angle + glm::radians(180.0f)));

                                // Draw a quad
                                ImGui::GetBackgroundDrawList()->AddQuadFilled(
                                    ImVec2(top_x_left, top_y_left),
                                    ImVec2(bottom_x_left, bottom_y_left),
                                    ImVec2(bottom_x_right, bottom_y_right),
                                    ImVec2(top_x_right, top_y_right),
                                    ImGui::GetColorU32(ImVec4(66.0f / 255.0f, 105.0f / 255.0f, 245.0f / 255.0f, 0.25f))
                                );

                                ImGui::GetBackgroundDrawList()->AddQuad(
                                    ImVec2(top_x_left, top_y_left),
                                    ImVec2(bottom_x_left, bottom_y_left),
                                    ImVec2(bottom_x_right, bottom_y_right),
                                    ImVec2(top_x_right, top_y_right),
                                    ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f))
                                );
                            }
                        }

                        //world_to_screen_methods[1]->call<void*>(&screen_pos, context, &pos, &view, &proj, &screen_size);
                        //draw_list->AddText(ImVec2(screen_pos.x, screen_pos.y), ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), "Collision");

                        if (made) {
                            ImGui::PushID(&collider);
                            
                            ImGui::SetNextTreeNodeOpen(true, ImGuiCond_::ImGuiCond_Once);

                        #if TDB_VER >= 69
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
                #if TDB_VER >= 69
                    }
                #endif
                }
            }

            if (made) {
                ImGui::TreePop();
            }

            ImGui::PopID();
        };

        attempt_display_chains(transform);

        for (auto t = child_transform_method->call<RETransform*>(context, transform); t != nullptr; t = next_transform_method->call<RETransform*>(context, t)) {
            attempt_display_chains(t);

            for (auto t2 = child_transform_method->call<RETransform*>(context,t); t2 != nullptr; t2 = next_transform_method->call<RETransform*>(context, t2)) {
                attempt_display_chains(t2);
            }
        }
    }

    ImGui::End();
}

