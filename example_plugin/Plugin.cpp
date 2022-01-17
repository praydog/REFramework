#include <Windows.h>

#include "API.hpp"

extern "C" __declspec(dllexport) void reframework_plugin_required_version(REFrameworkPluginVersion* version) {
    version->major = 1;
    version->minor = 0;
    version->patch = 0;
}

extern "C" __declspec(dllexport) bool reframework_plugin_initialize(const REFrameworkPluginInitializeParam* param) {
    MessageBox(0, "Hello, world!", "Message from plugin", MB_OK);
    return true;
}
