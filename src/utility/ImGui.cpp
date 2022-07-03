#include <tuple>

#include <sdk/SceneManager.hpp>
#include <sdk/Renderer.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

#include "ImGui.hpp"

namespace imgui {
bool draw_gizmo(Matrix4x4f& mat, IMGUIZMO_NAMESPACE::OPERATION op, IMGUIZMO_NAMESPACE::MODE mode) {
    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return false;
    }

    static auto get_proj_method = sdk::find_method_definition("via.Camera", "get_ProjectionMatrix");
    static auto get_view_method = sdk::find_method_definition("via.Camera", "get_ViewMatrix");

    IMGUIZMO_NAMESPACE::SetImGuiContext(ImGui::GetCurrentContext());
    IMGUIZMO_NAMESPACE::SetDrawlist(ImGui::GetBackgroundDrawList());
    IMGUIZMO_NAMESPACE::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

    Matrix4x4f proj{}, view{};
    get_proj_method->call<void*>(&proj, sdk::get_thread_context(), camera);
    get_view_method->call<void*>(&view, sdk::get_thread_context(), camera);

    return IMGUIZMO_NAMESPACE::Manipulate((float*)&view, (float*)&proj, op, mode, (float*)&mat);
}

void draw_cube(const Matrix4x4f& mat) {
    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    static auto get_proj_method = sdk::find_method_definition("via.Camera", "get_ProjectionMatrix");
    static auto get_view_method = sdk::find_method_definition("via.Camera", "get_ViewMatrix");

    IMGUIZMO_NAMESPACE::SetImGuiContext(ImGui::GetCurrentContext());
    IMGUIZMO_NAMESPACE::SetDrawlist(ImGui::GetBackgroundDrawList());
    IMGUIZMO_NAMESPACE::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

    Matrix4x4f proj{}, view{};
    get_proj_method->call<void*>(&proj, sdk::get_thread_context(), camera);
    get_view_method->call<void*>(&view, sdk::get_thread_context(), camera);

    IMGUIZMO_NAMESPACE::DrawCubes((float*)&view, (float*)&proj, (float*)&mat, 1);
}

void draw_grid(const Matrix4x4f& mat, float size) {
    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    static auto get_proj_method = sdk::find_method_definition("via.Camera", "get_ProjectionMatrix");
    static auto get_view_method = sdk::find_method_definition("via.Camera", "get_ViewMatrix");

    IMGUIZMO_NAMESPACE::SetImGuiContext(ImGui::GetCurrentContext());
    IMGUIZMO_NAMESPACE::SetDrawlist(ImGui::GetBackgroundDrawList());
    IMGUIZMO_NAMESPACE::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

    Matrix4x4f proj{}, view{};
    get_proj_method->call<void*>(&proj, sdk::get_thread_context(), camera);
    get_view_method->call<void*>(&view, sdk::get_thread_context(), camera);

    IMGUIZMO_NAMESPACE::DrawGrid((float*)&view, (float*)&proj, (float*)&mat, size);
}

std::optional<Vector3f> get_camera_up() {
    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return std::nullopt;
    }

    static auto transform_def = sdk::find_type_definition("via.Transform");
    static auto get_gameobject_method = transform_def->get_method("get_GameObject");
    static auto get_joints_method = transform_def->get_method("get_Joints");

    auto camera_gameobject = get_gameobject_method->call<::REGameObject*>(sdk::get_thread_context(), camera);

    if (camera_gameobject == nullptr) {
        std::nullopt;
    }

    auto camera_transform = camera_gameobject->transform;

    if (camera_transform == nullptr) {
        std::nullopt;
    }

    auto camera_joints = get_joints_method->call<sdk::SystemArray*>(sdk::get_thread_context(), camera_transform);

    if (camera_joints == nullptr) {
        std::nullopt;
    }

    auto camera_joint = (::REJoint*)camera_joints->get_element(0);

    if (camera_joint == nullptr) {
        std::nullopt;
    }

    return sdk::get_joint_rotation(camera_joint) * Vector3f{ 0.0f, 1.0f, 0.0f };
}

void draw_sphere(const Vector3f& center, float radius, ImU32 color, bool outline) {
    const auto camera_up = get_camera_up();

    if (!camera_up) {
        return;
    }

    const auto screen_pos_center = sdk::renderer::world_to_screen(center);

    if (screen_pos_center) {
        const auto pos_top = center + (glm::normalize(*camera_up) * radius);
        const auto screen_pos_top = sdk::renderer::world_to_screen(pos_top);

        if (screen_pos_top) {
            const auto radius2d = glm::length(*screen_pos_top - *screen_pos_center);

            // Inner
            ImGui::GetBackgroundDrawList()->AddCircleFilled(
                *(ImVec2*)&*screen_pos_center,
                radius2d,
                color,
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
}

void draw_capsule(const Vector3f& start, const Vector3f& end, float radius, ImU32 color, bool outline) {
    const auto camera_up = get_camera_up();

    if (!camera_up) {
        return;
    }

    
    auto get_screen_radius = [&](const Vector3f& pos, float radius) -> std::optional<std::tuple<float, Vector2f>> {
        const auto screen_pos_center = sdk::renderer::world_to_screen(pos);

        if (screen_pos_center) {
            const auto pos_top = pos + (glm::normalize(*camera_up) * radius);
            const auto screen_pos_top = sdk::renderer::world_to_screen(pos_top);

            if (screen_pos_top) {
                const auto radius2d = glm::length(*screen_pos_top - *screen_pos_center);

                return std::make_tuple(radius2d, *screen_pos_center);
            }
        }

        return std::nullopt;
    };

    // Capsule
    draw_sphere(start, radius, color, true);
    draw_sphere(end, radius, color, true);

    // Get the half points of the circles and draw a filled rectangle between them
    const auto top_screen_radius = get_screen_radius(start, radius);
    const auto bottom_screen_radius = get_screen_radius(end, radius);

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
            color
        );

        if (outline) {
            ImGui::GetBackgroundDrawList()->AddQuad(
                ImVec2(top_x_left, top_y_left),
                ImVec2(bottom_x_left, bottom_y_left),
                ImVec2(bottom_x_right, bottom_y_right),
                ImVec2(top_x_right, top_y_right),
                ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f))
            );
        }
    }
}

void progress_bar(float progress, const ImVec2& size_arg, const char* overlay) {
    const ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    const ImVec2 pos = window->DC.CursorPos;
    const ImVec2 size = ImGui::CalcItemSize(size_arg, ImGui::CalcItemWidth(), g.FontSize + style.FramePadding.y * 2.0f);
    ImRect bb(pos, pos + size);
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, 0)) {
        return;
    }

    const bool indeterminate = progress < 0.0f;
    float bar_begin = 0.0f, bar_end = ImSaturate(progress);

    if (indeterminate) {
        constexpr float bar_fraction = 0.2f;
        bar_begin = ImFmod(-progress, 1.0f) * (1.0f + bar_fraction) - bar_fraction;
        bar_end = ImMin(bar_begin + bar_fraction, 1.0f);
    }

    // Render
    progress = ImSaturate(progress);
    ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
    bb.Expand({ -style.FrameBorderSize, -style.FrameBorderSize });
    ImGui::RenderRectFilledRangeH(window->DrawList, bb, ImGui::GetColorU32(ImGuiCol_PlotHistogram), bar_begin, bar_end, style.FrameRounding);

    // Default displaying the fraction as percentage string, but user can override it
    if (!indeterminate || overlay) {
        if (!overlay) {
            char overlay_buf[32];
            ImFormatString(overlay_buf, IM_ARRAYSIZE(overlay_buf), "%.0f%%", progress * 100 + 0.01f);
            overlay = overlay_buf;
        }

        const ImVec2 overlay_size = ImGui::CalcTextSize(overlay, nullptr);
        if (overlay_size.x > 0.0f) {
            const ImVec2 fill_br = { (bb.Min.x + bb.Max.x - overlay_size.x) * 0.5f, bb.Max.y };
            ImGui::RenderTextClipped(
                ImVec2(ImClamp(fill_br.x + style.ItemSpacing.x, bb.Min.x, bb.Max.x - overlay_size.x - style.ItemInnerSpacing.x), bb.Min.y),
                bb.Max, overlay, nullptr, &overlay_size, ImVec2(0.0f, 0.5f), &bb);
        }
    }
}
}

#undef IMGUI_DEFINE_MATH_OPERATORS
