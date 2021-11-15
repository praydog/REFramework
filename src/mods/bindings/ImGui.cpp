#include <imgui.h>

#include "../ScriptRunner.hpp"
#include "../../sdk/SceneManager.hpp"

#include "ImGui.hpp"

namespace api::imgui {
bool button(const char* label) {
    return ImGui::Button(label);
}

void text(const char* text) {
    ImGui::Text(text);
}

sol::variadic_results checkbox(sol::this_state s, const char* label, bool v) {
    auto changed = ImGui::Checkbox(label, &v);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, v));

    return results;
}

sol::variadic_results drag_float(sol::this_state s, const char* label, float v, float v_speed, float v_min, float v_max, const char* display_format = "%.3f") {
    auto changed = ImGui::DragFloat(label, &v, v_speed, v_min, v_max, display_format);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, v));

    return results;
}

sol::variadic_results drag_int(sol::this_state s, const char* label, int v, float v_speed, int v_min, int v_max, const char* display_format = "%.0f") {
    auto changed = ImGui::DragInt(label, &v, v_speed, v_min, v_max, display_format);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, v));

    return results;
}

sol::variadic_results input_text(sol::this_state s, const char* label, const std::string& v, ImGuiInputTextFlags flags = 0) {
    static std::vector<char> buffer{};

    if (v.size() + 1 > buffer.size()) {
        buffer.resize(v.size() + 1);
    }

    if (v.empty()) {
        buffer[0] = '\0';
    } else {
        strcpy_s(buffer.data(), v.size() + 1, v.c_str());
    }

    auto changed = ImGui::InputText(label, &buffer[0], 512, flags);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, std::string{buffer.data()}));

    return results;
}

sol::variadic_results combo(sol::this_state s, const char* label, int selection, sol::table values) {
    const char* preview_value = "";

    if (!values.empty()) {
        if (selection < 1 || selection > values.size()) {
            selection = 1;
        }

        auto val_at_selection = values[selection].get<sol::object>();

        if (val_at_selection.is<const char*>()) {
            preview_value = val_at_selection.as<const char*>();
        }
    }

    auto selection_changed = false;

    if (ImGui::BeginCombo(label, preview_value)) {
        for (auto i = 1u; i <= values.size(); ++i) {
            auto val_at_i = values[i].get<sol::object>();

            if (val_at_i.is<const char*>()) {
                auto entry = val_at_i.as<const char*>();

                if (ImGui::Selectable(entry, selection == i)) {
                    selection = i;
                    selection_changed = true;
                }
            }
        }

        ImGui::EndCombo();
    }

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, selection_changed));
    results.push_back(sol::make_object(s, selection));

    return results;
}
}

namespace api::draw {
void world_text(const char* text, sol::object world_pos_object, ImU32 color = 0xFFFFFFFF) {
    auto scene = sdk::get_current_scene();

    if (scene == nullptr) {
        return;
    }

    auto context = sdk::get_thread_context();

    static auto scene_def = sdk::RETypeDB::get()->find_type("via.Scene");
    auto first_transform = sdk::call_object_func<RETransform*>(scene, scene_def, "get_FirstTransform", context, scene);

    if (first_transform == nullptr) {
        return;
    }

    if (world_pos_object.is<sol::nil_t>()) {
        return;
    }

    Vector4f world_pos{};

    if (world_pos_object.is<Vector2f>()) {
        auto& v2f = world_pos_object.as<Vector2f&>();
        world_pos = Vector4f{v2f.x, v2f.y, 0.0f, 1.0f};
    } else if (world_pos_object.is<Vector3f>()) {
        auto& v3f = world_pos_object.as<Vector3f&>();
        world_pos = Vector4f{v3f.x, v3f.y, v3f.z, 1.0f};
    } else if (world_pos_object.is<Vector4f>()) {
        auto& v4f = world_pos_object.as<Vector4f&>();
        world_pos = Vector4f{v4f.x, v4f.y, v4f.z, v4f.w};
    } else {
        return;
    }

    static auto transform_def = utility::re_managed_object::get_type_definition(first_transform);
    static auto next_transform_method = transform_def->get_method("get_Next");
    static auto get_gameobject_method = transform_def->get_method("get_GameObject");
    static auto get_position_method = transform_def->get_method("get_Position");
    static auto get_axisz_method = transform_def->get_method("get_AxisZ");
    static auto math_t = sdk::RETypeDB::get()->find_type("via.math");

    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    auto main_view = sdk::get_main_view();

    if (main_view == nullptr) {
        return;
    }

    auto camera_gameobject = get_gameobject_method->call<REGameObject*>(context, camera);
    auto camera_transform = camera_gameobject->transform;

    Vector4f camera_origin{};
    get_position_method->call<void*>(&camera_origin, context, camera_transform);

    Vector4f camera_forward{};
    get_axisz_method->call<void*>(&camera_forward, context, camera_transform);

    // Translate 2d position to 3d position (screen to world)
    Matrix4x4f proj{}, view{};
    float screen_size[2]{};
    sdk::call_object_func<void*>(camera, "get_ProjectionMatrix", &proj, context, camera);
    sdk::call_object_func<void*>(camera, "get_ViewMatrix", &view, context, camera);
    sdk::call_object_func<void*>(main_view, "get_Size", &screen_size, context, main_view);

    static auto world_to_screen_methods = math_t->get_methods("worldPos2ScreenPos"); // there are 2 of them.

    Vector4f screen_pos{};

    auto draw_list = ImGui::GetBackgroundDrawList();
    const auto delta = world_pos - camera_origin;

    // behind camera
    if (glm::dot(delta, -camera_forward) <= 0.0f) {
        return;
    }

    world_to_screen_methods[1]->call<void*>(&screen_pos, context, &world_pos, &view, &proj, &screen_size);
    draw_list->AddText(ImVec2{screen_pos.x, screen_pos.y}, color, text);
}

void text(const char* text, float x, float y, ImU32 color) {
    ImGui::GetBackgroundDrawList()->AddText(ImVec2{x, y}, color, text);
}

void filled_rect(float x, float y, float w, float h, ImU32 color) {
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2{x, y}, ImVec2{x + w, y + h}, color);
}

void outline_rect(float x, float y, float w, float h, ImU32 color) {
    ImGui::GetBackgroundDrawList()->AddRect(ImVec2{x, y}, ImVec2{x + w, y + h}, color);
}

void line(float x1, float y1, float x2, float y2, ImU32 color) {
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2{x1, y1}, ImVec2{x2, y2}, color);
}
} // namespace api::draw

void bindings::open_imgui(ScriptState* s) {
    auto& lua = s->lua();
    auto imgui = lua.create_table();

    imgui["button"] = api::imgui::button;
    imgui["combo"] = api::imgui::combo;
    imgui["drag_float"] = api::imgui::drag_float;
    imgui["drag_int"] = api::imgui::drag_int;
    imgui["input_text"] = api::imgui::input_text;
    imgui["text"] = api::imgui::text;
    imgui["checkbox"] = api::imgui::checkbox;
    lua["imgui"] = imgui;

    auto draw = lua.create_table();

    draw["world_text"] = api::draw::world_text;
    draw["text"] = api::draw::text;
    draw["filled_rect"] = api::draw::filled_rect;
    draw["outline_rect"] = api::draw::outline_rect;
    draw["line"] = api::draw::line;
    lua["draw"] = draw;
}
