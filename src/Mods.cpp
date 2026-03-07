#include <spdlog/spdlog.h>

#include <sdk/GameIdentity.hpp>
#include "mods/BackBufferRenderer.hpp"
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
#include "mods/MethodDatabase.hpp"
#include "mods/Scene.hpp"
#include "mods/ScriptRunner.hpp"
#include "mods/VR.hpp"
#include "mods/LooseFileLoader.hpp"
#include "mods/FaultyFileDetector.hpp"
#include "mods/vr/games/RE8VR.hpp"

#include "Mods.hpp"

Mods::Mods() {
    m_mods.emplace_back(BackBufferRenderer::get());
    m_mods.emplace_back(REFrameworkConfig::get());

    // IntegrityCheckBypass: only for games with anti-tamper (REENGINE_AT)
    if (sdk::GameIdentity::get().is_reengine_at()) {
        m_mods.emplace_back(IntegrityCheckBypass::get_shared_instance());
    }

#ifndef BAREBONES
    m_mods.emplace_back(MethodDatabase::get());
    m_mods.emplace_back(Hooks::get());
    m_mods.emplace_back(LooseFileLoader::get());

    if (sdk::GameIdentity::get().is_mhwilds()) {
        m_mods.emplace_back(FaultyFileDetector::get());
    }

    m_mods.emplace_back(VR::get());

    if (sdk::GameIdentity::get().is_re8() || sdk::GameIdentity::get().is_re7()) {
        m_mods.emplace_back(RE8VR::get());
    }

    {
        const auto& gi = sdk::GameIdentity::get();
        if (!gi.is_re8() && (gi.is_re2() || gi.is_re3())) {
            m_mods.emplace_back(FirstPerson::get());
        }
    }

    // All games!!!!
    m_mods.emplace_back(std::make_unique<Camera>());
    m_mods.emplace_back(Graphics::get());

    {
        const auto& gi = sdk::GameIdentity::get();
        if (gi.is_re2() || gi.is_re3() || gi.is_re8()) {
            m_mods.emplace_back(std::make_unique<ManualFlashlight>());
        }
    }

    m_mods.emplace_back(std::make_unique<FreeCam>());

    if (sdk::GameIdentity::get().tdb_ver() > 49) {
        m_mods.emplace_back(std::make_unique<SceneMods>());
    }

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

    utility::Config cfg{ (REFramework::get_persistent_dir() / REFrameworkConfig::REFRAMEWORK_CONFIG_NAME).string() };

    for (auto& mod : m_mods) {
        spdlog::info("{:s}::on_config_load()", mod->get_name().data());
        mod->on_config_load(cfg);
    }

    return std::nullopt;
}


std::optional<std::string> Mods::on_initialize_d3d_thread() const {
    auto do_not_hook_d3d = g_framework->acquire_do_not_hook_d3d();

    utility::Config cfg{ (REFramework::get_persistent_dir() / REFrameworkConfig::REFRAMEWORK_CONFIG_NAME).string() };

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
