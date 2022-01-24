#pragma once

#include <map>
#include <mutex>

#include <Windows.h>

#include "Mod.hpp"
#include "reframework/API.hpp"

namespace reframework {
extern REFrameworkRendererData g_renderer_data;
}

class PluginLoader : public Mod {
public:
    static std::shared_ptr<PluginLoader> get();

    // This is called prior to most REFramework initialization so that all plugins are at least **loaded** early on. REFramework plugin
    // specifics (`reframework_plugin_initialize`) are still delayed until REFramework is fully setup.
    void early_init();

    std::string_view get_name() const override { return "PluginLoader"; }
    std::optional<std::string> on_initialize() override;
    void on_draw_ui() override;

private:
    std::mutex m_mux{};
    std::map<std::string, HMODULE> m_plugins{};
    std::map<std::string, std::string> m_plugin_load_errors{};
    std::map<std::string, std::string> m_plugin_load_warnings{};
};
