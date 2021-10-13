#include <spdlog/spdlog.h>

#include "mods/IntegrityCheckBypass.hpp"
#include "mods/Hooks.hpp"
#include "mods/Camera.hpp"
#include "mods/FirstPerson.hpp"
#include "mods/DeveloperTools.hpp"
#include "mods/ManualFlashlight.hpp"
#include "mods/FreeCam.hpp"
#include "mods/Scene.hpp"
#include "mods/VR.hpp"

#include "Mods.hpp"

Mods::Mods()
{
#if defined(RE3) || defined(RE8)
    m_mods.emplace_back(std::make_unique<IntegrityCheckBypass>());
#endif

#ifndef BAREBONES
    m_mods.emplace_back(std::make_unique<Hooks>());

#ifndef MHRISE
    m_mods.emplace_back(VR::get());
#endif

#ifndef RE8

#if defined(RE2) || defined(RE3)
    m_mods.emplace_back(FirstPerson::get());
#endif

#else
    m_mods.emplace_back(std::make_unique<Camera>());
#endif

#if defined(RE2) || defined(RE3) || defined(RE8)
    m_mods.emplace_back(std::make_unique<ManualFlashlight>());
#endif

    m_mods.emplace_back(std::make_unique<FreeCam>());

#ifndef RE7
    m_mods.emplace_back(std::make_unique<SceneMods>());
#endif

#endif

#ifdef DEVELOPER
    m_mods.emplace_back(std::make_unique<DeveloperTools>());
#endif
}

std::optional<std::string> Mods::on_initialize() const {
    for (auto& mod : m_mods) {
        spdlog::info("{:s}::on_initialize()", mod->get_name().data());

        if (auto e = mod->on_initialize(); e != std::nullopt) {
            spdlog::info("{:s}::on_initialize() has failed: {:s}", mod->get_name().data(), *e);
            return e;
        }
    }

    utility::Config cfg{ "re2_fw_config.txt" };

    for (auto& mod : m_mods) {
        spdlog::info("{:s}::on_config_load()", mod->get_name().data());
        mod->on_config_load(cfg);
    }

    return std::nullopt;
}

void Mods::on_frame() const {
    for (auto& mod : m_mods) {
        mod->on_frame();
    }
}

void Mods::on_post_frame() const {
    for (auto& mod : m_mods) {
        mod->on_post_frame();
    }
}

void Mods::on_draw_ui() const {
    for (auto& mod : m_mods) {
        mod->on_draw_ui();
    }
}

void Mods::on_device_reset() const {
    for (auto& mod : m_mods) {
        mod->on_device_reset();
    }
}
