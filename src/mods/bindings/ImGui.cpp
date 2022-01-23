#include <imgui.h>

#include "../ScriptRunner.hpp"
#include "../../sdk/SceneManager.hpp"

#include "ImGui.hpp"

namespace api::imgui {
bool button(const char* label) {
    if (label == nullptr) {
        label = "";
    }

    return ImGui::Button(label);
}

void text(const char* text) {
    if (text == nullptr) {
        text = "";
    }

    ImGui::Text(text);
}

sol::variadic_results checkbox(sol::this_state s, const char* label, bool v) {
    if (label == nullptr) {
        label = "";
    }

    auto changed = ImGui::Checkbox(label, &v);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, v));

    return results;
}

sol::variadic_results drag_float(sol::this_state s, const char* label, float v, float v_speed, float v_min, float v_max, const char* display_format = "%.3f") {
    if (label == nullptr) {
        label = "";
    }

    auto changed = ImGui::DragFloat(label, &v, v_speed, v_min, v_max, display_format);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, v));

    return results;
}

sol::variadic_results drag_float2(sol::this_state s, const char* label, Vector2f v, float v_speed, float v_min, float v_max, const char* display_format = "%.3f") {
    if (label == nullptr) {
        label = "";
    }

    auto changed = ImGui::DragFloat2(label, (float*)&v, v_speed, v_min, v_max, display_format);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, v));

    return results;
}

sol::variadic_results drag_float3(sol::this_state s, const char* label, Vector3f v, float v_speed, float v_min, float v_max, const char* display_format = "%.3f") {
    if (label == nullptr) {
        label = "";
    }

    auto changed = ImGui::DragFloat3(label, (float*)&v, v_speed, v_min, v_max, display_format);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, v));

    return results;
}


sol::variadic_results drag_float4(sol::this_state s, const char* label, Vector4f v, float v_speed, float v_min, float v_max, const char* display_format = "%.3f") {
    if (label == nullptr) {
        label = "";
    }

    auto changed = ImGui::DragFloat4(label, (float*)&v, v_speed, v_min, v_max, display_format);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, v));

    return results;
}

sol::variadic_results drag_int(sol::this_state s, const char* label, int v, float v_speed, int v_min, int v_max, const char* display_format = "%d") {
    if (label == nullptr) {
        label = "";
    }

    auto changed = ImGui::DragInt(label, &v, v_speed, v_min, v_max, display_format);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, v));

    return results;
}

sol::variadic_results slider_float(sol::this_state s, const char* label, float v, float v_min, float v_max, const char* display_format = "%.3f") {
    if (label == nullptr) {
        label = "";
    }

    auto changed = ImGui::SliderFloat(label, &v, v_min, v_max, display_format);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, v));

    return results;
}


sol::variadic_results slider_int(sol::this_state s, const char* label, int v, int v_min, int v_max, const char* display_format = "%d") {
    if (label == nullptr) {
        label = "";
    }

    auto changed = ImGui::SliderInt(label, &v, v_min, v_max, display_format);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, v));

    return results;
}

sol::variadic_results input_text(sol::this_state s, const char* label, const std::string& v, ImGuiInputTextFlags flags = 0) {
    flags |= ImGuiInputTextFlags_CallbackResize;

    if (label == nullptr) {
        label = "";
    }

    static std::string buffer{""};
    buffer = v;

    static auto input_text_callback = [](ImGuiInputTextCallbackData* data) -> int {
        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
            buffer.resize(data->BufTextLen);
            data->Buf = (char*)buffer.c_str();
        }

        return 0;
    };

    auto changed = ImGui::InputText(label, buffer.data(), buffer.capacity() + 1, flags, input_text_callback);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, std::string{buffer.data()}));

    return results;
}

sol::variadic_results combo(sol::this_state s, const char* label, int selection, sol::table values) {
    if (label == nullptr) {
        label = "";
    }

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

bool tree_node(const char* label) {
    if (label == nullptr) {
        label = "";
    }

    return ImGui::TreeNode(label);
}

bool tree_node_ptr_id(const void* id, const char* label) {
    if (label == nullptr) {
        label = "";
    }

    return ImGui::TreeNode(id, label);
}

bool tree_node_str_id(const char* id, const char* label) {
    if (label == nullptr) {
        label = "";
    }

    return ImGui::TreeNode(id, label);
}

void tree_pop() {
    ImGui::TreePop();
}

void same_line() {
    ImGui::SameLine();
}

bool is_item_hovered() {
    return ImGui::IsItemHovered();
}

bool begin_window(const char* name, sol::object open_obj, ImGuiWindowFlags flags = 0) {
    if (name == nullptr) {
        name = "";
    }

    bool open = true;
    bool* open_p = nullptr;

    if (!open_obj.is<sol::nil_t>() && open_obj.is<bool>()) {
        open = open_obj.as<bool>();
        open_p = &open;
    }

    if (!open) {
        return false;
    }

    ImGui::Begin(name, open_p, flags);

    return open;
}

void end_window() {
    ImGui::End();
}

bool begin_child_window(const char* name, sol::object size_obj, sol::object border_obj, ImGuiWindowFlags flags = 0) {
    if (name == nullptr) {
        name = "";
    }

    ImVec2 size{0, 0};
    bool border{false};

    if (size_obj.is<Vector2f>()) {
        size = ImVec2{size_obj.as<Vector2f>().x, size_obj.as<Vector2f>().y};
    } else if (size_obj.is<Vector3f>()) {
        size = ImVec2{size_obj.as<Vector3f>().x, size_obj.as<Vector3f>().y};
    } else if (size_obj.is<Vector4f>()) {
        size = ImVec2{size_obj.as<Vector4f>().x, size_obj.as<Vector4f>().y};
    }

    if (border_obj.is<bool>()) {
        border = border_obj.as<bool>();
    }

    return ImGui::BeginChild(name, size, border, flags);
}

void end_child_window() {
    ImGui::EndChild();
}

void begin_group() {
    ImGui::BeginGroup();
}

void end_group() {
    ImGui::EndGroup();
}

void begin_rect() {
    ImGui::BeginGroup();
}

void end_rect(sol::object additional_size_obj, sol::object rounding_obj) {
    ImGui::EndGroup();

    float rounding = rounding_obj.is<float>() ? rounding_obj.as<float>() : ImGui::GetStyle().FrameRounding;
    float additional_size = additional_size_obj.is<float>() ? additional_size_obj.as<float>() : 0.0f;

    auto mins = ImGui::GetItemRectMin();
    mins.x -= additional_size;
    mins.y -= additional_size;

    auto maxs = ImGui::GetItemRectMax();
    maxs.x += additional_size;
    maxs.y += additional_size;

    ImGui::GetWindowDrawList()->AddRect(mins, maxs, ImGui::GetColorU32(ImGuiCol_Border), ImGui::GetStyle().FrameRounding, ImDrawCornerFlags_All, 1.0f);
}

void separator() {
    ImGui::Separator();
}

void spacing() {
    ImGui::Spacing();
}

void new_line() {
    ImGui::NewLine();
}

bool collapsing_header(const char* name) {
    return ImGui::CollapsingHeader(name);
}

int load_font(const char* filepath, int size, sol::object ranges) {
    namespace fs = std::filesystem;

    std::string modpath{};

    modpath.resize(1024, 0);
    modpath.resize(GetModuleFileName(nullptr, modpath.data(), modpath.size()));

    auto fonts_path = fs::path{modpath}.parent_path() / "reframework" / "fonts";
    auto font_path = fonts_path / filepath;

    fs::create_directories(fonts_path);
    std::vector<ImWchar> ranges_vec{};

    if (ranges.is<sol::table>()) {
        sol::table ranges_table = ranges;

        for (auto i = 1u; i <= ranges_table.size(); ++i) {
            ranges_vec.push_back(ranges_table[i].get<ImWchar>());
        }
    }

    return g_framework->add_font(font_path, size, ranges_vec);
}

void push_font(int font) {
    ImGui::PushFont(g_framework->get_font(font));
}

void pop_font() {
    ImGui::PopFont();
}
} // namespace api::imgui

namespace api::draw {
std::optional<Vector2f> world_to_screen(sol::object world_pos_object) {
    auto scene = sdk::get_current_scene();

    if (scene == nullptr) {
        return std::nullopt;
    }

    auto context = sdk::get_thread_context();

    static auto scene_def = sdk::RETypeDB::get()->find_type("via.Scene");
    auto first_transform = sdk::call_object_func<RETransform*>(scene, scene_def, "get_FirstTransform", context, scene);

    if (first_transform == nullptr) {
        return std::nullopt;
    }

    if (world_pos_object.is<sol::nil_t>()) {
        return std::nullopt;
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
        return std::nullopt;
    }

    static auto transform_def = utility::re_managed_object::get_type_definition(first_transform);
    static auto next_transform_method = transform_def->get_method("get_Next");
    static auto get_gameobject_method = transform_def->get_method("get_GameObject");
    static auto get_position_method = transform_def->get_method("get_Position");
    static auto get_axisz_method = transform_def->get_method("get_AxisZ");
    static auto math_t = sdk::RETypeDB::get()->find_type("via.math");

    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return std::nullopt;
    }

    auto main_view = sdk::get_main_view();

    if (main_view == nullptr) {
        return std::nullopt;
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

    Vector4f screen_pos{};

    const auto delta = world_pos - camera_origin;

    // behind camera
    if (glm::dot(delta, -camera_forward) <= 0.0f) {
        return std::nullopt;
    }

    static auto world_to_screen_method = math_t->get_method("worldPos2ScreenPos(via.vec3, via.mat4, via.mat4, via.Size)"); // there are 2 of them.
    world_to_screen_method->call<void*>(&screen_pos, context, &world_pos, &view, &proj, &screen_size);

    return Vector2f{screen_pos.x, screen_pos.y};
}

void world_text(const char* text, sol::object world_pos_object, ImU32 color = 0xFFFFFFFF) {
    auto screen_pos = world_to_screen(world_pos_object);

    if (!screen_pos) {
        return;
    }

    auto draw_list = ImGui::GetBackgroundDrawList();
    draw_list->AddText(ImVec2{screen_pos->x, screen_pos->y}, color, text);
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
    imgui["drag_float2"] = api::imgui::drag_float2;
    imgui["drag_float3"] = api::imgui::drag_float3;
    imgui["drag_float4"] = api::imgui::drag_float4;
    imgui["drag_int"] = api::imgui::drag_int;
    imgui["slider_float"] = api::imgui::slider_float;
    imgui["slider_int"] = api::imgui::slider_int;
    imgui["input_text"] = api::imgui::input_text;
    imgui["text"] = api::imgui::text;
    imgui["checkbox"] = api::imgui::checkbox;
    imgui["tree_node"] = api::imgui::tree_node;
    imgui["tree_node_ptr_id"] = api::imgui::tree_node_ptr_id;
    imgui["tree_node_str_id"] = api::imgui::tree_node_str_id;
    imgui["tree_pop"] = api::imgui::tree_pop;
    imgui["same_line"] = api::imgui::same_line;
    imgui["is_item_hovered"] = api::imgui::is_item_hovered;
    imgui["begin_window"] = api::imgui::begin_window;
    imgui["end_window"] = api::imgui::end_window;
    imgui["begin_child_window"] = api::imgui::begin_child_window;
    imgui["end_child_window"] = api::imgui::end_child_window;
    imgui["begin_group"] = api::imgui::begin_group;
    imgui["end_group"] = api::imgui::end_group;
    imgui["begin_rect"] = api::imgui::begin_rect;
    imgui["end_rect"] = api::imgui::end_rect;
    imgui["separator"] = api::imgui::separator;
    imgui["spacing"] = api::imgui::spacing;
    imgui["new_line"] = api::imgui::new_line;
    imgui["collapsing_header"] = api::imgui::collapsing_header;
    imgui["load_font"] = api::imgui::load_font;
    imgui["push_font"] = api::imgui::push_font;
    imgui["pop_font"] = api::imgui::pop_font;
    lua["imgui"] = imgui;

    auto draw = lua.create_table();

    draw["world_to_screen"] = api::draw::world_to_screen;
    draw["world_text"] = api::draw::world_text;
    draw["text"] = api::draw::text;
    draw["filled_rect"] = api::draw::filled_rect;
    draw["outline_rect"] = api::draw::outline_rect;
    draw["line"] = api::draw::line;
    lua["draw"] = draw;
}
