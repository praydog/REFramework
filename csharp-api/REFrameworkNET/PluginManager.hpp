#pragma once
#pragma managed

extern "C" {
    #include <reframework/API.h>
}

#include <cstdint>

#include "./API.hpp"

#include "PluginLoadContext.hpp"

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
    static System::Collections::Generic::List<System::Reflection::Assembly^>^ LoadDependencies();
    static void GenerateReferenceAssemblies(System::Collections::Generic::List<System::Reflection::Assembly^>^ deps);
    static bool LoadPlugins(uintptr_t param_raw);
    static bool LoadPlugins_FromSourceCode(uintptr_t param_raw, System::Collections::Generic::List<System::Reflection::Assembly^>^ deps);
    static void UnloadDynamicAssemblies();

    static System::Collections::Generic::List<System::Reflection::Assembly^>^ s_loaded_assemblies{gcnew System::Collections::Generic::List<System::Reflection::Assembly^>()};
    static System::Collections::Generic::List<System::Reflection::Assembly^>^ s_dynamic_assemblies{gcnew System::Collections::Generic::List<System::Reflection::Assembly^>()};

    static System::Collections::Generic::List<System::Reflection::Assembly^>^ s_dependencies{gcnew System::Collections::Generic::List<System::Reflection::Assembly^>()};

    static PluginLoadContext^ s_default_context{nullptr};

    // The main watcher
    static System::IO::FileSystemWatcher^ s_source_scripts_watcher{nullptr};

    // We also need a watcher list for symlinks that are in the directory
    static System::Collections::Generic::List<System::IO::FileSystemWatcher^>^ s_symlink_watchers{gcnew System::Collections::Generic::List<System::IO::FileSystemWatcher^>()};
    static bool s_wants_reload{false};

    static void SetupFileWatcher();
    static void SetupIndividualFileWatcher(System::String^ p); // individual symlinks
    static void SetupSymlinkWatchers(System::String^ p); // all symlinks in a directory

    static void OnSourceScriptsChanged(System::Object^ sender, System::IO::FileSystemEventArgs^ e);
    static void BeginRendering();
    delegate void BeginRenderingDelegate();
    static BeginRenderingDelegate^ s_begin_rendering_delegate{gcnew BeginRenderingDelegate(&BeginRendering)};
};
}