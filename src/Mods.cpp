#include <spdlog/spdlog.h>

#include "PositionHooks.hpp"
#include "FirstPerson.hpp"
#include "DeveloperTools.hpp"
#include "ManualFlashlight.hpp"

#include "Mods.hpp"

Mods::Mods()
{
    m_mods.emplace_back(std::make_unique<PositionHooks>());
    m_mods.emplace_back(std::make_unique<FirstPerson>());
    m_mods.emplace_back(std::make_unique<ManualFlashlight>());

#ifdef DEVELOPER
    m_mods.emplace_back(std::make_unique<DeveloperTools>());
#endif
}

bool Mods::onInitialize() const {
    for (auto& mod : m_mods) {
        spdlog::info("{:s}::onInitialize()", mod->getName().data());

        if (!mod->onInitialize()) {
            spdlog::info("{:s}::onInitialize() has failed", mod->getName().data());
            return false;
        }
    }

    utility::Config cfg{ "re2_fw_config.txt" };

    for (auto& mod : m_mods) {
        spdlog::info("{:s}::onConfigLoad()", mod->getName().data());
        mod->onConfigLoad(cfg);
    }

    return true;
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

