#pragma managed

extern "C" {
    #include <reframework/API.h>
}

#include <sstream>
#include <mutex>
#include <filesystem>

#include <Windows.h>
#include <msclr/marshal_cppstd.h>

#include "API.hpp"
#include "PluginManager.hpp"

#include "Plugin.hpp"

using namespace reframework;
using namespace System;
using namespace System::Runtime;

extern "C" __declspec(dllexport) bool reframework_plugin_initialize(const REFrameworkPluginInitializeParam* param) {
    // Create a console
    AllocConsole();

    return REFrameworkNET::PluginManager::Entry(param);
}

extern "C" __declspec(dllexport) void reframework_plugin_required_version(REFrameworkPluginVersion* version) {
    version->major = REFRAMEWORK_PLUGIN_VERSION_MAJOR;
    version->minor = REFRAMEWORK_PLUGIN_VERSION_MINOR;
    version->patch = REFRAMEWORK_PLUGIN_VERSION_PATCH;
}
