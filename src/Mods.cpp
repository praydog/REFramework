#include <spdlog/spdlog.h>

#include "mods/APIProxy.hpp"
#include "mods/Camera.hpp"
#include "mods/Graphics.hpp"
#include "mods/DeveloperTools.hpp"
#include "mods/FirstPerson.hpp"
#include "mods/FreeCam.hpp"
#include "mods/Hooks.hpp"
#include "mods/IntegrityCheckBypass.hpp"
#include "mods/ManualFlashlight.hpp"
#include "mods/PluginLoader.hpp"
#include "mods/REFrameworkConfig.hpp"
#include "mods/Scene.hpp"
#include "mods/ScriptRunner.hpp"
#include "mods/VR.hpp"
#include "mods/vr/games/RE8VR.hpp"

#include "Mods.hpp"

Mods::Mods() {
    m_mods.emplace_back(REFrameworkConfig::get());

#if defined(RE3) || defined(RE8) || defined(MHRISE)
    m_mods.emplace_back(std::make_unique<IntegrityCheckBypass>());
#endif

#ifndef BAREBONES
    m_mods.emplace_back(Hooks::get());

    m_mods.emplace_back(VR::get());

#if defined(RE8) || defined(RE7)
    m_mods.emplace_back(RE8VR::get());
#endif

#ifndef RE8
#if defined(RE2) || defined(RE3)
    m_mods.emplace_back(FirstPerson::get());
#endif
#endif

    // All games!!!!
    m_mods.emplace_back(std::make_unique<Camera>());
    m_mods.emplace_back(std::make_unique<Graphics>());

#if defined(RE2) || defined(RE3) || defined(RE8)
    m_mods.emplace_back(std::make_unique<ManualFlashlight>());
#endif

    m_mods.emplace_back(std::make_unique<FreeCam>());

#if TDB_VER > 49
    m_mods.emplace_back(std::make_unique<SceneMods>());
#endif

#endif

#ifdef DEVELOPER
    auto dev_tools = std::make_shared<DeveloperTools>();
    m_mods.emplace_back(dev_tools);

    for (auto& tool : dev_tools->get_tools()) {
        m_mods.emplace_back(tool);
    }
#endif

    m_mods.emplace_back(APIProxy::get());
    m_mods.emplace_back(PluginLoader::get());
    m_mods.emplace_back(ScriptRunner::get());
}

std::optional<std::string> Mods::on_initialize() const {
    for (auto& mod : m_mods) {
        spdlog::info("{:s}::on_initialize()", mod->get_name().data());

        if (auto e = mod->on_initialize(); e != std::nullopt) {
            spdlog::info("{:s}::on_initialize() has failed: {:s}", mod->get_name().data(), *e);
            return e;
        }
    }

    utility::Config cfg{ (REFramework::get_persistent_dir() / "re2_fw_config.txt").string() };

    for (auto& mod : m_mods) {
        spdlog::info("{:s}::on_config_load()", mod->get_name().data());
        mod->on_config_load(cfg);
    }

    return std::nullopt;
}


std::optional<std::string> Mods::on_initialize_d3d_thread() const {
    std::scoped_lock _{g_framework->get_hook_monitor_mutex()};

    utility::Config cfg{ (REFramework::get_persistent_dir() / "re2_fw_config.txt").string() };

    // once here to at least setup the values
    for (auto& mod : m_mods) {
        spdlog::info("{:s}::on_config_load()", mod->get_name().data());
        mod->on_config_load(cfg);
    }

    for (auto& mod : m_mods) {
        spdlog::info("{:s}::on_initialize_d3d_thread()", mod->get_name().data());

        if (auto e = mod->on_initialize_d3d_thread(); e != std::nullopt) {
            spdlog::info("{:s}::on_initialize_d3d_thread() has failed: {:s}", mod->get_name().data(), *e);
            return e;
        }
    }

    for (auto& mod : m_mods) {
        spdlog::info("{:s}::on_config_load()", mod->get_name().data());
        mod->on_config_load(cfg);
    }

    return std::nullopt;
}

void Mods::on_pre_imgui_frame() const {
    for (auto& mod : m_mods) {
        mod->on_pre_imgui_frame();
    }
}

void Mods::on_frame() const {
    for (auto& mod : m_mods) {
        mod->on_frame();
    }
}

void Mods::on_present() const {
    for (auto& mod : m_mods) {
        mod->on_present();
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
