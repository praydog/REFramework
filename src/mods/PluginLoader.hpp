#pragma once

#include <map>
#include <mutex>

#include <Windows.h>

#include "Mod.hpp"
#include "reframework/API.hpp"

bool reframework_on_lua_state_created(REFLuaStateCreatedCb cb);
bool reframework_on_lua_state_destroyed(REFLuaStateDestroyedCb cb);
bool reframework_on_present(REFOnPresentCb cb);
bool reframework_on_pre_application_entry(const char* name, REFOnPreApplicationEntryCb cb);
bool reframework_on_post_application_entry(const char* name, REFOnPostApplicationEntryCb cb);
void reframework_lock_lua();
void reframework_unlock_lua();
bool reframework_on_device_reset(REFOnDeviceResetCb cb);
bool reframework_on_message(REFOnMessageCb cb);
bool reframework_on_imgui_frame(REFOnImGuiFrameCb cb);
bool reframework_on_imgui_draw_ui(REFOnImGuiDrawUICb cb);
bool reframework_on_pre_gui_draw_element(REFOnPreGuiDrawElementCb cb);

lua_State* reframework_create_script_state();
void reframework_destroy_script_state(lua_State*);
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
