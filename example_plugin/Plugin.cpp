#include <Windows.h>

#include <reframework/API.hpp>

lua_State* g_lua = nullptr;

void on_lua_state_created(lua_State* l) {
    g_lua = l;

    MessageBox(NULL, "Lua State Created", "DLL", MB_OK);
}

void on_lua_state_destroyed(lua_State* l) {
    g_lua = nullptr;

    MessageBox(NULL, "Lua State Destroyed", "DLL", MB_OK);
}

void on_frame() {
    OutputDebugString("Example Frame");
}

void on_pre_begin_rendering() {
    OutputDebugString("Example Pre Begin Rendering");
}

void on_post_end_rendering() {
    OutputDebugString("Example Post End Rendering");
}

extern "C" __declspec(dllexport) void reframework_plugin_required_version(REFrameworkPluginVersion* version) {
    version->major = REFRAMEWORK_PLUGIN_VERSION_MAJOR;
    version->minor = REFRAMEWORK_PLUGIN_VERSION_MINOR;
    version->patch = REFRAMEWORK_PLUGIN_VERSION_PATCH;
}

extern "C" __declspec(dllexport) bool reframework_plugin_initialize(const REFrameworkPluginInitializeParam* param) {
    const auto functions = param->functions;
    functions->on_lua_state_created(on_lua_state_created);
    functions->on_lua_state_destroyed(on_lua_state_destroyed);
    functions->on_frame(on_frame);
    functions->on_pre_application_entry("BeginRendering", on_pre_begin_rendering);
    functions->on_post_application_entry("EndRendering", on_post_end_rendering);

    return true;
}
