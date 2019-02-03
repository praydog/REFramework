#include <functional>
#include <unordered_set>

#include <spdlog/spdlog.h>
#include <imgui/imgui.h>

#include "REFramework.hpp"

#include "ComponentTraverser.hpp"

void ComponentTraverser::onDrawUI() {
#ifndef NDEBUG
    ImGui::Begin("ComponentHook");

    if (ImGui::Button("Refresh Components")) {
        refresh();
    }

    ImGui::Text("Refresh Count: %i", m_timesRefreshed);

    ImGui::End();
#endif
}

void ComponentTraverser::onGetMainCamera(RECamera* camera) {
    if (!m_issueRefresh || std::chrono::system_clock::now() < m_nextTime) {
        return;
    }

    auto callOnComponent = [this](REComponent* component) {
        if (component == nullptr) {
            return;
        }

        auto& mods = g_framework->getMods().getMods();
        for (auto& mod : mods) {
            mod->onComponent(component);
        }
    };

    std::function<void(REComponent*, std::unordered_set<REComponent*>&)> traverseHierarchy;

    auto traverseHierarchy2 = [&](REComponent* startComp, std::unordered_set<REComponent*>& encounteredComponents) {
        if (startComp == nullptr) {
            return;
        }

        if (encounteredComponents.count(startComp) > 0) {
            return;
        }

        auto gameObject = startComp->ownerGameObject;

        if (gameObject == nullptr) {
            return;
        }

        //spdlog::info("{:p} {} {}", (void*)startComp, utility::REString::getString(gameObject->name).c_str(), startComp->info->classInfo->type->name);

        encounteredComponents.insert(startComp);
        callOnComponent(startComp);

        for (auto component = startComp->nextComponent; component && component != startComp; component = component->nextComponent) {
            traverseHierarchy(component, encounteredComponents);
        }
    };

    traverseHierarchy = [&](REComponent* startComp, std::unordered_set<REComponent*>& encounteredComponents) {
        if (startComp == nullptr) {
            return;
        }

        if (encounteredComponents.count(startComp) > 0) {
            return;
        }

        traverseHierarchy2(startComp, encounteredComponents);

        for (auto component = startComp->idkComponent; component && component != startComp; component = component->idkComponent) {
            auto gameObject = component->ownerGameObject;

            if (gameObject == nullptr) {
                continue;
            }

            traverseHierarchy2(component, encounteredComponents);
        }
    };

    std::unordered_set<REComponent*> encounteredComponents;
    traverseHierarchy(camera, encounteredComponents);

    m_issueRefresh = false;
    m_nextTime = std::chrono::system_clock::now() + std::chrono::seconds(3);
    ++m_timesRefreshed;
}

void ComponentTraverser::refresh() {
    m_issueRefresh = true;
}

void ComponentTraverser::refreshComponents() {
    g_framework->getMods().getComponentTraverser()->refresh();
}
