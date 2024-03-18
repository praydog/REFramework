#pragma once
#pragma managed

extern "C" {
    #include <reframework/API.h>
}

#include <cstdint>

#include "./API.hpp"

namespace REFrameworkNET {
private ref class PluginManager
{
private:
    // To stop these funcs from getting called more than once
    static bool s_initialized = false;
    static REFrameworkNET::API^ s_api_instance{};

public:
    static bool Entry(const REFrameworkPluginInitializeParam* param) {
        return LoadPlugins_FromDefaultContext(param);
    }

private:
    // Executed initially when we get loaded via LoadLibrary
    // which is not in the correct context to actually load the managed plugins
    static bool LoadPlugins_FromDefaultContext(const REFrameworkPluginInitializeParam* param);

    // meant to be executed in the correct context
    // after loading "ourselves" via System::Reflection::Assembly::LoadFrom
    static bool LoadPlugins(uintptr_t param_raw);
    static bool LoadPlugins_FromSourceCode(uintptr_t param_raw);
};
}