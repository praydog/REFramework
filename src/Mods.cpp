#include <spdlog/spdlog.h>

#include "PositionHooks.hpp"
#include "FirstPerson.hpp"
#include "DeveloperTools.hpp"
#include "ManualFlashlight.hpp"
#include "FreeCam.hpp"

#include "Mods.hpp"

Mods::Mods()
{
    m_mods.emplace_back(std::make_unique<PositionHooks>());
    m_mods.emplace_back(std::make_unique<FirstPerson>());
    m_mods.emplace_back(std::make_unique<ManualFlashlight>());
    m_mods.emplace_back(std::make_unique<FreeCam>());

#ifdef DEVELOPER
    m_mods.emplace_back(std::make_unique<DeveloperTools>());
#endif
}

std::optional<std::string> Mods::onInitialize() const {
    for (auto& mod : m_mods) {
        spdlog::info("{:s}::onInitialize()", mod->getName().data());

        if (auto e = mod->onInitialize(); e != std::nullopt) {
            spdlog::info("{:s}::onInitialize() has failed: {:s}", mod->getName().data(), *e);
            return e;
        }
    }

    utility::Config cfg{ "re2_fw_config.txt" };

    for (auto& mod : m_mods) {
        spdlog::info("{:s}::onConfigLoad()", mod->getName().data());
        mod->onConfigLoad(cfg);
    }

    return std::nullopt;
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

