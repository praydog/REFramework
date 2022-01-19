#include <filesystem>

#include <imgui.h>
#include <spdlog/spdlog.h>

#include "REFramework.hpp"
#include "reframework/API.hpp"

#include "PluginLoader.hpp"

REFrameworkPluginVersion g_plugin_version{
    REFRAMEWORK_PLUGIN_VERSION_MAJOR, REFRAMEWORK_PLUGIN_VERSION_MINOR, REFRAMEWORK_PLUGIN_VERSION_PATCH};

namespace reframework {
REFrameworkRendererData g_renderer_data{
    REFRAMEWORK_RENDERER_D3D12, nullptr, nullptr, nullptr
};
}

REFrameworkPluginFunctions g_plugin_functions{
    reframework_on_initialized,
    reframework_on_lua_state_created,
    reframework_on_lua_state_destroyed,
    reframework_on_frame,
    reframework_on_pre_application_entry,
    reframework_on_post_application_entry,
    reframework_lock_lua,
    reframework_unlock_lua,
    reframework_on_device_reset,
    reframework_on_message
};
REFrameworkPluginInitializeParam g_plugin_initialize_param{nullptr, &g_plugin_version, &g_plugin_functions, &reframework::g_renderer_data};

std::optional<std::string> PluginLoader::on_initialize() {
    namespace fs = std::filesystem;

    std::string module_path{};

    module_path.resize(1024, 0);
    module_path.resize(GetModuleFileName(nullptr, module_path.data(), module_path.size()));
    spdlog::info("[PluginLoader] Module path {}", module_path);

    auto plugin_path = fs::path{module_path}.parent_path() / "plugins";

    spdlog::info("[PluginLoader] Creating directories {}", plugin_path.string());
    fs::create_directories(plugin_path);
    spdlog::info("[PluginLoader] Loading plugins...");

    // Load all dlls in the plugins directory.
    for (auto&& entry : fs::directory_iterator{plugin_path}) {
        auto&& path = entry.path();

        if (path.has_extension() && path.extension() == ".dll") {
            auto module = LoadLibrary(path.string().c_str());

            if (module == nullptr) {
                spdlog::error("[PluginLoader] Failed to load {}", path.string());
                m_plugin_load_errors.emplace(path.stem().string(), "Failed to load");
                continue;
            }

            spdlog::info("[PluginLoader] Loaded {}", path.string());
            m_plugins.emplace(path.stem().string(), module);
        }
    }

    // Call reframework_plugin_required_version on any dlls that export it.
    g_plugin_initialize_param.reframework_module = g_framework->get_reframework_module();
    reframework::g_renderer_data.renderer_type = (int)g_framework->get_renderer_type();
    
    if (reframework::g_renderer_data.renderer_type == REFRAMEWORK_RENDERER_D3D11) {
        auto& d3d11 = g_framework->get_d3d11_hook();

        reframework::g_renderer_data.device = d3d11->get_device();
        reframework::g_renderer_data.swapchain = d3d11->get_swap_chain();
    } else if (reframework::g_renderer_data.renderer_type == REFRAMEWORK_RENDERER_D3D12) {
        auto& d3d12 = g_framework->get_d3d12_hook();

        reframework::g_renderer_data.device = d3d12->get_device();
        reframework::g_renderer_data.swapchain = d3d12->get_swap_chain();
        reframework::g_renderer_data.command_queue = d3d12->get_command_queue();
    } else {
        spdlog::error("[PluginLoader] Unsupported renderer type {}", reframework::g_renderer_data.renderer_type);
        return "PluginLoader: Unsupported renderer type detected";
    }

    for (auto it = m_plugins.begin(); it != m_plugins.end();) {
        auto name = it->first;
        auto mod = it->second;
        auto required_version_fn = (REFPluginRequiredVersionFn)GetProcAddress(mod, "reframework_plugin_required_version");

        if (required_version_fn == nullptr) {
            ++it;
            continue;
        }

        REFrameworkPluginVersion required_version{};
        required_version_fn(&required_version);

        spdlog::info(
            "[PluginLoader] {} requires version {}.{}.{}", name, required_version.major, required_version.minor, required_version.patch);

        if (required_version.major != g_plugin_version.major) {
            spdlog::error("[PluginLoader] Plugin {} requires a different major version", name);
            m_plugin_load_errors.emplace(name, "Requires a different major version");
            FreeLibrary(mod);
            it = m_plugins.erase(it);
            continue;
        }

        if (required_version.minor > g_plugin_version.minor) {
            spdlog::error("[PluginLoader] Plugin {} requires a newer minor version", name);
            m_plugin_load_errors.emplace(name, "Requires a newer minor version");
            FreeLibrary(mod);
            it = m_plugins.erase(it);
            continue;
        }

        if (required_version.patch > g_plugin_version.patch) {
            spdlog::warn("[PluginLoader] Plugin {} desires a newer patch version", name);
            m_plugin_load_warnings.emplace(name, "Desires a newer patch version");
        }

        ++it;
    }

    // Call reframework_plugin_initialize on any dlls that export it.
    for (auto it = m_plugins.begin(); it != m_plugins.end();) {
        auto name = it->first;
        auto mod = it->second;
        auto init_fn = (REFPluginInitializeFn)GetProcAddress(mod, "reframework_plugin_initialize");

        if (init_fn == nullptr) {
            ++it;
            continue;
        }

        spdlog::info("[PluginLoader] Initializing {}...", name);

        if (!init_fn(&g_plugin_initialize_param)) {
            spdlog::error("[PluginLoader] Failed to initialize {}", name);
            m_plugin_load_errors.emplace(name, "Failed to initialize");
            FreeLibrary(mod);
            it = m_plugins.erase(it);
            continue;
        }

        ++it;
    }

    return std::nullopt;
}

void PluginLoader::on_draw_ui() {
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_Once);

    if (ImGui::CollapsingHeader(get_name().data())) {
        if (!m_plugins.empty()) {
            ImGui::Text("Loaded plugins:");

            for (auto&& [name, _] : m_plugins) {
                ImGui::Text(name.c_str());
            }
        } else {
            ImGui::Text("No plugins loaded.");
        }

        if (!m_plugin_load_errors.empty()) {
            ImGui::Spacing();
            ImGui::Text("Errors:");
            for (auto&& [name, error] : m_plugin_load_errors) {
                ImGui::Text("%s - %s", name.c_str(), error.c_str());
            }
        }

        if (!m_plugin_load_warnings.empty()) {
            ImGui::Spacing();
            ImGui::Text("Warnings:");
            for (auto&& [name, warning] : m_plugin_load_warnings) {
                ImGui::Text("%s - %s", name.c_str(), warning.c_str());
            }
        }
    }
}
