#include <spdlog/spdlog.h>

#include "PositionHooks.hpp"
#include "FirstPerson.hpp"
#include "ObjectExplorer.hpp"

#include "Mods.hpp"

Mods::Mods() {
    m_mods.push_back(std::make_unique<PositionHooks>());
    m_mods.push_back(std::make_unique<FirstPerson>());
    m_mods.push_back(std::make_unique<ObjectExplorer>());
}

bool Mods::onInitialize() const {
    bool ret = true;

    for (auto& mod : m_mods) {
        spdlog::info("{:s}::onInitialize()", mod->getName().data());

        if (ret = mod->onInitialize(); !ret) {
            spdlog::info("{:s}::onInitialize() has failed", mod->getName().data());
            break;
        }
    }

    return ret;
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

