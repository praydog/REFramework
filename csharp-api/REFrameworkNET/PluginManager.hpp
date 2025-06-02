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

internal:
    ref class PluginState;

    // Executed initially when we get loaded via LoadLibrary
    // which is not in the correct context to actually load the managed plugins
    static bool LoadPlugins_FromDefaultContext(const REFrameworkPluginInitializeParam* param);

    // meant to be executed in the correct context
    // after loading "ourselves" via System::Reflection::Assembly::LoadFrom
    static System::Collections::Generic::List<System::Reflection::Assembly^>^ LoadAssemblies(System::String^ path);
    static System::Collections::Generic::List<System::Reflection::Assembly^>^ LoadDependencies();
    static bool ShouldRecompile(System::String^ metadataPath, System::String^ currentGameHash, System::String^ currentFrameworkHash);
    static bool WriteMetadata(System::String^ metadataPath, System::String^ currentGameHash, System::String^ currentFrameworkHash);
    static void GenerateReferenceAssemblies(System::Collections::Generic::List<System::Reflection::Assembly^>^ deps);
    static bool LoadPlugins(uintptr_t param_raw);
    static bool LoadPlugins_FromSourceCode(uintptr_t param_raw, System::Collections::Generic::List<System::Reflection::Assembly^>^ deps);
    static bool LoadPlugins_FromDLLs(uintptr_t param_raw, System::Collections::Generic::List<System::Reflection::Assembly^>^ deps);
    static bool TriggerPluginLoad(PluginState^ state);
    static void UnloadPlugins();

    // This one is outside of a window context
    static void ImGuiCallback(::REFImGuiFrameCbData* data);
    delegate void ImGuiCallbackDelegate(::REFImGuiFrameCbData* data);

    // This one is in the middle of drawing REFramework's UI
    static void ImGuiDrawUICallback(::REFImGuiFrameCbData* data);
    delegate void ImGuiDrawUICallbackDelegate(::REFImGuiFrameCbData* data);

    static ImGuiCallbackDelegate^ s_imgui_callback_delegate{gcnew ImGuiCallbackDelegate(&ImGuiCallback)};
    static ImGuiDrawUICallbackDelegate^ s_imgui_draw_ui_callback_delegate{gcnew ImGuiDrawUICallbackDelegate(&ImGuiDrawUICallback)};

    static System::Collections::Generic::List<System::Reflection::Assembly^>^ s_dependencies{gcnew System::Collections::Generic::List<System::Reflection::Assembly^>()};

    // The main watcher
    static System::IO::FileSystemWatcher^ s_source_scripts_watcher{nullptr};

    // We also need a watcher list for symlinks that are in the directory
    static System::Collections::Generic::List<System::IO::FileSystemWatcher^>^ s_symlink_watchers{gcnew System::Collections::Generic::List<System::IO::FileSystemWatcher^>()};
    static bool s_wants_reload{false};
    static bool s_wants_reload_automatic{false};

    static void SetupFileWatcher();
    static void SetupIndividualFileWatcher(System::String^ p); // individual symlinks
    static void SetupSymlinkWatchers(System::String^ p); // all symlinks in a directory

    static void OnSourceScriptsChanged(System::Object^ sender, System::IO::FileSystemEventArgs^ e);
    static void BeginRendering();
    delegate void BeginRenderingDelegate();
    
    static BeginRenderingDelegate^ s_begin_rendering_delegate{gcnew BeginRenderingDelegate(&BeginRendering)};

    ref class PluginState {
    internal:
        PluginState(System::String^ path, bool is_dynamic) : script_path(path), is_dynamic(is_dynamic) 
        { 
        }

        ~PluginState() {
            Unload();
        }

        void Unload();
        bool SynchronousUnload();

        bool IsAlive() {
            return load_context_weak != nullptr && load_context_weak->IsAlive;
        }

        void DisplayOptions();

        PluginLoadContext^ load_context{gcnew PluginLoadContext()};
        System::WeakReference^ load_context_weak{gcnew System::WeakReference(load_context)};
        System::Reflection::Assembly^ assembly{nullptr};
        System::String^ script_path{nullptr}; // Either to a .cs file or a .dll file, don't think the assembly will contain the path so we need to keep it

        // TODO: Add a friendly name that the plugins can expose?

        bool is_dynamic{false};
    };

    static System::Collections::Generic::List<PluginState^>^ s_plugin_states{gcnew System::Collections::Generic::List<PluginState^>()};
    static System::Collections::Generic::List<PluginState^>^ s_plugin_states_to_remove{gcnew System::Collections::Generic::List<PluginState^>()};
    static bool s_auto_reload_plugins{true};
};
}