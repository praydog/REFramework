#include <imgui.h>
#include <imgui_internal.h>
#include <imnodes.h>

#include "../ScriptRunner.hpp"
#include "sdk/SceneManager.hpp"
#include "REFramework.hpp"
#include "utility/ImGui.hpp"

#include "ImGui.hpp"

namespace api::imgui {
int32_t g_disabled_counts{0};

void cleanup() {
    for (auto i = 0; i < g_disabled_counts; ++i) {
        ImGui::EndDisabled();
    }

    g_disabled_counts = 0;
}

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
    } else if (obj.is<Vector3f>()) {
        auto vec = obj.as<Vector3f>();
        out.x = vec.x;
        out.y = vec.y;
    } else if (obj.is<Vector4f>()) {
        auto vec = obj.as<Vector4f>();
        out.x = vec.x;
        out.y = vec.y;
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

bool button(const char* label, sol::object size_object) {
    if (label == nullptr) {
        label = "";
    }

    const auto size = create_imvec2(size_object);

    return ImGui::Button(label, size);
}

bool small_button(const char* label) {
    if (label == nullptr) {
        label = "";
    }

    return ImGui::SmallButton(label);
}

bool invisible_button(const char* id, sol::object size_object, sol::object flags_object) {
    if (id == nullptr) {
        id = "";
    }

    const auto size = create_imvec2(size_object);

    ImGuiButtonFlags flags = 0;

    if (flags_object.is<int>()) {
        flags = (ImGuiButtonFlags)(flags_object.as<int>());
    }

    return ImGui::InvisibleButton(id, size, flags);
}

bool arrow_button(const char* str_id, int dir) {
    if (str_id == nullptr) {
        str_id = "";
    }

    return ImGui::ArrowButton(str_id, (ImGuiDir)dir);
}

void text(const char* text) {
    if (text == nullptr) {
        text = "";
    }

    ImGui::TextUnformatted(text);
}

void text_colored(const char* text, unsigned int color) {
    if (text == nullptr) {
        text = "";
    }

    auto r = color & 0xFF;
    auto g = (color >> 8) & 0xFF;
    auto b = (color >> 16) & 0xFF;
    auto a = (color >> 24) & 0xFF;

    ImGui::TextColored(ImVec4{ (float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f }, text);
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
    flags |= ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CallbackAlways;

    if (label == nullptr) {
        label = "";
    }

    static std::string buffer{""};
    buffer = v;

    static int selection_start, selection_end;

    static auto input_text_callback = [](ImGuiInputTextCallbackData* data) -> int {
        if ((data->EventFlag & ImGuiInputTextFlags_CallbackResize) != 0) {
            buffer.resize(data->BufTextLen);
            data->Buf = (char*)buffer.c_str();
        }

        selection_start = data->SelectionStart;
        selection_end = data->SelectionEnd;

        return 0;
    };

    auto changed = ImGui::InputText(label, buffer.data(), buffer.capacity() + 1, flags, input_text_callback);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, std::string{buffer.data()}));
    results.push_back(sol::make_object(s, selection_start));
    results.push_back(sol::make_object(s, selection_end));

    return results;
}

sol::variadic_results input_text_multiline(sol::this_state s, const char* label, const std::string& v, sol::object size_obj, ImGuiInputTextFlags flags) {
    flags |= ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CallbackAlways;

    if (label == nullptr) {
        label = "";
    }

    static std::string buffer{""};
    buffer = v;

    static int selection_start, selection_end;

    static auto input_text_callback = [](ImGuiInputTextCallbackData* data) -> int {
        if ((data->EventFlag & ImGuiInputTextFlags_CallbackResize) != 0) {
            buffer.resize(data->BufTextLen);
            data->Buf = (char*)buffer.c_str();
        }

        selection_start = data->SelectionStart;
        selection_end = data->SelectionEnd;

        return 0;
    };

    const auto size = create_imvec2(size_obj);

    auto changed = ImGui::InputTextMultiline(label, buffer.data(), buffer.capacity() + 1, size, flags, input_text_callback);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, std::string{buffer.data()}));
    results.push_back(sol::make_object(s, selection_start));
    results.push_back(sol::make_object(s, selection_end));

    return results;
}


sol::variadic_results combo(sol::this_state s, const char* label, sol::object selection, sol::table values) {
    if (label == nullptr) {
        label = "";
    }

    const char* preview_value = "";

    if (!values.empty()) {
        if (selection.is<sol::nil_t>() || values.get_or(selection, sol::make_object(s, sol::nil)).is<sol::nil_t>()) {
            selection = (*values.begin()).first;
        }

        auto val_at_selection = values[selection].get<sol::object>();

        if (val_at_selection.is<const char*>()) {
            preview_value = val_at_selection.as<const char*>();
        }
    }

    auto selection_changed = false;

    if (ImGui::BeginCombo(label, preview_value)) {
        //for (auto i = 1u; i <= values.size(); ++i) {
        for (auto& [key, val] : values) {
            auto val_at_k = values[key].get<sol::object>();

            if (val_at_k.is<const char*>()) {
                auto entry = val_at_k.as<const char*>();

                if (ImGui::Selectable(entry, selection == key)) {
                    selection = key;
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

bool is_item_hovered(sol::object flags_obj) {
    ImGuiHoveredFlags flags{0};

    if (flags_obj.is<int>()) {
        flags = (ImGuiHoveredFlags)flags_obj.as<int>();
    }

    return ImGui::IsItemHovered(flags);
}

bool is_item_active() {
    return ImGui::IsItemActive();
}

bool is_item_focused() {
    return ImGui::IsItemFocused();
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

    const auto size = create_imvec2(size_obj);
    bool border{false};

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

    ImGui::GetWindowDrawList()->AddRect(mins, maxs, ImGui::GetColorU32(ImGuiCol_Border), ImGui::GetStyle().FrameRounding, ImDrawFlags_RoundCornersAll, 1.0f);
}

void begin_disabled(sol::object disabled_obj) {
    bool disabled{true};

    if (disabled_obj.is<bool>()) {
        disabled = disabled_obj.as<bool>();
    }

    ++g_disabled_counts;
    ImGui::BeginDisabled(disabled);
}

void end_disabled() {
    if (g_disabled_counts > 0) {
        --g_disabled_counts;
        ImGui::EndDisabled();
    }
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

    if (std::filesystem::path(filepath).is_absolute()) {
        throw std::runtime_error("Font filepath must not be absolute.");
    }

    if (std::string{filepath}.find("..") != std::string::npos) {
        throw std::runtime_error("Font filepath cannot access parent directories.");
    }

    const auto fonts_path = REFramework::get_persistent_dir() / "reframework" / "fonts";
    const auto font_path = fonts_path / filepath;

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

ImGuiID get_id(sol::object id) {
    if (id.is<int>()) {
        return id.as<ImGuiID>();
    } else if (id.is<const char*>()) {
        return ImGui::GetID(id.as<const char*>());
    } else if (id.is<void*>()) {
        return ImGui::GetID(id.as<void*>());
    } else {
        throw sol::error("Type must be int, const char* or void*");
    }

    return 0;
}

Vector2f get_mouse() {
    const auto mouse = ImGui::GetMousePos();

    return Vector2f{
        mouse.x,
        mouse.y,
    };
}

int get_key_index(int imgui_key) {
    return (ImGuiKey)imgui_key;
}

bool is_key_down(int key) {
    return ImGui::IsKeyDown((ImGuiKey)key);
}

bool is_key_pressed(int key) {
    return ImGui::IsKeyPressed((ImGuiKey)key);
}

bool is_key_released(int key) {
    return ImGui::IsKeyReleased((ImGuiKey)key);
}

bool is_mouse_down(int button) {
    return ImGui::IsMouseDown(button);
}

bool is_mouse_clicked(int button) {
    return ImGui::IsMouseClicked(button);
}

bool is_mouse_released(int button) {
    return ImGui::IsMouseReleased(button);
}

bool is_mouse_double_clicked(int button) {
    return ImGui::IsMouseDoubleClicked(button);
}

void indent(int indent_width) {
    ImGui::Indent(indent_width);
}

void unindent(int indent_width) {
    ImGui::Unindent(indent_width);
}

void begin_tooltip() {
    ImGui::BeginTooltip();
}

void end_tooltip() {
    ImGui::EndTooltip();
}

void set_tooltip(const char* text) {
    if (text == nullptr) {
        text = "";
    }

    ImGui::SetTooltip(text);
}

void open_popup(const char* str_id, sol::object flags_obj) {
    if (str_id == nullptr) {
        str_id = "";
    }

    ImGuiWindowFlags flags{0};

    if (flags_obj.is<int>()) {
        flags = (ImGuiWindowFlags)flags_obj.as<int>();
    }

    ImGui::OpenPopup(str_id, flags);
}

bool begin_popup(const char* str_id, sol::object flags_obj) {
    int flags{0};

    if (flags_obj.is<int>()) {
        flags = flags_obj.as<int>();
    }

    return ImGui::BeginPopup(str_id, flags);
}

bool begin_popup_context_item(const char* str_id, sol::object flags_obj) {
    int flags{1};

    if (flags_obj.is<int>()) {
        flags = flags_obj.as<int>();
    }

    return ImGui::BeginPopupContextItem(str_id, flags);
}

void end_popup() {
    ImGui::EndPopup();
}

void close_current_popup() {
    ImGui::CloseCurrentPopup();
}

bool is_popup_open(const char* str_id) {
    return ImGui::IsPopupOpen(str_id);
}

Vector2f calc_text_size(const char* text) {
    const auto result = ImGui::CalcTextSize(text);

    return Vector2f{
        result.x,
        result.y,
    };
}

Vector2f get_window_size() {
    const auto result = ImGui::GetWindowSize();

    return Vector2f{
        result.x,
        result.y,
    };
}

Vector2f get_window_pos() {
    const auto result = ImGui::GetWindowPos();

    return Vector2f{
        result.x,
        result.y,
    };
}

ImDrawList* get_window_draw_list() {
    return ImGui::GetWindowDrawList();
}

ImDrawList* get_background_draw_list() {
    return ImGui::GetBackgroundDrawList();
}

ImDrawList* get_foreground_draw_list() {
    return ImGui::GetForegroundDrawList();
}

void set_next_item_open(bool is_open, sol::object condition_obj) {
    ImGuiCond condition{0};

    if (condition_obj.is<int>()) {
        condition = (ImGuiCond)condition_obj.as<int>();
    }

    ImGui::SetNextItemOpen(is_open, condition);
}

bool begin_list_box(const char* label, sol::object size_obj) {
    if (label == nullptr) {
        label = "";
    }

    auto size = create_imvec2(size_obj);

    return ImGui::BeginListBox(label, size);
}

void end_list_box() {
    ImGui::EndListBox();
}

bool begin_menu_bar() {
    return ImGui::BeginMenuBar();
}

void end_menu_bar() {
    ImGui::EndMenuBar();
}

bool begin_main_menu_bar() {
    return ImGui::BeginMainMenuBar();
}

void end_main_menu_bar() {
    ImGui::EndMainMenuBar();
}

bool begin_menu(const char* label, sol::object enabled_obj) {
    if (label == nullptr) {
        label = "";
    }

    bool enabled{true};

    if (enabled_obj.is<bool>()) {
        enabled = enabled_obj.as<bool>();
    }

    return ImGui::BeginMenu(label, enabled);
}

void end_menu() {
    ImGui::EndMenu();
}

bool menu_item(const char* label, sol::object shortcut_obj, sol::object selected_obj, sol::object enabled_obj) {
    if (label == nullptr) {
        label = "";
    }

    const char* shortcut{nullptr};
    bool selected{false};
    bool enabled{true};

    if (shortcut_obj.is<const char*>()) {
        shortcut = shortcut_obj.as<const char*>();
    } else {
        shortcut = "";
    }

    if (selected_obj.is<bool>()) {
        selected = selected_obj.as<bool>();
    }

    if (enabled_obj.is<bool>()) {
        enabled = enabled_obj.as<bool>();
    }

    return ImGui::MenuItem(label, shortcut, selected, enabled);
}

Vector2f get_display_size() {
    const auto& result = ImGui::GetIO().DisplaySize;

    return Vector2f{
        result.x,
        result.y,
    };
}

void push_item_width(float item_width) {
    ImGui::PushItemWidth(item_width);
}

void pop_item_width() {
    ImGui::PopItemWidth();
}

void set_next_item_width(float item_width) {
    ImGui::SetNextItemWidth(item_width);
}

float calc_item_width() {
    return ImGui::CalcItemWidth();
}

void item_size(sol::object pos, sol::object size, sol::object text_baseline_y) {
    if (text_baseline_y.is<float>()) {
        ImGui::ItemSize(ImRect{create_imvec2(pos), create_imvec2(size)}, text_baseline_y.as<float>());
    } else {
        ImGui::ItemSize(ImRect{create_imvec2(pos), create_imvec2(size)});
    }
}

bool item_add(const char* label, sol::object pos, sol::object size) {
    if (label == nullptr) {
        label = "";
    }

    const auto window = ImGui::GetCurrentWindow();

    if (window == nullptr) {
        return false;
    }

    return ImGui::ItemAdd(ImRect{create_imvec2(pos), create_imvec2(size)}, window->GetID(label));
}

void push_style_color(int style_color, sol::object color_obj) {
    if (color_obj.is<int>()) {
        ImGui::PushStyleColor((ImGuiCol)style_color, (ImU32)color_obj.as<int>());
    } else if (color_obj.is<Vector4f>()) {
        ImGui::PushStyleColor((ImGuiCol)style_color, create_imvec4(color_obj));
    }
}

void pop_style_color(sol::object count_obj) {
    int count{1};

    if (count_obj.is<int>()) {
        count = count_obj.as<int>();
    }

    ImGui::PopStyleColor(count);
}

void push_style_var(int idx, sol::object value_obj) {
    if (value_obj.is<float>()) {
        ImGui::PushStyleVar((ImGuiStyleVar)idx, value_obj.as<float>());
    } else if (value_obj.is<Vector2f>()) {
        ImGui::PushStyleVar((ImGuiStyleVar)idx, create_imvec2(value_obj));
    }
}

void pop_style_var(sol::object count_obj) {
    int count{1};

    if (count_obj.is<int>()) {
        count = count_obj.as<int>();
    }

    ImGui::PopStyleVar(count);
}

Vector2f get_cursor_pos() {
    const auto result = ImGui::GetCursorPos();

    return Vector2f{
        result.x,
        result.y,
    };
}

void set_cursor_pos(sol::object pos) {
    ImGui::SetCursorPos(create_imvec2(pos));
}

Vector2f get_cursor_start_pos() {
    const auto result = ImGui::GetCursorStartPos();

    return Vector2f{
        result.x,
        result.y,
    };
}

Vector2f get_cursor_screen_pos() {
    const auto result = ImGui::GetCursorScreenPos();

    return Vector2f{
        result.x,
        result.y,
    };
}

void set_cursor_screen_pos(sol::object pos) {
    ImGui::SetCursorScreenPos(create_imvec2(pos));
}

void set_item_default_focus() {
    ImGui::SetItemDefaultFocus();
}

void set_clipboard(sol::object data) {
    ImGui::SetClipboardText(data.as<const char*>());
}

const char* get_clipboard() {
    return ImGui::GetClipboardText();
}

void progress_bar(float progress, sol::object size, const char* overlay ){
    if (overlay == nullptr) {
        overlay = "";
    }

    ImGui::ProgressBar(progress, create_imvec2(size), overlay);
}

bool begin_table(const char* str_id, int column, sol::object flags_obj, sol::object outer_size_obj, sol::object inner_width_obj) {
    if (str_id == nullptr) {
        str_id = "";
    }

    ImVec2 outer_size{};
    if (outer_size_obj.is<Vector2f>()) {
        const auto& vec = outer_size_obj.as<Vector2f>();
        outer_size = {vec.x, vec.y};
    }

    float inner_width = inner_width_obj.is<float>() ? inner_width_obj.as<float>() : 0.0f;
    int flags = flags_obj.is<int>() ? flags_obj.as<int>() : 0;

    return ImGui::BeginTable(str_id, column, flags, outer_size, inner_width);
}

void end_table() {
    ImGui::EndTable();
}

void table_next_row(sol::object row_flags, sol::object min_row_height) {
    int flags = row_flags.is<int>() ? row_flags.as<int>() : 0;
    float min_height = min_row_height.is<float>() ? min_row_height.as<float>() : 0.0f;
    ImGui::TableNextRow(flags, min_height);
}

bool table_next_column() {
    return ImGui::TableNextColumn();
}

bool table_set_column_index(int column_index) {
    return ImGui::TableSetColumnIndex(column_index);
}

void table_setup_column(const char* label, sol::object flags_obj, sol::object init_width_or_weight_obj, sol::object user_id_obj) {
    if (label == nullptr) {
        label = "";
    }

    auto flags = flags_obj.is<int>() ? flags_obj.as<int>() : 0;
    auto init_width = init_width_or_weight_obj.is<float>() ? init_width_or_weight_obj.as<float>() : 0.0f;
    auto user_id = user_id_obj.is<ImGuiID>() ? user_id_obj.as<ImGuiID>() : 0;

    ImGui::TableSetupColumn(label, flags, init_width, user_id);
}

void table_setup_scroll_freeze(int cols, int rows) {
    ImGui::TableSetupScrollFreeze(cols, rows);
}

void table_headers_row() {
    ImGui::TableHeadersRow();
}

void table_header(const char* label) {
    if (label == nullptr) {
        label = "";
    }

    ImGui::TableHeader(label);
}

int table_get_column_count() {
    return ImGui::TableGetColumnCount();
}

int table_get_column_index() {
    return ImGui::TableGetColumnIndex();
}

int table_get_row_index() {
    return ImGui::TableGetRowIndex();
}

const char* table_get_column_name(int column = -1) {
    return ImGui::TableGetColumnName(column);
}

ImGuiTableColumnFlags table_get_column_flags(sol::object column) {
    return ImGui::TableGetColumnFlags(column.is<int>() ? column.as<int>() : -1);
}

void table_set_bg_color(ImGuiTableBgTarget target, ImU32 color, sol::object column) {
    ImGui::TableSetBgColor(target, color, column.is<int>() ? column.as<int>() : -1);
}

void table_set_bg_color_vec4(ImGuiTableBgTarget target, Vector4f color, sol::object column) {
    ImVec4 _color = {color.x, color.y, color.z, color.w};
    ImGui::TableSetBgColor(target, ImGui::ColorConvertFloat4ToU32(_color), column.is<int>() ? column.as<int>() : -1);
}

ImGuiTableSortSpecs* table_get_sort_specs() {
    return ImGui::TableGetSortSpecs();
}

// Window Drawlist
void draw_list_path_clear() {
    if (auto dl = ImGui::GetWindowDrawList(); dl != nullptr) {
        dl->PathClear();
    }
}

void draw_list_path_line_to(sol::object pos_obj) {
    auto pos = create_imvec2(pos_obj);
    if (auto dl = ImGui::GetWindowDrawList(); dl != nullptr) {
        dl->PathLineTo(pos);
    }
}

void draw_list_path_stroke(ImU32 color, bool closed, float thickness) {
    if (auto dl = ImGui::GetWindowDrawList(); dl != nullptr) {
        dl->PathStroke(color, closed, thickness);
    }
}
} // namespace api::imgui

// Scroll APIs
namespace api::imgui {
float get_scroll_x() {
    return ImGui::GetScrollX();
}

float get_scroll_y() {
    return ImGui::GetScrollY();
}

void set_scroll_x(float scroll_x) {
    ImGui::SetScrollX(scroll_x);
}

void set_scroll_y(float scroll_y) {
    ImGui::SetScrollY(scroll_y);
}

float get_scroll_max_x() {
    return ImGui::GetScrollMaxX();
}

float get_scroll_max_y() {
    return ImGui::GetScrollMaxY();
}

void set_scroll_here_x(float center_x_ratio = 0.5f) {
    ImGui::SetScrollHereX(center_x_ratio);
}

void set_scroll_here_y(float center_y_ratio = 0.5f) {
    ImGui::SetScrollHereY(center_y_ratio);
}

void set_scroll_from_pos_x(float local_x, float center_x_ratio = 0.5f) {
    ImGui::SetScrollFromPosX(local_x, center_x_ratio);
}

void set_scroll_from_pos_y(float local_y, float center_y_ratio = 0.5f) {
    ImGui::SetScrollFromPosY(local_y, center_y_ratio);
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

    camera_origin.w = 1.0f;

    Vector4f camera_forward{};
    get_axisz_method->call<void*>(&camera_forward, context, camera_transform);

    camera_forward.w = 1.0f;

    // Translate 2d position to 3d position (screen to world)
    Matrix4x4f proj{}, view{};
    float screen_size[2]{};
    sdk::call_object_func<void*>(camera, "get_ProjectionMatrix", &proj, context, camera);
    sdk::call_object_func<void*>(camera, "get_ViewMatrix", &view, context, camera);
    sdk::call_object_func<void*>(main_view, "get_WindowSize", &screen_size, context, main_view);

    Vector4f screen_pos{};

    const auto delta = world_pos - camera_origin;

    // behind camera
    if (glm::dot(Vector3f{delta}, Vector3f{-camera_forward}) <= 0.0f) {
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

namespace api::imnodes {
int32_t g_node_editor_counts{0};
int32_t g_node_counts{};
int32_t g_node_titlebar_counts{0};
int32_t g_color_style_counts{};
int32_t g_color_style_var_counts{};
int32_t g_input_attribute_counts{0};
int32_t g_output_attribute_counts{0};
int32_t g_static_attribute_counts{0};

bool is_editor_hovered() {
    return ImNodes::IsEditorHovered();
}

sol::variadic_results is_node_hovered(sol::this_state s) {
    int id{};
    const auto result = ImNodes::IsNodeHovered(&id);

    sol::variadic_results results{};
    results.push_back(sol::make_object<bool>(s, result));
    results.push_back(sol::make_object<int>(s, id));

    return results;
}

sol::variadic_results is_link_hovered(sol::this_state s) {
    int id{};
    const auto result = ImNodes::IsLinkHovered(&id);

    sol::variadic_results results{};
    results.push_back(sol::make_object<bool>(s, result));
    results.push_back(sol::make_object<int>(s, id));

    return results;
}

sol::variadic_results is_pin_hovered(sol::this_state s) {
    int id{};
    const auto result = ImNodes::IsPinHovered(&id);

    sol::variadic_results results{};
    results.push_back(sol::make_object<bool>(s, result));
    results.push_back(sol::make_object<int>(s, id));

    return results;
}

void begin_node_editor() {
    ImNodes::BeginNodeEditor();
    ++g_node_editor_counts;
}

void end_node_editor() {
    ImNodes::EndNodeEditor();
    --g_node_editor_counts;
}

void begin_node(int id) {
    ImNodes::BeginNode(id);
    ++g_node_counts;
}

void end_node() {
    ImNodes::EndNode();
    --g_node_counts;
}

void begin_node_titlebar() {
    ImNodes::BeginNodeTitleBar();
    ++g_node_titlebar_counts;
}

void end_node_titlebar() {
    ImNodes::EndNodeTitleBar();
    --g_node_titlebar_counts;
}

void begin_input_attribute(int attr, ImNodesPinShape shape) {
    ImNodes::BeginInputAttribute(attr, shape);
    ++g_input_attribute_counts;
}

void end_input_attribute() {
    ImNodes::EndInputAttribute();
    --g_input_attribute_counts;
}

void begin_output_attribute(int attr, ImNodesPinShape shape) {
    ImNodes::BeginOutputAttribute(attr, shape);
    ++g_output_attribute_counts;
}

void end_output_attribute() {
    ImNodes::EndOutputAttribute();
    --g_output_attribute_counts;
}

void begin_static_attribute(int attr) {
    ImNodes::BeginStaticAttribute(attr);
    ++g_static_attribute_counts;
}

void end_static_attribute() {
    ImNodes::EndStaticAttribute();
    --g_static_attribute_counts;
}

void minimap(float size_fraction, const ImNodesMiniMapLocation location) {
    ImNodes::MiniMap(size_fraction, location);
}

void link(int id, int start_attr_id, int end_attr_id) {
    ImNodes::Link(id, start_attr_id, end_attr_id);
}

Vector2f get_node_dimensions(int id) {
    const auto result = ImNodes::GetNodeDimensions(id);

    return Vector2f{
        result.x,
        result.y,
    };
}

void push_color_style(ImNodesCol item, uint32_t color) {
    ImNodes::PushColorStyle(item, color);
    ++g_color_style_counts;
}

void pop_color_style() {
    ImNodes::PopColorStyle();
    --g_color_style_counts;
}

void push_style_var(ImNodesStyleVar item, float value) {
    ImNodes::PushStyleVar(item, value);
    ++g_color_style_var_counts;
}

void push_style_var_vec2(ImNodesStyleVar item, float x, float y) {
    ImNodes::PushStyleVar(item, ImVec2{x, y});
    ++g_color_style_var_counts;
}

void pop_style_var() {
    ImNodes::PopStyleVar();
    --g_color_style_var_counts;
}

void set_node_screen_space_pos(int id, float x, float y) {
    ImNodes::SetNodeScreenSpacePos(id, ImVec2{x, y});
}

void set_node_editor_space_pos(int id, float x, float y) {
    ImNodes::SetNodeEditorSpacePos(id, ImVec2{x, y});
}

void set_node_grid_space_pos(int id, float x, float y) {
    ImNodes::SetNodeGridSpacePos(id, ImVec2{x, y});
}

Vector2f get_node_screen_space_pos(int id) {
    const auto result = ImNodes::GetNodeScreenSpacePos(id);

    return Vector2f{
        result.x,
        result.y,
    };
}

Vector2f get_node_editor_space_pos(int id) {
    const auto result = ImNodes::GetNodeEditorSpacePos(id);

    return Vector2f{
        result.x,
        result.y,
    };
}

Vector2f get_node_grid_space_pos(int id) {
    const auto result = ImNodes::GetNodeGridSpacePos(id);

    return Vector2f{
        result.x,
        result.y,
    };
}

void clear_node_selection(sol::object id_object) {
    if (id_object.is<sol::nil_t>()) {
        ImNodes::ClearNodeSelection();
    } else if (id_object.is<int>()) {
        ImNodes::ClearNodeSelection(id_object.as<int>());
    } else {
        throw sol::error("id_object must be nil or int");
    }
}

void clear_link_selection(sol::object id_object) {
    if (id_object.is<sol::nil_t>()) {
        ImNodes::ClearLinkSelection();
    } else if (id_object.is<int>()) {
        ImNodes::ClearLinkSelection(id_object.as<int>());
    } else {
        throw sol::error("id_object must be nil or int");
    }
}

void editor_reset_panning(float x, float y) {
    ImNodes::EditorContextResetPanning(ImVec2{x, y});
}

Vector2f editor_get_panning() {
    const auto result = ImNodes::EditorContextGetPanning();

    return Vector2f{
        result.x,
        result.y,
    };
}

sol::variadic_results is_link_started(sol::this_state s) {
    sol::variadic_results results{};

    int id{};
    bool result = ImNodes::IsLinkStarted(&id);

    results.push_back(sol::make_object<bool>(s, result));
    results.push_back(sol::make_object<int>(s, id));

    return results;
}

sol::variadic_results is_link_dropped(sol::this_state s, bool including_detached_links) {
    sol::variadic_results results{};

    int id{};
    bool result = ImNodes::IsLinkDropped(&id, including_detached_links);

    results.push_back(sol::make_object<bool>(s, result));
    results.push_back(sol::make_object<int>(s, id));

    return results;
}

sol::variadic_results is_link_destroyed(sol::this_state s) {
    sol::variadic_results results{};

    int id{};
    bool result = ImNodes::IsLinkDestroyed(&id);

    results.push_back(sol::make_object<bool>(s, result));
    results.push_back(sol::make_object<int>(s, id));

    return results;
}

sol::variadic_results is_link_created(sol::this_state s) {
    sol::variadic_results results{};

    int started_at_node{};
    int started_at_attr{};
    int ended_at_node{};
    int ended_at_attr{};
    bool created_from_snap{};

    bool result = ImNodes::IsLinkCreated(
        &started_at_node,
        &started_at_attr,
        &ended_at_node,
        &ended_at_attr,
        &created_from_snap);

    results.push_back(sol::make_object<bool>(s, result));
    results.push_back(sol::make_object<int>(s, started_at_node));
    results.push_back(sol::make_object<int>(s, started_at_attr));
    results.push_back(sol::make_object<int>(s, ended_at_node));
    results.push_back(sol::make_object<int>(s, ended_at_attr));
    results.push_back(sol::make_object<bool>(s, created_from_snap));

    return results;
}

std::vector<int> get_selected_nodes() {
    std::vector<int> out{};

    const auto num_selected_nodes = ImNodes::NumSelectedNodes();

    if (num_selected_nodes > 0) {
        out.resize(num_selected_nodes);
        ImNodes::GetSelectedNodes(out.data());
    }

    return out;
}

std::vector<int> get_selected_links() {
    std::vector<int> out{};

    const auto num_selected_links = ImNodes::NumSelectedLinks();

    if (num_selected_links > 0) {
        out.resize(num_selected_links);
        ImNodes::GetSelectedLinks(out.data());
    }

    return out;
}


void cleanup() {
    for (auto i = 0; i < g_node_editor_counts; ++i) {
        end_node_editor();
    }

    g_node_editor_counts = 0;

    for (auto i = 0; i < g_node_counts; ++i) {
        end_node();
    }

    g_node_counts = 0;

    for (auto i = 0; i < g_node_titlebar_counts; ++i) {
        end_node_titlebar();
    }

    g_node_titlebar_counts = 0;

    for (auto i = 0; i < g_color_style_counts; ++i) {
        pop_color_style();
    }

    g_color_style_counts = 0;

    for (auto i = 0; i < g_color_style_var_counts; ++i) {
        pop_style_var();
    }

    g_color_style_var_counts = 0;

    for (auto i = 0; i < g_input_attribute_counts; ++i) {
        end_input_attribute();
    }

    g_input_attribute_counts = 0;

    for (auto i = 0; i < g_output_attribute_counts; ++i) {
        end_output_attribute();
    }

    g_output_attribute_counts = 0;

    for (auto i = 0; i < g_static_attribute_counts; ++i) {
        end_static_attribute();
    }

    g_static_attribute_counts = 0;
}
} // namespace imnodes

void bindings::open_imgui(ScriptState* s) {
    auto& lua = s->lua();
    auto imgui = lua.create_table();

    imgui["button"] = api::imgui::button;
    imgui["small_button"] = api::imgui::small_button;
    imgui["invisible_button"] = api::imgui::invisible_button;
    imgui["arrow_button"] = api::imgui::arrow_button;
    imgui["combo"] = api::imgui::combo;
    imgui["drag_float"] = api::imgui::drag_float;
    imgui["drag_float2"] = api::imgui::drag_float2;
    imgui["drag_float3"] = api::imgui::drag_float3;
    imgui["drag_float4"] = api::imgui::drag_float4;
    imgui["drag_int"] = api::imgui::drag_int;
    imgui["slider_float"] = api::imgui::slider_float;
    imgui["slider_int"] = api::imgui::slider_int;
    imgui["input_text"] = api::imgui::input_text;
    imgui["input_text_multiline"] = api::imgui::input_text_multiline;
    imgui["text"] = api::imgui::text;
    imgui["text_colored"] = api::imgui::text_colored;
    imgui["checkbox"] = api::imgui::checkbox;
    imgui["tree_node"] = api::imgui::tree_node;
    imgui["tree_node_ptr_id"] = api::imgui::tree_node_ptr_id;
    imgui["tree_node_str_id"] = api::imgui::tree_node_str_id;
    imgui["tree_pop"] = api::imgui::tree_pop;
    imgui["same_line"] = api::imgui::same_line;
    imgui["is_item_hovered"] = api::imgui::is_item_hovered;
    imgui["is_item_active"] = api::imgui::is_item_active;
    imgui["is_item_focused"] = api::imgui::is_item_focused;
    imgui["begin_window"] = api::imgui::begin_window;
    imgui["end_window"] = api::imgui::end_window;
    imgui["begin_child_window"] = api::imgui::begin_child_window;
    imgui["end_child_window"] = api::imgui::end_child_window;
    imgui["begin_group"] = api::imgui::begin_group;
    imgui["end_group"] = api::imgui::end_group;
    imgui["begin_rect"] = api::imgui::begin_rect;
    imgui["end_rect"] = api::imgui::end_rect;
    imgui["begin_disabled"] = api::imgui::begin_disabled;
    imgui["end_disabled"] = api::imgui::end_disabled;
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
    imgui["get_id"] = api::imgui::get_id;
    imgui["get_mouse"] = api::imgui::get_mouse;
    imgui["get_key_index"] = api::imgui::get_key_index;
    imgui["is_key_down"] = api::imgui::is_key_down;
    imgui["is_key_pressed"] = api::imgui::is_key_pressed;
    imgui["is_key_released"] = api::imgui::is_key_released;
    imgui["is_mouse_down"] = api::imgui::is_mouse_down;
    imgui["is_mouse_clicked"] = api::imgui::is_mouse_clicked;
    imgui["is_mouse_released"] = api::imgui::is_mouse_released;
    imgui["is_mouse_double_clicked"] = api::imgui::is_mouse_double_clicked;
    imgui["indent"] = api::imgui::indent;
    imgui["unindent"] = api::imgui::unindent;
    imgui["begin_tooltip"] = api::imgui::begin_tooltip;
    imgui["end_tooltip"] = api::imgui::end_tooltip;
    imgui["set_tooltip"] = api::imgui::set_tooltip;
    imgui["open_popup"] = api::imgui::open_popup;
    imgui["begin_popup"] = api::imgui::begin_popup;
    imgui["begin_popup_context_item"] = api::imgui::begin_popup_context_item;
    imgui["end_popup"] = api::imgui::end_popup;
    imgui["close_current_popup"] = api::imgui::close_current_popup;
    imgui["is_popup_open"] = api::imgui::is_popup_open;
    imgui["calc_text_size"] = api::imgui::calc_text_size;
    imgui["get_window_size"] = api::imgui::get_window_size;
    imgui["get_window_pos"] = api::imgui::get_window_pos;
    imgui["set_next_item_open"] = api::imgui::set_next_item_open;
    imgui["begin_list_box"] = api::imgui::begin_list_box;
    imgui["end_list_box"] = api::imgui::end_list_box;
    imgui["begin_menu_bar"] = api::imgui::begin_menu_bar;
    imgui["end_menu_bar"] = api::imgui::end_menu_bar;
    imgui["begin_main_menu_bar"] = api::imgui::begin_main_menu_bar;
    imgui["end_main_menu_bar"] = api::imgui::end_main_menu_bar;
    imgui["begin_menu"] = api::imgui::begin_menu;
    imgui["end_menu"] = api::imgui::end_menu;
    imgui["menu_item"] = api::imgui::menu_item;
    imgui["get_display_size"] = api::imgui::get_display_size;
    imgui["get_window_draw_list"] = api::imgui::get_window_draw_list;
    imgui["get_background_draw_list"] = api::imgui::get_background_draw_list;
    imgui["get_foreground_draw_list"] = api::imgui::get_foreground_draw_list;

    // Item
    imgui["push_item_width"] = api::imgui::push_item_width;
    imgui["pop_item_width"] = api::imgui::pop_item_width;
    imgui["set_next_item_width"] = api::imgui::set_next_item_width;
    imgui["calc_item_width"] = api::imgui::calc_item_width;
    imgui["item_add"] = api::imgui::item_add;
    imgui["item_size"] = api::imgui::item_size;

    imgui["push_style_color"] = api::imgui::push_style_color;
    imgui["pop_style_color"] = api::imgui::pop_style_color;
    imgui["push_style_var"] = api::imgui::push_style_var;
    imgui["pop_style_var"] = api::imgui::pop_style_var;
    imgui["get_cursor_pos"] = api::imgui::get_cursor_pos;
    imgui["set_cursor_pos"] = api::imgui::set_cursor_pos;
    imgui["get_cursor_start_pos"] = api::imgui::get_cursor_start_pos;
    imgui["get_cursor_screen_pos"] = api::imgui::get_cursor_screen_pos;
    imgui["set_cursor_screen_pos"] = api::imgui::set_cursor_screen_pos;
    imgui["set_item_default_focus"] = api::imgui::set_item_default_focus;
    imgui["set_clipboard"] = api::imgui::set_clipboard;
    imgui["get_clipboard"] = api::imgui::get_clipboard;
    imgui["progress_bar"] = api::imgui::progress_bar;

    // Draw list
    imgui["draw_list_path_clear"] = api::imgui::draw_list_path_clear;
    imgui["draw_list_path_line_to"] = api::imgui::draw_list_path_line_to;
    imgui["draw_list_path_stroke"] = api::imgui::draw_list_path_stroke;
    
    // SCROLL APIs
    imgui["get_scroll_x"] = api::imgui::get_scroll_x;
    imgui["get_scroll_y"] = api::imgui::get_scroll_y;
    imgui["set_scroll_x"] = api::imgui::set_scroll_x;
    imgui["set_scroll_y"] = api::imgui::set_scroll_y;
    imgui["get_scroll_max_x"] = api::imgui::get_scroll_max_x;
    imgui["get_scroll_max_y"] = api::imgui::get_scroll_max_y;
    imgui["set_scroll_here_x"] = api::imgui::set_scroll_here_x;
    imgui["set_scroll_here_y"] = api::imgui::set_scroll_here_y;
    imgui["set_scroll_from_pos_x"] = api::imgui::set_scroll_from_pos_x;
    imgui["set_scroll_from_pos_y"] = api::imgui::set_scroll_from_pos_y;


    // TABLE APIS
    imgui["begin_table"] = api::imgui::begin_table;
    imgui["end_table"] = api::imgui::end_table;
    imgui["table_next_row"] = api::imgui::table_next_row;
    imgui["table_next_column"] = api::imgui::table_next_column;
    imgui["table_set_column_index"] = api::imgui::table_set_column_index;
    imgui["table_setup_column"] = api::imgui::table_setup_column;
    imgui["table_setup_scroll_freeze"] = api::imgui::table_setup_scroll_freeze;
    imgui["table_headers_row"] = api::imgui::table_headers_row;
    imgui["table_header"] = api::imgui::table_header;
    imgui["table_get_sort_specs"] = api::imgui::table_get_sort_specs;
    imgui["table_get_column_count"] = api::imgui::table_get_column_count;
    imgui["table_get_column_index"] = api::imgui::table_get_column_index;
    imgui["table_get_row_index"] = api::imgui::table_get_row_index;
    imgui["table_get_column_name"] = api::imgui::table_get_column_name;
    imgui["table_get_column_flags"] = api::imgui::table_get_column_flags;
    imgui["table_set_bg_color"] = api::imgui::table_set_bg_color;
    imgui.new_enum("ImGuizmoOperation", 
                    "TRANSLATE", ImGuizmo::OPERATION::TRANSLATE, 
                    "ROTATE", ImGuizmo::OPERATION::ROTATE,
                    "SCALE", ImGuizmo::OPERATION::SCALE,
                    "SCALEU", ImGuizmo::OPERATION::SCALEU,
                    "UNIVERSAL", ImGuizmo::OPERATION::UNIVERSAL);
    imgui.new_enum("ImGuizmoMode", 
                    "WORLD", ImGuizmo::MODE::WORLD,
                    "LOCAL", ImGuizmo::MODE::LOCAL);
    imgui.new_usertype<ImGuiTableSortSpecs>("TableSortSpecs",
        "specs_dirty", &ImGuiTableSortSpecs::SpecsDirty,
        "get_specs", [](ImGuiTableSortSpecs* specs) {
            std::vector<ImGuiTableColumnSortSpecs*> out {};

            for (int i = 0; i < specs->SpecsCount; ++i) {
                out.push_back(const_cast<ImGuiTableColumnSortSpecs*>(specs->Specs + i));
            }

            return out;
        }
    );
    imgui.new_usertype<ImGuiTableColumnSortSpecs>("TableColumnSortSpecs", 
        "user_id", &ImGuiTableColumnSortSpecs::ColumnUserID,
        "column_index", &ImGuiTableColumnSortSpecs::ColumnIndex,
        "sort_order", &ImGuiTableColumnSortSpecs::SortOrder,
        "sort_direction", sol::readonly_property([](ImGuiTableColumnSortSpecs* specs){
            return specs->SortDirection;
        })
    );
    imgui.new_enum("TableFlags",
        "None", ImGuiTableFlags_None,
        "Resizable", ImGuiTableFlags_Resizable,
        "Reorderable", ImGuiTableFlags_Reorderable,
        "Hideable", ImGuiTableFlags_Hideable,
        "Sortable", ImGuiTableFlags_Sortable,
        "NoSavedSettings", ImGuiTableFlags_NoSavedSettings,
        "ContextMenuInBody", ImGuiTableFlags_ContextMenuInBody,
        "RowBg", ImGuiTableFlags_RowBg,
        "BordersInnerH", ImGuiTableFlags_BordersInnerH,
        "BordersOuterH", ImGuiTableFlags_BordersOuterH,
        "BordersInnerV", ImGuiTableFlags_BordersInnerV,
        "BordersOuterV", ImGuiTableFlags_BordersOuterV,
        "BordersH", ImGuiTableFlags_BordersH,
        "BordersV", ImGuiTableFlags_BordersV,
        "BordersInner", ImGuiTableFlags_BordersInner,
        "BordersOuter", ImGuiTableFlags_BordersOuter,
        "Borders", ImGuiTableFlags_Borders,
        "NoBordersInBody", ImGuiTableFlags_NoBordersInBody,
        "NoBordersInBodyUntilResize", ImGuiTableFlags_NoBordersInBodyUntilResize,
        "SizingFixedFit", ImGuiTableFlags_SizingFixedFit,
        "SizingFixedSame", ImGuiTableFlags_SizingFixedSame,
        "SizingStretchProp", ImGuiTableFlags_SizingStretchProp,
        "SizingStretchSame", ImGuiTableFlags_SizingStretchSame,
        "NoHostExtendX", ImGuiTableFlags_NoHostExtendX,
        "NoHostExtendY", ImGuiTableFlags_NoHostExtendY,
        "NoKeepColumnsVisible", ImGuiTableFlags_NoKeepColumnsVisible,
        "PreciseWidths", ImGuiTableFlags_PreciseWidths,
        "NoClip", ImGuiTableFlags_NoClip,
        "PadOuterX", ImGuiTableFlags_PadOuterX,
        "NoPadOuterX", ImGuiTableFlags_NoPadOuterX,
        "NoPadInnerX", ImGuiTableFlags_NoPadInnerX,
        "ScrollX", ImGuiTableFlags_ScrollX,
        "ScrollY", ImGuiTableFlags_ScrollY,
        "SortMulti", ImGuiTableFlags_SortMulti,
        "SortTristate", ImGuiTableFlags_SortTristate
    );
    imgui.new_enum("ColumnFlags",
        "None", ImGuiTableColumnFlags_None,                
        "DefaultHide", ImGuiTableColumnFlags_DefaultHide,         
        "DefaultSort", ImGuiTableColumnFlags_DefaultSort,         
        "WidthStretch", ImGuiTableColumnFlags_WidthStretch,        
        "WidthFixed", ImGuiTableColumnFlags_WidthFixed,          
        "NoResize", ImGuiTableColumnFlags_NoResize,            
        "NoReorder", ImGuiTableColumnFlags_NoReorder,           
        "NoHide", ImGuiTableColumnFlags_NoHide,              
        "NoClip", ImGuiTableColumnFlags_NoClip,              
        "NoSort", ImGuiTableColumnFlags_NoSort,              
        "NoSortAscending", ImGuiTableColumnFlags_NoSortAscending,     
        "NoSortDescending", ImGuiTableColumnFlags_NoSortDescending,    
        "NoHeaderWidth", ImGuiTableColumnFlags_NoHeaderWidth,       
        "PreferSortAscending", ImGuiTableColumnFlags_PreferSortAscending, 
        "PreferSortDescending", ImGuiTableColumnFlags_PreferSortDescending,
        "IndentEnable", ImGuiTableColumnFlags_IndentEnable,        
        "IndentDisable", ImGuiTableColumnFlags_IndentDisable,       
        "IsEnabled", ImGuiTableColumnFlags_IsEnabled,           
        "IsVisible", ImGuiTableColumnFlags_IsVisible,           
        "IsSorted", ImGuiTableColumnFlags_IsSorted,            
        "IsHovered", ImGuiTableColumnFlags_IsHovered           
    );

    imgui.new_enum("ImGuiKey",
        
        // Keys
        "Key_None", ImGuiKey_None,
        "Key_Tab", ImGuiKey_Tab,
        "Key_LeftArrow", ImGuiKey_LeftArrow,
        "Key_RightArrow", ImGuiKey_RightArrow,
        "Key_UpArrow", ImGuiKey_UpArrow,
        "Key_DownArrow", ImGuiKey_DownArrow,
        "Key_PageUp", ImGuiKey_PageUp,
        "Key_PageDown", ImGuiKey_PageDown,
        "Key_Home", ImGuiKey_Home,
        "Key_End", ImGuiKey_End,
        "Key_Insert", ImGuiKey_Insert,
        "Key_Delete", ImGuiKey_Delete,
        "Key_Backspace", ImGuiKey_Backspace,
        "Key_Space", ImGuiKey_Space,
        "Key_Enter", ImGuiKey_Enter,
        "Key_Escape", ImGuiKey_Escape,
        "Key_LeftCtrl", ImGuiKey_LeftCtrl,
        "Key_LeftShift", ImGuiKey_LeftShift,
        "Key_LeftAlt", ImGuiKey_LeftAlt,
        "Key_LeftSuper", ImGuiKey_LeftSuper,
        "Key_RightCtrl", ImGuiKey_RightCtrl,
        "Key_RightShift", ImGuiKey_RightShift,
        "Key_RightAlt", ImGuiKey_RightAlt,
        "Key_RightSuper", ImGuiKey_RightSuper,
        "Key_Menu", ImGuiKey_Menu,
        "Key_0", ImGuiKey_0,
        "Key_1", ImGuiKey_1,
        "Key_2", ImGuiKey_2,
        "Key_3", ImGuiKey_3,
        "Key_4", ImGuiKey_4,
        "Key_5", ImGuiKey_5,
        "Key_6", ImGuiKey_6,
        "Key_7", ImGuiKey_7,
        "Key_8", ImGuiKey_8,
        "Key_9", ImGuiKey_9,
        "Key_A", ImGuiKey_A,
        "Key_B", ImGuiKey_B,
        "Key_C", ImGuiKey_C,
        "Key_D", ImGuiKey_D,
        "Key_E", ImGuiKey_E,
        "Key_F", ImGuiKey_F,
        "Key_G", ImGuiKey_G,
        "Key_H", ImGuiKey_H,
        "Key_I", ImGuiKey_I,
        "Key_J", ImGuiKey_J,
        "Key_K", ImGuiKey_K,
        "Key_L", ImGuiKey_L,
        "Key_M", ImGuiKey_M,
        "Key_N", ImGuiKey_N,
        "Key_O", ImGuiKey_O,
        "Key_P", ImGuiKey_P,
        "Key_Q", ImGuiKey_Q,
        "Key_R", ImGuiKey_R,
        "Key_S", ImGuiKey_S,
        "Key_T", ImGuiKey_T,
        "Key_U", ImGuiKey_U,
        "Key_V", ImGuiKey_V,
        "Key_W", ImGuiKey_W,
        "Key_X", ImGuiKey_X,
        "Key_Y", ImGuiKey_Y,
        "Key_Z", ImGuiKey_Z,
        "Key_F1", ImGuiKey_F1,
        "Key_F2", ImGuiKey_F2,
        "Key_F3", ImGuiKey_F3,
        "Key_F4", ImGuiKey_F4,
        "Key_F5", ImGuiKey_F5,
        "Key_F6", ImGuiKey_F6,
        "Key_F7", ImGuiKey_F7,
        "Key_F8", ImGuiKey_F8,
        "Key_F9", ImGuiKey_F9,
        "Key_F10", ImGuiKey_F10,
        "Key_F11", ImGuiKey_F11,
        "Key_F12", ImGuiKey_F12,
        "Key_F13", ImGuiKey_F13,
        "Key_F14", ImGuiKey_F14,
        "Key_F15", ImGuiKey_F15,
        "Key_F16", ImGuiKey_F16,
        "Key_F17", ImGuiKey_F17,
        "Key_F18", ImGuiKey_F18,
        "Key_F19", ImGuiKey_F19,
        "Key_F20", ImGuiKey_F20,
        "Key_F21", ImGuiKey_F21,
        "Key_F22", ImGuiKey_F22,
        "Key_F23", ImGuiKey_F23,
        "Key_F24", ImGuiKey_F24,
        "Key_Apostrophe", ImGuiKey_Apostrophe,
        "Key_Comma", ImGuiKey_Comma,
        "Key_Minus", ImGuiKey_Minus,
        "Key_Period", ImGuiKey_Period,
        "Key_Slash", ImGuiKey_Slash,
        "Key_Semicolon", ImGuiKey_Semicolon,
        "Key_Equal", ImGuiKey_Equal,
        "Key_LeftBracket", ImGuiKey_LeftBracket,
        "Key_Backslash", ImGuiKey_Backslash,
        "Key_RightBracket", ImGuiKey_RightBracket,
        "Key_GraveAccent", ImGuiKey_GraveAccent,
        "Key_CapsLock", ImGuiKey_CapsLock,
        "Key_ScrollLock", ImGuiKey_ScrollLock,
        "Key_NumLock", ImGuiKey_NumLock,
        "Key_PrintScreen", ImGuiKey_PrintScreen,
        "Key_Pause", ImGuiKey_Pause,
        "Key_Keypad0", ImGuiKey_Keypad0,
        "Key_Keypad1", ImGuiKey_Keypad1,
        "Key_Keypad2", ImGuiKey_Keypad2,
        "Key_Keypad3", ImGuiKey_Keypad3,
        "Key_Keypad4", ImGuiKey_Keypad4,
        "Key_Keypad5", ImGuiKey_Keypad5,
        "Key_Keypad6", ImGuiKey_Keypad6,
        "Key_Keypad7", ImGuiKey_Keypad7,
        "Key_Keypad8", ImGuiKey_Keypad8,
        "Key_Keypad9", ImGuiKey_Keypad9,
        "Key_KeypadDecimal", ImGuiKey_KeypadDecimal,
        "Key_KeypadDivide", ImGuiKey_KeypadDivide,
        "Key_KeypadMultiply", ImGuiKey_KeypadMultiply,
        "Key_KeypadSubtract", ImGuiKey_KeypadSubtract,
        "Key_KeypadAdd", ImGuiKey_KeypadAdd,
        "Key_KeypadEnter", ImGuiKey_KeypadEnter,
        "Key_KeypadEqual", ImGuiKey_KeypadEqual,
        "Key_AppBack", ImGuiKey_AppBack,
        "Key_AppForward", ImGuiKey_AppForward,
        "Key_Oem102", ImGuiKey_Oem102,
        "Key_GamepadStart", ImGuiKey_GamepadStart,
        "Key_GamepadBack", ImGuiKey_GamepadBack,
        "Key_GamepadFaceLeft", ImGuiKey_GamepadFaceLeft,
        "Key_GamepadFaceRight", ImGuiKey_GamepadFaceRight,
        "Key_GamepadFaceUp", ImGuiKey_GamepadFaceUp,
        "Key_GamepadFaceDown", ImGuiKey_GamepadFaceDown,
        "Key_GamepadDpadLeft", ImGuiKey_GamepadDpadLeft,
        "Key_GamepadDpadRight", ImGuiKey_GamepadDpadRight,
        "Key_GamepadDpadUp", ImGuiKey_GamepadDpadUp,
        "Key_GamepadDpadDown", ImGuiKey_GamepadDpadDown,
        "Key_GamepadL1", ImGuiKey_GamepadL1,
        "Key_GamepadR1", ImGuiKey_GamepadR1,
        "Key_GamepadL2", ImGuiKey_GamepadL2,
        "Key_GamepadR2", ImGuiKey_GamepadR2,
        "Key_GamepadL3", ImGuiKey_GamepadL3,
        "Key_GamepadR3", ImGuiKey_GamepadR3,
        "Key_GamepadLStickLeft", ImGuiKey_GamepadLStickLeft,
        "Key_GamepadLStickRight", ImGuiKey_GamepadLStickRight,
        "Key_GamepadLStickUp", ImGuiKey_GamepadLStickUp,
        "Key_GamepadLStickDown", ImGuiKey_GamepadLStickDown,
        "Key_GamepadRStickLeft", ImGuiKey_GamepadRStickLeft,
        "Key_GamepadRStickRight", ImGuiKey_GamepadRStickRight,
        "Key_GamepadRStickUp", ImGuiKey_GamepadRStickUp,
        "Key_GamepadRStickDown", ImGuiKey_GamepadRStickDown,
        "Key_MouseLeft", ImGuiKey_MouseLeft,
        "Key_MouseRight", ImGuiKey_MouseRight,
        "Key_MouseMiddle", ImGuiKey_MouseMiddle,
        "Key_MouseX1", ImGuiKey_MouseX1,
        "Key_MouseX2", ImGuiKey_MouseX2,
        "Key_MouseWheelX", ImGuiKey_MouseWheelX,
        "Key_MouseWheelY", ImGuiKey_MouseWheelY,

        // Modifiers
        "Mod_None", ImGuiMod_None,
        "Mod_Ctrl", ImGuiMod_Ctrl,
        "Mod_Shift", ImGuiMod_Shift,
        "Mod_Alt", ImGuiMod_Alt,
        "Mod_Super", ImGuiMod_Super,
        "Mod_Mask_", ImGuiMod_Mask_
    );
    
    imgui.new_enum("ImGuiStyleVar",
        "Alpha", ImGuiStyleVar_Alpha,
        "DisabledAlpha", ImGuiStyleVar_DisabledAlpha,
        "WindowPadding", ImGuiStyleVar_WindowPadding,
        "WindowRounding", ImGuiStyleVar_WindowRounding,
        "WindowBorderSize", ImGuiStyleVar_WindowBorderSize,
        "WindowMinSize", ImGuiStyleVar_WindowMinSize,
        "WindowTitleAlign", ImGuiStyleVar_WindowTitleAlign,
        "ChildRounding", ImGuiStyleVar_ChildRounding,
        "ChildBorderSize", ImGuiStyleVar_ChildBorderSize,
        "PopupRounding", ImGuiStyleVar_PopupRounding,
        "PopupBorderSize", ImGuiStyleVar_PopupBorderSize,
        "FramePadding", ImGuiStyleVar_FramePadding,
        "FrameRounding", ImGuiStyleVar_FrameRounding,
        "FrameBorderSize", ImGuiStyleVar_FrameBorderSize,
        "ItemSpacing", ImGuiStyleVar_ItemSpacing,
        "ItemInnerSpacing", ImGuiStyleVar_ItemInnerSpacing,
        "IndentSpacing", ImGuiStyleVar_IndentSpacing,
        "CellPadding", ImGuiStyleVar_CellPadding,
        "ScrollbarSize", ImGuiStyleVar_ScrollbarSize,
        "ScrollbarRounding", ImGuiStyleVar_ScrollbarRounding,
        "GrabMinSize", ImGuiStyleVar_GrabMinSize,
        "GrabRounding", ImGuiStyleVar_GrabRounding,
        "ImageBorderSize", ImGuiStyleVar_ImageBorderSize,
        "TabRounding", ImGuiStyleVar_TabRounding,
        "TabBorderSize", ImGuiStyleVar_TabBorderSize,
        "TabBarBorderSize", ImGuiStyleVar_TabBarBorderSize,
        "TabBarOverlineSize", ImGuiStyleVar_TabBarOverlineSize,
        "TableAngledHeadersAngle", ImGuiStyleVar_TableAngledHeadersAngle,
        "TableAngledHeadersTextAlign", ImGuiStyleVar_TableAngledHeadersTextAlign,
        "ButtonTextAlign", ImGuiStyleVar_ButtonTextAlign,
        "SelectableTextAlign", ImGuiStyleVar_SelectableTextAlign,
        "SeparatorTextBorderSize", ImGuiStyleVar_SeparatorTextBorderSize,
        "SeparatorTextAlign", ImGuiStyleVar_SeparatorTextAlign,
        "SeparatorTextPadding", ImGuiStyleVar_SeparatorTextPadding
    );

    imgui.new_usertype<ImDrawList>("ImDrawList", 
        "push_clip_rect", [](ImDrawList* draw_list, sol::object min, sol::object max, bool intersect_with_current_clip_rect) {
            draw_list->PushClipRect(::api::imgui::create_imvec2(min), ::api::imgui::create_imvec2(max), intersect_with_current_clip_rect);
        },
        "push_clip_rect_fullscreen", &ImDrawList::PushClipRectFullScreen,
        "pop_clip_rect", &ImDrawList::PopClipRect,

        "add_line", [](ImDrawList* draw_list, sol::object p1, sol::object p2, uint32_t col, float thickness) {
            draw_list->AddLine(::api::imgui::create_imvec2(p1), ::api::imgui::create_imvec2(p2), col, thickness);
        },
        "add_rect", [](ImDrawList* draw_list, sol::object min, sol::object max, uint32_t col, float rounding, ImDrawFlags flags, float thickness) {
            draw_list->AddRect(::api::imgui::create_imvec2(min), ::api::imgui::create_imvec2(max), col, rounding, flags, thickness);
        },
        "add_rect_filled", [](ImDrawList* draw_list, sol::object min, sol::object max, uint32_t col, float rounding, ImDrawFlags flags) {
            draw_list->AddRectFilled(::api::imgui::create_imvec2(min), ::api::imgui::create_imvec2(max), col, rounding, flags);
        },
        "add_rect_filled_multi_color", [](ImDrawList* draw_list, sol::object min, sol::object max, uint32_t col_upr_left, uint32_t col_upr_right,
            uint32_t col_bot_right, uint32_t col_bot_left) {
            draw_list->AddRectFilledMultiColor(::api::imgui::create_imvec2(min), ::api::imgui::create_imvec2(max), col_upr_left, col_upr_right, col_bot_right, col_bot_left);
        },
        "add_quad", [](ImDrawList* draw_list, sol::object p1, sol::object p2, sol::object p3, sol::object p4, uint32_t col, float thickness) {
            draw_list->AddQuad(::api::imgui::create_imvec2(p1), ::api::imgui::create_imvec2(p2), ::api::imgui::create_imvec2(p3), ::api::imgui::create_imvec2(p4), col, thickness);
        },
        "add_quad_filled", [](ImDrawList* draw_list, sol::object p1, sol::object p2, sol::object p3, sol::object p4, uint32_t col) {
            draw_list->AddQuadFilled(::api::imgui::create_imvec2(p1), ::api::imgui::create_imvec2(p2), ::api::imgui::create_imvec2(p3), ::api::imgui::create_imvec2(p4), col);
        },
        "add_triangle", [](ImDrawList* draw_list, sol::object p1, sol::object p2, sol::object p3, uint32_t col, float thickness) {
            draw_list->AddTriangle(::api::imgui::create_imvec2(p1), ::api::imgui::create_imvec2(p2), ::api::imgui::create_imvec2(p3), col, thickness);
        },
        "add_triangle_filled", [](ImDrawList* draw_list, sol::object p1, sol::object p2, sol::object p3, uint32_t col) {
            draw_list->AddTriangleFilled(::api::imgui::create_imvec2(p1), ::api::imgui::create_imvec2(p2), ::api::imgui::create_imvec2(p3), col);
        },
        "add_circle", [](ImDrawList* draw_list, sol::object center, float radius, uint32_t col, int num_segments, float thickness) {
            draw_list->AddCircle(::api::imgui::create_imvec2(center), radius, col, num_segments, thickness);
        },
        "add_circle_filled", [](ImDrawList* draw_list, sol::object center, float radius, uint32_t col, int num_segments) {
            draw_list->AddCircleFilled(::api::imgui::create_imvec2(center), radius, col, num_segments);
        },
        "add_ngon", [](ImDrawList* draw_list, sol::object center, float radius, uint32_t col, int num_segments, float thickness) {
            draw_list->AddNgon(::api::imgui::create_imvec2(center), radius, col, num_segments, thickness);
        },
        "add_ngon_filled", [](ImDrawList* draw_list, sol::object center, float radius, uint32_t col, int num_segments) {
            draw_list->AddNgonFilled(::api::imgui::create_imvec2(center), radius, col, num_segments);
        },
        "add_ellipse", [](ImDrawList* draw_list, sol::object center, sol::object radius, uint32_t col, float rot, int num_segments, float thickness) {
            draw_list->AddEllipse(::api::imgui::create_imvec2(center), ::api::imgui::create_imvec2(radius), col, num_segments, rot, thickness);
        },
        "add_ellipse_filled", [](ImDrawList* draw_list, sol::object center, sol::object radius, uint32_t col, float rot, int num_segments) {
            draw_list->AddEllipseFilled(::api::imgui::create_imvec2(center), ::api::imgui::create_imvec2(radius), col, rot, num_segments);
        },
        "add_text", [](ImDrawList* draw_list, sol::object pos, uint32_t col, const std::string& text) {
            draw_list->AddText(::api::imgui::create_imvec2(pos), col, text.c_str());
        },
        "add_bezier_cubic", [](ImDrawList* draw_list, sol::object p1, sol::object p2, sol::object p3, sol::object p4, uint32_t col, float thickness) {
            draw_list->AddBezierCubic(::api::imgui::create_imvec2(p1), ::api::imgui::create_imvec2(p2), ::api::imgui::create_imvec2(p3), ::api::imgui::create_imvec2(p4), col, thickness);
        },
        "add_bezier_quadratic", [](ImDrawList* draw_list, sol::object p1, sol::object p2, sol::object p3, uint32_t col, float thickness) {
            draw_list->AddBezierQuadratic(::api::imgui::create_imvec2(p1), ::api::imgui::create_imvec2(p2), ::api::imgui::create_imvec2(p3), col, thickness);
        },

        // Path APIs
        "path_clear", &ImDrawList::PathClear, 
        "path_line_to", [](ImDrawList* draw_list, sol::object pos) { draw_list->PathLineTo(::api::imgui::create_imvec2(pos)); }, 
        "path_line_to_merge_duplicate", [](ImDrawList* draw_list, sol::object pos) {
            draw_list->PathLineToMergeDuplicate(::api::imgui::create_imvec2(pos));
        }, 
        "path_fill_convex", &ImDrawList::PathFillConvex,
        "path_fill_concave", &ImDrawList::PathFillConcave,
        "path_stroke", [](ImDrawList* draw_list, uint32_t col, ImDrawFlags flags, float thickness) {
            draw_list->PathStroke(col, flags, thickness);
        },
        "path_arc_to", [](ImDrawList* draw_list, sol::object center, float radius, float a_min, float a_max, int num_segments) {
            draw_list->PathArcTo(::api::imgui::create_imvec2(center), radius, a_min, a_max, num_segments);
        },
        "path_arc_to_fast",
        [](ImDrawList* draw_list, sol::object center, float radius, int a_min_of_12, int a_max_of_12) {
            draw_list->PathArcToFast(::api::imgui::create_imvec2(center), radius, a_min_of_12, a_max_of_12);
        },
        "path_elliptical_arc_to", [](ImDrawList* draw_list, sol::object center, sol::object radius, float a_min, float a_max, int num_segments) {
            draw_list->PathEllipticalArcTo(::api::imgui::create_imvec2(center), ::api::imgui::create_imvec2(radius), a_min, a_max, num_segments);
        },
        "path_bezier_cubic_curve_to", [](ImDrawList* draw_list, sol::object p2, sol::object p3, sol::object p4, int num_segments) {
            draw_list->PathBezierCubicCurveTo(::api::imgui::create_imvec2(p2), ::api::imgui::create_imvec2(p3), ::api::imgui::create_imvec2(p4), num_segments);
        },
        "path_bezier_quadratic_curve_to", [](ImDrawList* draw_list, sol::object p2, sol::object p3, int num_segments) {
            draw_list->PathBezierQuadraticCurveTo(::api::imgui::create_imvec2(p2), ::api::imgui::create_imvec2(p3), num_segments);
        },
        "path_rect", [](ImDrawList* draw_list, sol::object min, sol::object max, float rounding, ImDrawFlags flags) {
            draw_list->PathRect(::api::imgui::create_imvec2(min), ::api::imgui::create_imvec2(max), rounding, flags);
        }
    );

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

    auto imnodes = lua.create_table();

    imnodes["is_editor_hovered"] = &api::imnodes::is_editor_hovered;
    imnodes["is_node_hovered"] = &api::imnodes::is_node_hovered;
    imnodes["is_link_hovered"] = &api::imnodes::is_link_hovered;
    imnodes["is_pin_hovered"] = &api::imnodes::is_pin_hovered;
    imnodes["begin_node_editor"] = &api::imnodes::begin_node_editor;
    imnodes["end_node_editor"] = &api::imnodes::end_node_editor;
    imnodes["begin_node"] = &api::imnodes::begin_node;
    imnodes["end_node"] = &api::imnodes::end_node;
    imnodes["begin_node_titlebar"] = &api::imnodes::begin_node_titlebar;
    imnodes["end_node_titlebar"] = &api::imnodes::end_node_titlebar;
    imnodes["begin_input_attribute"] = &api::imnodes::begin_input_attribute;
    imnodes["end_input_attribute"] = &api::imnodes::end_input_attribute;
    imnodes["begin_output_attribute"] = &api::imnodes::begin_output_attribute;
    imnodes["end_output_attribute"] = &api::imnodes::end_output_attribute;
    imnodes["begin_static_attribute"] = &api::imnodes::begin_static_attribute;
    imnodes["end_static_attribute"] = &api::imnodes::end_static_attribute;
    imnodes["minimap"] = &api::imnodes::minimap;
    imnodes["link"] = &api::imnodes::link;
    imnodes["get_node_dimensions"] = &api::imnodes::get_node_dimensions;
    imnodes["push_color_style"] = &api::imnodes::push_color_style;
    imnodes["pop_color_style"] = &api::imnodes::pop_color_style;
    imnodes["push_style_var"] = &api::imnodes::push_style_var;
    imnodes["push_style_var_vec2"] = &api::imnodes::push_style_var_vec2;
    imnodes["pop_style_var"] = &api::imnodes::pop_style_var;
    imnodes["set_node_screen_space_pos"] = &api::imnodes::set_node_screen_space_pos;
    imnodes["set_node_editor_space_pos"] = &api::imnodes::set_node_editor_space_pos;
    imnodes["set_node_grid_space_pos"] = &api::imnodes::set_node_grid_space_pos;
    imnodes["get_node_screen_space_pos"] = &api::imnodes::get_node_screen_space_pos;
    imnodes["get_node_editor_space_pos"] = &api::imnodes::get_node_editor_space_pos;
    imnodes["get_node_grid_space_pos"] = &api::imnodes::get_node_grid_space_pos;
    imnodes["is_editor_hovered"] = &ImNodes::IsEditorHovered;
    imnodes["is_node_selected"] = &ImNodes::IsNodeSelected;
    imnodes["is_link_selected"] = &ImNodes::IsLinkSelected;
    imnodes["num_selected_nodes"] = &ImNodes::NumSelectedNodes;
    imnodes["num_selected_links"] = &ImNodes::NumSelectedLinks;
    imnodes["clear_node_selection"] = &api::imnodes::clear_node_selection;
    imnodes["clear_link_selection"] = &api::imnodes::clear_link_selection;
    imnodes["select_node"] = &ImNodes::SelectNode;
    imnodes["select_link"] = &ImNodes::SelectLink;
    imnodes["is_attribute_active"] = &ImNodes::IsAttributeActive;
    imnodes["snap_node_to_grid"] = &ImNodes::SnapNodeToGrid;

    imnodes["editor_move_to_node"] = &ImNodes::EditorContextMoveToNode;
    imnodes["editor_reset_panning"] = &api::imnodes::editor_reset_panning;
    imnodes["editor_get_panning"] = &api::imnodes::editor_get_panning;
    
    imnodes["is_link_started"] = &api::imnodes::is_link_started;
    imnodes["is_link_dropped"] = &api::imnodes::is_link_dropped;
    imnodes["is_link_created"] = &api::imnodes::is_link_created;
    imnodes["is_link_destroyed"] = &api::imnodes::is_link_destroyed;

    imnodes["get_selected_nodes"] = &api::imnodes::get_selected_nodes;
    imnodes["get_selected_links"] = &api::imnodes::get_selected_links;

    lua["imnodes"] = imnodes;
}
