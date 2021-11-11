#include <imgui.h>

#include "../ScriptRunner.hpp"

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
    auto preview_value = values[selection].get<const char*>();
    auto selection_changed = false;

    if (ImGui::BeginCombo(label, preview_value)) {
        for (auto i = 1u; i <= values.size(); ++i) {
            auto entry = values[i].get<const char*>();
            if (ImGui::Selectable(entry, selection == i)) {
                selection = i;
                selection_changed = true;
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
}
