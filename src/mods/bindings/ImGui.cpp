#include <imgui.h>

#include "../ScriptRunner.hpp"
#include "sdk/SceneManager.hpp"
#include "REFramework.hpp"
#include "utility/ImGui.hpp"

#include "ImGui.hpp"

namespace api::imgui {
ImVec2 create_imvec2(sol::object obj) {
    ImVec2 out{ 0.0f, 0.0f };

    if (obj.is<Vector2f>()) {
        auto vec = obj.as<Vector2f>();
        out.x = vec.x;
        out.y = vec.y;
    } else if (obj.is<sol::table>()) {
        auto table = obj.as<sol::table>();

        if (table.size() == 2) {
            out.x = table.get<float>(1);
            out.y = table.get<float>(2);
        } else {
            throw sol::error{ "Invalid table passed. Table size must be 2." };
        }
    }

    return out;
};

ImVec4 create_imvec4(sol::object obj) {
    ImVec4 out{ 0.0f, 0.0f, 0.0f, 0.0f };

    if (obj.is<Vector4f>()) {
        auto vec = obj.as<Vector4f>();
        out.x = vec.x;
        out.y = vec.y;
        out.z = vec.z;
        out.w = vec.w;
    } else if (obj.is<sol::table>()) {
        auto table = obj.as<sol::table>();

        if (table.size() == 4) {
            out.x = table.get<float>(1);
            out.y = table.get<float>(2);
            out.z = table.get<float>(3);
            out.w = table.get<float>(4);
        } else {
            throw sol::error{ "Invalid table passed. Table size must be 4." };
        }
    }

    return out;
};

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

sol::variadic_results input_text(sol::this_state s, const char* label, const std::string& v, ImGuiInputTextFlags flags) {
    flags |= ImGuiInputTextFlags_CallbackResize;

    if (label == nullptr) {
        label = "";
    }

    static std::string buffer{""};
    buffer = v;

    static auto input_text_callback = [](ImGuiInputTextCallbackData* data) -> int {
        if ((data->EventFlag & ImGuiInputTextFlags_CallbackResize) != 0) {
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

int load_font(sol::object filepath_obj, int size, sol::object ranges) {
    namespace fs = std::filesystem;
    const char* filepath = "doesnt-exist.not-a-real-font";

    if (filepath_obj.is<const char*>()) {
        filepath = filepath_obj.as<const char*>();
    }

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

int get_default_font_size() {
    return g_framework->get_font_size();
}

sol::variadic_results color_picker(sol::this_state s, const char* label, unsigned int color, sol::object flags_obj) {
    ImGuiColorEditFlags flags{};

    if (flags_obj.is<int>()) {
        flags = (ImGuiColorEditFlags)flags_obj.as<int>();
    }

    auto r = color & 0xFF;
    auto g = (color >> 8) & 0xFF;
    auto b = (color >> 16) & 0xFF;
    auto a = (color >> 24) & 0xFF;

    float col[4]{
        r / 255.0f,
        g / 255.0f,
        b / 255.0f,
        a / 255.0f,
    };

    auto changed = ImGui::ColorPicker4(label, col, flags);

    r = (unsigned int)(col[0] * 255.0f);
    g = (unsigned int)(col[1] * 255.0f);
    b = (unsigned int)(col[2] * 255.0f);
    a = (unsigned int)(col[3] * 255.0f);

    unsigned int new_color = 0;

    new_color |= r;
    new_color |= g << 8;
    new_color |= b << 16;
    new_color |= a << 24;

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, new_color));

    return results;
}

sol::variadic_results color_picker_argb(sol::this_state s, const char* label, unsigned int color, sol::object flags_obj) {
    ImGuiColorEditFlags flags{};

    if (flags_obj.is<int>()) {
        flags = (ImGuiColorEditFlags)flags_obj.as<int>();
    }

    auto r = (color >> 16) & 0xFF;
    auto g = (color >> 8) & 0xFF;
    auto b = color & 0xFF;
    auto a = (color >> 24) & 0xFF;

    float col[4]{
        r / 255.0f,
        g / 255.0f,
        b / 255.0f,
        a / 255.0f,
    };

    auto changed = ImGui::ColorPicker4(label, col, flags);

    r = (unsigned int)(col[0] * 255.0f);
    g = (unsigned int)(col[1] * 255.0f);
    b = (unsigned int)(col[2] * 255.0f);
    a = (unsigned int)(col[3] * 255.0f);

    unsigned int new_color = 0;

    new_color |= r << 16;
    new_color |= g << 8;
    new_color |= b;
    new_color |= a << 24;

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, new_color));

    return results;
}

sol::variadic_results color_picker3(sol::this_state s, const char* label, Vector3f color, sol::object flags_obj) {
    if (label == nullptr) {
        label = "";
    }

    ImGuiColorEditFlags flags{};

    if (flags_obj.is<int>()) {
        flags = (ImGuiColorEditFlags)flags_obj.as<int>();
    }

    auto changed = ImGui::ColorPicker3(label, &color.x, flags);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, color));

    return results;
}

sol::variadic_results color_picker4(sol::this_state s, const char* label, Vector4f color, sol::object flags_obj) {
    if (label == nullptr) {
        label = "";
    }

    ImGuiColorEditFlags flags{};

    if (flags_obj.is<int>()) {
        flags = (ImGuiColorEditFlags)flags_obj.as<int>();
    }

    auto changed = ImGui::ColorPicker4(label, &color.x, flags);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, color));

    return results;
}

sol::variadic_results color_edit(sol::this_state s, const char* label, unsigned int color, sol::object flags_obj) {
    ImGuiColorEditFlags flags{};

    if (flags_obj.is<int>()) {
        flags = (ImGuiColorEditFlags)flags_obj.as<int>();
    }

   
    auto r = color & 0xFF;
    auto g = (color >> 8) & 0xFF;
    auto b = (color >> 16) & 0xFF;
    auto a = (color >> 24) & 0xFF;

    float col[4]{
        r / 255.0f,
        g / 255.0f,
        b / 255.0f,
        a / 255.0f,
    };

    auto changed = ImGui::ColorEdit4(label, col, flags);

    r = (unsigned int)(col[0] * 255.0f);
    g = (unsigned int)(col[1] * 255.0f);
    b = (unsigned int)(col[2] * 255.0f);
    a = (unsigned int)(col[3] * 255.0f);

    unsigned int new_color = 0;

    new_color |= r;
    new_color |= g << 8;
    new_color |= b << 16;
    new_color |= a << 24;

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, new_color));

    return results;
}

sol::variadic_results color_edit_argb(sol::this_state s, const char* label, unsigned int color, sol::object flags_obj) {
    ImGuiColorEditFlags flags{};

    if (flags_obj.is<int>()) {
        flags = (ImGuiColorEditFlags)flags_obj.as<int>();
    }

    auto r = (color >> 16) & 0xFF;
    auto g = (color >> 8) & 0xFF;
    auto b = color & 0xFF;
    auto a = (color >> 24) & 0xFF;

    float col[4]{
        r / 255.0f,
        g / 255.0f,
        b / 255.0f,
        a / 255.0f,
    };

    auto changed = ImGui::ColorEdit4(label, col, flags);

    r = (unsigned int)(col[0] * 255.0f);
    g = (unsigned int)(col[1] * 255.0f);
    b = (unsigned int)(col[2] * 255.0f);
    a = (unsigned int)(col[3] * 255.0f);

    unsigned int new_color = 0;

    new_color |= r << 16;
    new_color |= g << 8;
    new_color |= b;
    new_color |= a << 24;

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, new_color));

    return results;
}

sol::variadic_results color_edit3(sol::this_state s, const char* label, Vector3f color, sol::object flags_obj) {
    if (label == nullptr) {
        label = "";
    }

    ImGuiColorEditFlags flags{};

    if (flags_obj.is<int>()) {
        flags = (ImGuiColorEditFlags)flags_obj.as<int>();
    }

    auto changed = ImGui::ColorEdit3(label, &color.x, flags);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, color));

    return results;
}

sol::variadic_results color_edit4(sol::this_state s, const char* label, Vector4f color, sol::object flags_obj) {
    if (label == nullptr) {
        label = "";
    }

    ImGuiColorEditFlags flags{};

    if (flags_obj.is<int>()) {
        flags = (ImGuiColorEditFlags)flags_obj.as<int>();
    }

    auto changed = ImGui::ColorEdit4(label, &color.x, flags);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, color));

    return results;
}

void set_next_window_pos(sol::object pos_obj, sol::object condition_obj, sol::object pivot_obj) {
    ImGuiCond condition{};

    if (condition_obj.is<int>()) {
        condition = (ImGuiCond)condition_obj.as<int>();
    }

    auto pos = create_imvec2(pos_obj);
    auto pivot = create_imvec2(pivot_obj);

    ImGui::SetNextWindowPos(pos, condition, pivot);
}

void set_next_window_size(sol::object size_obj, sol::object condition_obj) {
    ImGuiCond condition{};

    if (condition_obj.is<int>()) {
        condition = (ImGuiCond)condition_obj.as<int>();
    }

    auto size = create_imvec2(size_obj);

    ImGui::SetNextWindowSize(size, condition);
}

void push_id(sol::object id) {
    if (id.is<int>()) {
        ImGui::PushID(id.as<int>());
    } else if (id.is<const char*>()) {
        ImGui::PushID(id.as<const char*>());
    } else if (id.is<void*>()) {
        ImGui::PushID(id.as<void*>());
    } else {
        throw sol::error("Type must be int, const char* or void*");
    }
}

void pop_id() {
    ImGui::PopID();
}

Vector2f get_mouse() {
    const auto mouse = ImGui::GetMousePos();

    return Vector2f{
        mouse.x,
        mouse.y,
    };
}
} // namespace api::imgui

namespace api::draw {
std::optional<Vector2f> world_to_screen(sol::object world_pos_object) {
    auto scene = sdk::get_current_scene();

    if (scene == nullptr) {
        return std::nullopt;
    }

    auto context = sdk::get_thread_context();

    static auto scene_def = sdk::find_type_definition("via.Scene");
    auto first_transform = sdk::call_native_func_easy<RETransform*>(scene, scene_def, "get_FirstTransform");

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
    static auto math_t = sdk::find_type_definition("via.math");

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

void outline_circle(float x, float y, float radius, ImU32 color, sol::object num_segments) {
    auto segments = num_segments.is<sol::nil_t>() ? 32 : num_segments.as<int>();

    ImGui::GetBackgroundDrawList()->AddCircle(ImVec2{x, y}, radius, color, segments);
}

void filled_circle(float x, float y, float radius, ImU32 color, sol::object num_segments) {
    auto segments = num_segments.is<sol::nil_t>() ? 32 : num_segments.as<int>();

    ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2{x, y}, radius, color, segments);
}

void outline_quad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, ImU32 color) {
    ImGui::GetBackgroundDrawList()->AddQuad(ImVec2{x1, y1}, ImVec2{x2, y2}, ImVec2{x3, y3}, ImVec2{x4, y4}, color);
}

void filled_quad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, ImU32 color) {
    ImGui::GetBackgroundDrawList()->AddQuadFilled(ImVec2{x1, y1}, ImVec2{x2, y2}, ImVec2{x3, y3}, ImVec2{x4, y4}, color);
}

void sphere(sol::object world_pos_object, float radius, ImU32 color, bool outline) {
    Vector3f world_pos{};

    if (world_pos_object.is<Vector2f>()) {
        auto& v2f = world_pos_object.as<Vector2f&>();
        world_pos = Vector3f{v2f.x, v2f.y, 0.0f};
    } else if (world_pos_object.is<Vector3f>()) {
        auto& v3f = world_pos_object.as<Vector3f&>();
        world_pos = Vector3f{v3f.x, v3f.y, v3f.z};
    } else if (world_pos_object.is<Vector4f>()) {
        auto& v4f = world_pos_object.as<Vector4f&>();
        world_pos = Vector3f{v4f.x, v4f.y, v4f.z};
    } else {
        return;
    }

    ::imgui::draw_sphere(world_pos, radius, color, outline);
}

void capsule(sol::object start_pos_object, sol::object end_pos_object, float radius, ImU32 color, bool outline) {
    Vector3f start_pos{};

    if (start_pos_object.is<Vector2f>()) {
        auto& v2f = start_pos_object.as<Vector2f&>();
        start_pos = Vector3f{v2f.x, v2f.y, 0.0f};
    } else if (start_pos_object.is<Vector3f>()) {
        auto& v3f = start_pos_object.as<Vector3f&>();
        start_pos = Vector3f{v3f.x, v3f.y, v3f.z};
    } else if (start_pos_object.is<Vector4f>()) {
        auto& v4f = start_pos_object.as<Vector4f&>();
        start_pos = Vector3f{v4f.x, v4f.y, v4f.z};
    } else {
        return;
    }

    Vector3f end_pos{};

    if (end_pos_object.is<Vector2f>()) {
        auto& v2f = end_pos_object.as<Vector2f&>();
        end_pos = Vector3f{v2f.x, v2f.y, 0.0f};
    } else if (end_pos_object.is<Vector3f>()) {
        auto& v3f = end_pos_object.as<Vector3f&>();
        end_pos = Vector3f{v3f.x, v3f.y, v3f.z};
    } else if (end_pos_object.is<Vector4f>()) {
        auto& v4f = end_pos_object.as<Vector4f&>();
        end_pos = Vector3f{v4f.x, v4f.y, v4f.z};
    } else {
        return;
    }

    ::imgui::draw_capsule(start_pos, end_pos, radius, color, outline);
}

sol::variadic_results gizmo(sol::this_state s, int64_t unique_id, Matrix4x4f& transform, sol::object operation_obj, sol::object mode_obj) {
    if (!ImGui::GetIO().MouseDown[0]) {
        ImGuizmo::Enable(false);
        ImGuizmo::Enable(true);
    }

    ImGuizmo::OPERATION operation{};
    ImGuizmo::MODE mode{};

    if (mode_obj.is<sol::nil_t>()) {
        mode = ImGuizmo::MODE::WORLD;
    } else if (mode_obj.is<int>()) {
        mode = (ImGuizmo::MODE)mode_obj.as<int>();
    } else {
        throw sol::error("Invalid mode passed to gizmo");
    }

    if (operation_obj.is<sol::nil_t>()) {
        operation = ImGuizmo::OPERATION::UNIVERSAL;
    } else if (operation_obj.is<int>()) {
        operation = (ImGuizmo::OPERATION)operation_obj.as<int>();
    } else {
        throw sol::error("Invalid operation passed to gizmo");
    }

    ImGuizmo::SetID(unique_id);
    bool changed = ::imgui::draw_gizmo(transform, operation, mode);

    sol::variadic_results results{};

    results.push_back(sol::make_object<bool>(s, changed));
    results.push_back(sol::make_object<Matrix4x4f>(s, transform));

    return results;
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
    imgui["get_default_font_size"] = api::imgui::get_default_font_size;
    imgui["color_picker"] = api::imgui::color_picker;
    imgui["color_picker_argb"] = api::imgui::color_picker_argb;
    imgui["color_picker3"] = api::imgui::color_picker3;
    imgui["color_picker4"] = api::imgui::color_picker4;
    imgui["color_edit"] = api::imgui::color_edit;
    imgui["color_edit_argb"] = api::imgui::color_edit_argb;
    imgui["color_edit3"] = api::imgui::color_edit3;
    imgui["color_edit4"] = api::imgui::color_edit4;
    imgui["set_next_window_pos"] = api::imgui::set_next_window_pos;
    imgui["set_next_window_size"] = api::imgui::set_next_window_size;
    imgui["push_id"] = api::imgui::push_id;
    imgui["pop_id"] = api::imgui::pop_id;
    imgui["get_mouse"] = api::imgui::get_mouse;
    imgui.new_enum("ImGuizmoOperation", 
                    "TRANSLATE", ImGuizmo::OPERATION::TRANSLATE, 
                    "ROTATE", ImGuizmo::OPERATION::ROTATE,
                    "SCALE", ImGuizmo::OPERATION::SCALE,
                    "SCALEU", ImGuizmo::OPERATION::SCALEU,
                    "UNIVERSAL", ImGuizmo::OPERATION::UNIVERSAL);
    imgui.new_enum("ImGuizmoMode", 
                    "WORLD", ImGuizmo::MODE::WORLD,
                    "LOCAL", ImGuizmo::MODE::LOCAL);
    lua["imgui"] = imgui;

    auto imguizmo = lua.create_table();

    imguizmo["is_over"] = [] { return ImGuizmo::IsOver(); };
    imguizmo["is_using"] = [] { return ImGuizmo::IsUsing(); };

    lua["imguizmo"] = imguizmo;

    auto draw = lua.create_table();

    draw["world_to_screen"] = api::draw::world_to_screen;
    draw["world_text"] = api::draw::world_text;
    draw["text"] = api::draw::text;
    draw["filled_rect"] = api::draw::filled_rect;
    draw["outline_rect"] = api::draw::outline_rect;
    draw["line"] = api::draw::line;
    draw["outline_circle"] = api::draw::outline_circle;
    draw["filled_circle"] = api::draw::filled_circle;
    draw["outline_quad"] = api::draw::outline_quad;
    draw["filled_quad"] = api::draw::filled_quad;
    draw["sphere"] = api::draw::sphere;
    draw["capsule"] = api::draw::capsule;
    draw["gizmo"] = api::draw::gizmo;
    draw["cube"] = [](const Matrix4x4f& mat) { ::imgui::draw_cube(mat); };
    draw["grid"] = [](const Matrix4x4f& mat, float size) { ::imgui::draw_grid(mat, size); };
    lua["draw"] = draw;
}
