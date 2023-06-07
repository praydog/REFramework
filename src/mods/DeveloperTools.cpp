#include "tools/GameObjectsDisplay.hpp"
#include "tools/ChainViewer.hpp"
#include "tools/ObjectExplorer.hpp"

#include "DeveloperTools.hpp"

DeveloperTools::DeveloperTools() {
    m_tools.emplace_back(std::make_shared<ChainViewer>());
    m_tools.emplace_back(std::make_shared<GameObjectsDisplay>());
    #ifndef _DEBUG
    // std::structs are not same as Release, this made crash
    m_tools.emplace_back(ObjectExplorer::get());
    #endif
}

void DeveloperTools::on_draw_ui() {
    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    for (auto& tool : m_tools) {
        tool->on_draw_dev_ui();
    }
}

