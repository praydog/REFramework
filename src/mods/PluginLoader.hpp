#pragma once

#include <map>

#include <Windows.h>

#include "Mod.hpp"

class PluginLoader : public Mod {
public:
    std::string_view get_name() const override { return "PluginLoader"; }
    std::optional<std::string> on_initialize() override;
    void on_draw_ui() override;

private:
    std::map<std::string, HMODULE> m_plugins{};
    std::map<std::string, std::string> m_plugin_load_errors{};
    std::map<std::string, std::string> m_plugin_load_warnings{};
};
