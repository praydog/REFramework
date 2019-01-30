#include "ComponentHook.hpp"
#include "PositionHooks.hpp"
#include "FirstPerson.hpp"

#include "Mods.hpp"

Mods::Mods()
{
    m_mods.push_back(m_componentTraverser = std::make_shared<ComponentTraverser>());
    m_mods.push_back(std::make_unique<ComponentHook>());
    m_mods.push_back(std::make_unique<PositionHooks>());
    m_mods.push_back(std::make_unique<FirstPerson>());
}

void Mods::onFrame() const {
    for (auto& mod : m_mods) {
        mod->onFrame();
    }
}

void Mods::onDrawUI() const {
    for (auto& mod : m_mods) {
        mod->onDrawUI();
    }
}

