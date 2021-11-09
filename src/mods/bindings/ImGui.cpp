#include <imgui.h>

#include "../ScriptRunner.hpp"

#include "ImGui.hpp"

namespace api::imgui {
bool button(const char* label) {
    return ImGui::Button(label);
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
    lua["imgui"] = imgui;
}
