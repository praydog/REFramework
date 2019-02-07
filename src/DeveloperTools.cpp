#include "ObjectExplorer.hpp"

#include "DeveloperTools.hpp"

DeveloperTools::DeveloperTools() {
    m_tools.push_back(std::make_shared<ObjectExplorer>());
}

void DeveloperTools::onDrawUI() {
    if (!ImGui::CollapsingHeader(getName().data())) {
        return;
    }

    for (auto& tool : m_tools) {
        tool->onDrawUI();
    }

    ImGui::TreePop();
}

