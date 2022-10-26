#include <imgui.h>

#include "tools/GameObjectsDisplay.hpp"
#include "tools/ChainViewer.hpp"
#include "tools/ObjectExplorer.hpp"

#include "DeveloperTools.hpp"

DeveloperTools::DeveloperTools() {
    m_tools.emplace_back(std::make_shared<ChainViewer>());
    m_tools.emplace_back(std::make_shared<GameObjectsDisplay>());
    m_tools.emplace_back(ObjectExplorer::get());
}

void DeveloperTools::on_draw_ui() {
    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    for (auto& tool : m_tools) {
        ImGui::PushID(tool->get_name().data());
        tool->on_draw_dev_ui();
        ImGui::PopID();
    }
}

