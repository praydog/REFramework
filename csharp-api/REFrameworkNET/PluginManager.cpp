#include <exception>
#include <filesystem>

#include "Attributes/Plugin.hpp"
#include "Attributes/MethodHook.hpp"
#include "Attributes/Callback.hpp"
#include "MethodHook.hpp"
#include "SystemString.hpp"
#include "NativePool.hpp"
#include "PluginManager.hpp"

using namespace System;
using namespace System::IO;
using namespace System::Reflection;
using namespace System::Collections::Generic;
using namespace System::Text::Json;
using namespace msclr::interop;

namespace REFrameworkNET {
    // Executed initially when we get loaded via LoadLibrary
    // which is not in the correct context to actually load the managed plugins
    bool PluginManager::LoadPlugins_FromDefaultContext(const REFrameworkPluginInitializeParam* param) try {
        if (s_initialized) {
            return true;
        }

        System::Console::WriteLine("Attempting to load plugins from initial context");

        // Make sure plugins that are loaded can find a reference to the current assembly
        // Even though "this" is loaded currently, its not in the right context to be found
        // So we have to load ourselves again, and call CSharpAPIImpl.ManagedImpl via a dynamic lookup
        auto self = System::Reflection::Assembly::LoadFrom(System::Reflection::Assembly::GetExecutingAssembly()->Location);

        if (self == nullptr) {
            System::Console::WriteLine("Failed to load self");
            return false;
        }

        // Get CSharpAPIImpl type
        auto type = self->GetType("REFrameworkNET.PluginManager");

        if (type == nullptr) {
            System::Console::WriteLine("Failed to get type REFrameworkNET.PluginManager");
            return false;
        }

        // Get LoadPlugins method
        auto method = type->GetMethod("LoadPlugins", System::Reflection::BindingFlags::Static | System::Reflection::BindingFlags::NonPublic);

        if (method == nullptr) {
            System::Console::WriteLine("Failed to get method ManagedImplPartTwo");
            return false;
        }

        // Invoke LoadPlugins method
        System::Console::WriteLine("Invoking LoadPlugins...");
        method->Invoke(nullptr, gcnew array<Object^>{reinterpret_cast<uintptr_t>(param)});
        s_initialized = true;

        return true;
    }  catch (System::Reflection::ReflectionTypeLoadException^ ex) {
        auto loaderExceptions = ex->LoaderExceptions;
        for each (Exception^ innerEx in loaderExceptions) {
            System::Console::WriteLine(innerEx->Message);
        }

        System::Console::WriteLine(ex->Message);
        return false;
    } catch (System::Exception^ e) {
        System::Console::WriteLine(e->Message);

        // log stack
        auto ex = e;
        while (ex != nullptr) {
            System::Console::WriteLine(ex->StackTrace);
            ex = ex->InnerException;
        }

        return false;
    } catch (const std::exception& e) {
        System::Console::WriteLine(gcnew System::String(e.what()));
        return false;
    } catch (...) {
        System::Console::WriteLine("Unknown exception caught");
        return false;
    }

    List<System::Reflection::Assembly^>^ PluginManager::LoadAssemblies(System::String^ dependencies_dir) {
        REFrameworkNET::API::LogInfo("Loading Assemblies from " + dependencies_dir + "...");

        //const auto dependencies_path = std::filesystem::current_path() / "reframework" / "plugins" / "managed" / "dependencies";

        std::filesystem::create_directories(msclr::interop::marshal_as<std::string>(dependencies_dir));

        auto files = System::IO::Directory::GetFiles(dependencies_dir, "*.dll");

        auto assemblies = gcnew List<System::Reflection::Assembly^>();

        if (files->Length == 0) {
            REFrameworkNET::API::LogInfo("No dependencies found in " + dependencies_dir);
            return assemblies;
        }

        REFrameworkNET::API::LogInfo("Loading dependencies from " + dependencies_dir + "...");
        
        for each (System::String^ file in files) {
            try {
                REFrameworkNET::API::LogInfo("Loading dependency " + file + "...");
                if (auto assem = System::Reflection::Assembly::LoadFrom(file); assem != nullptr) {
                    assemblies->Add(assem);
                    REFrameworkNET::API::LogInfo("Loaded " + file);
                }
            } catch(System::Exception^ e) {
                REFrameworkNET::API::LogInfo(e->Message);
                // log stack
                auto ex = e;
                while (ex != nullptr) {
                    REFrameworkNET::API::LogInfo(ex->StackTrace);
                    ex = ex->InnerException;
                }
            } catch(const std::exception& e) {
                REFrameworkNET::API::LogInfo(gcnew System::String(e.what()));
            } catch(...) {
                REFrameworkNET::API::LogInfo("Unknown exception caught while loading dependency " + file);
            }
        }

        return assemblies;
    }

    List<System::Reflection::Assembly^>^ PluginManager::LoadDependencies() {
        const auto dependencies_path = std::filesystem::current_path() / "reframework" / "plugins" / "managed" / "dependencies";
        return LoadAssemblies(gcnew System::String(dependencies_path.wstring().c_str()));
    }

    bool PluginManager::ShouldRecompile(System::String^ metadataPath, System::String^ currentGameHash, System::String^ currentFrameworkHash) {
        if (!File::Exists(metadataPath)) {
            return true; // No metadata file = force recompile
        }
    
        try {
            auto jsonText = File::ReadAllText(metadataPath);
            auto jsonDocument = JsonDocument::Parse(jsonText);
            auto root = jsonDocument->RootElement;
    
            System::String^ previousGameHash = root.GetProperty("GameExecutableHash").GetString();
            System::String^ previousFrameworkHash = root.GetProperty("REFrameworkNetHash").GetString();
    
            return (previousGameHash != currentGameHash || previousFrameworkHash != currentFrameworkHash);
        } catch (Exception^ ex) {
            REFrameworkNET::API::LogError("Error reading metadata.json: " + ex->Message);
            return true; // Force recompile if there's an issue with the file
        }
    }

    bool PluginManager::WriteMetadata(System::String^ metadataPath, System::String^ currentGameHash, System::String^ currentFrameworkHash) {
        auto metadata = gcnew System::Collections::Generic::Dictionary<System::String^, System::String^>();
        metadata->Add("GameExecutableHash", currentGameHash);
        metadata->Add("REFrameworkNetHash", currentFrameworkHash);
    
        try {
            auto jsonText = JsonSerializer::Serialize(metadata);
            File::WriteAllText(metadataPath, jsonText);
            System::Console::WriteLine("Wrote metadata.json");
            System::Console::WriteLine(jsonText);
            return true;
        } catch (Exception^ ex) {
            REFrameworkNET::API::LogError("Error writing metadata.json: " + ex->Message);
            return false;
        }
    }

    void PluginManager::GenerateReferenceAssemblies(List<System::Reflection::Assembly^>^ deps) {
        REFrameworkNET::API::LogInfo("Generating reference assemblies...");

        auto generatedFolder = std::filesystem::current_path() / "reframework" / "plugins" / "managed" / "generated";
        std::filesystem::create_directories(generatedFolder);

        auto pathToGame = System::Diagnostics::Process::GetCurrentProcess()->MainModule->FileName;
        auto gameHash = REFrameworkNET::HashHelper::ComputeSHA256(pathToGame);

        auto pathToFramework = System::Reflection::Assembly::GetExecutingAssembly()->Location;
        auto frameworkHash = REFrameworkNET::HashHelper::ComputeSHA256(pathToFramework);

        System::Console::WriteLine("Game hash: " + gameHash);
        System::Console::WriteLine("Framework hash: " + frameworkHash);
        System::Console::WriteLine("Game path: " + pathToGame);
        System::Console::WriteLine("Framework path: " + pathToFramework);

        auto metadataPath = generatedFolder / "metadata.json";
        auto files = System::IO::Directory::GetFiles(gcnew System::String(generatedFolder.wstring().c_str()), "*.dll");

        if (files->Length > 0 && !ShouldRecompile(gcnew System::String(metadataPath.wstring().c_str()), gameHash, frameworkHash)) {
            REFrameworkNET::API::LogInfo("No need to recompile reference assemblies");
            
            // Loop through all DLLs in the generated folder and load them instead
            for each (System::String^ file in files) {
                try {
                    REFrameworkNET::API::LogInfo("Loading generated assembly " + file + "...");
                    if (auto assem = System::Reflection::Assembly::LoadFrom(file); assem != nullptr) {
                        deps->Add(assem);
                        REFrameworkNET::API::LogInfo("Loaded " + file);
                    }
                } catch(System::Exception^ e) {
                    REFrameworkNET::API::LogInfo(e->Message);
                    // log stack
                    auto ex = e;
                    while (ex != nullptr) {
                        REFrameworkNET::API::LogInfo(ex->StackTrace);
                        ex = ex->InnerException;
                    }
                } catch(const std::exception& e) {
                    REFrameworkNET::API::LogInfo(gcnew System::String(e.what()));
                } catch(...) {
                    REFrameworkNET::API::LogInfo("Unknown exception caught while loading generated assembly " + file);
                }
            }

            return;
        }

        // Look for AssemblyGenerator class in the loaded deps
        for each (System::Reflection::Assembly^ a in deps) {
            if (auto generator = a->GetType("REFrameworkNET.AssemblyGenerator"); generator != nullptr) {
                // Look for Main method in the AssemblyGenerator class
                auto mainMethod = generator->GetMethod(
                                            "Main", 
                                            System::Reflection::BindingFlags::Static | System::Reflection::BindingFlags::Public);

               if (mainMethod != nullptr) {
                    REFrameworkNET::API::LogInfo("Found AssemblyGenerator.Main in " + a->Location);

                    auto result = (List<Compiler::DynamicAssemblyBytecode^>^)mainMethod->Invoke(nullptr, nullptr);

                    if (result->Count != 0) {
                        WriteMetadata(gcnew System::String(metadataPath.wstring().c_str()), gameHash, frameworkHash);
                    }

                    // Append the generated assemblies to the list of deps
                    for each (Compiler::DynamicAssemblyBytecode^ bytes in result) {
                        REFrameworkNET::API::LogInfo("Adding generated assembly to deps...");

                        std::string assemblyName = msclr::interop::marshal_as<std::string>(bytes->AssemblyName);
                        assemblyName = "REFramework.NET." + assemblyName + ".dll";

                        auto path = generatedFolder / assemblyName;
                        System::IO::File::WriteAllBytes(gcnew System::String(path.wstring().c_str()), bytes->Bytecode);
                        REFrameworkNET::API::LogInfo("Wrote generated assembly to " + gcnew System::String(path.wstring().c_str()));

                        auto assem = System::Reflection::Assembly::LoadFrom(gcnew System::String(path.wstring().c_str()));

                        if (assem != nullptr) {
                            REFrameworkNET::API::LogInfo("Loaded generated assembly with " + assem->GetTypes()->Length + " types");
                            deps->Add(assem);
                        }
                    }
                    
                    break;
                }
            }
        }

        // Unclog the GC after all that
        System::GC::Collect(0, System::GCCollectionMode::Forced, false);
        System::GC::Collect(1, System::GCCollectionMode::Forced, false);
        System::GC::Collect(2, System::GCCollectionMode::Forced, false);
    }

    // meant to be executed in the correct context
    // after loading "ourselves" via System::Reflection::Assembly::LoadFrom
    bool PluginManager::LoadPlugins(uintptr_t param_raw) try {
        System::Console::WriteLine("LoadPlugins called");

        if (PluginManager::s_initialized) {
            System::Console::WriteLine("Already initialized");
            return true;
        }

        PluginManager::s_initialized = true;

        auto self = System::Reflection::Assembly::GetExecutingAssembly();

        // Look for any DLLs in the "managed" directory, load them, then call a function in them (REFrameworkPlugin.Main)
        // This is useful for loading C# plugins
        // Create the REFramework::API class first though (managed)
        if (PluginManager::s_api_instance == nullptr) {
            PluginManager::s_api_instance = gcnew REFrameworkNET::API(param_raw);
            System::Console::WriteLine("Created API instance");
        }

        // Must be set up before we load anything as it sets up the LoadLibraryExW hook for cimgui
        auto imgui_callback_c = System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(s_imgui_callback_delegate).ToPointer();
        auto imgui_draw_ui_callback_c = System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(s_imgui_draw_ui_callback_delegate).ToPointer();
        auto api_fns = REFrameworkNET::API::GetNativeImplementation()->param()->functions;
        api_fns->on_imgui_frame((::REFOnImGuiFrameCb)imgui_callback_c);
        api_fns->on_imgui_draw_ui((::REFOnImGuiFrameCb)imgui_draw_ui_callback_c);

        s_dependencies = LoadDependencies(); // Pre-loads DLLs in the dependencies folder before loading the plugins

        try {
            GenerateReferenceAssemblies(s_dependencies);
        } catch(System::Exception^ e) {
            REFrameworkNET::API::LogError("Could not generate reference assemblies: " + e->Message);

            auto ex = e;
            while (ex != nullptr) {
                REFrameworkNET::API::LogError(ex->StackTrace);
                ex = ex->InnerException;
            }
        } catch (const std::exception& e) {
            REFrameworkNET::API::LogError("Could not generate reference assemblies: " + gcnew System::String(e.what()));
        } catch (...) {
            REFrameworkNET::API::LogError("Could not generate reference assemblies: Unknown exception caught");
        }

        // Try-catch because the user might not have the compiler
        // dependencies in the plugins directory
        try {
            LoadPlugins_FromSourceCode(param_raw, s_dependencies);
        } catch (System::Exception^ e) {
            REFrameworkNET::API::LogError("Could not load plugins from source code: " + e->Message);

            auto ex = e;
            while (ex != nullptr) {
                REFrameworkNET::API::LogError(ex->StackTrace);
                ex = ex->InnerException;
            }
        } catch (const std::exception& e) {
            REFrameworkNET::API::LogError("Could not load plugins from source code: " + gcnew System::String(e.what()));
        } catch (...) {
            REFrameworkNET::API::LogError("Could not load plugins from source code: Unknown exception caught");
        }

        System::Console::WriteLine("Continue with managed plugins...");

        LoadPlugins_FromDLLs(param_raw, s_dependencies);

        return true;
    } catch(System::Exception^ e) {
        System::Console::WriteLine(e->Message);
        // log stack
        auto ex = e;
        while (ex != nullptr) {
            System::Console::WriteLine(ex->StackTrace);
            ex = ex->InnerException;
        }

        return false;
    } catch(const std::exception& e) {
        System::Console::WriteLine(gcnew System::String(e.what()));
        return false;
    } catch(...) {
        System::Console::WriteLine("Unknown exception caught");
        return false;
    }

    bool PluginManager::TriggerPluginLoad(PluginManager::PluginState^ state) {
        auto assem = state->assembly;

        if (assem == nullptr) {
            REFrameworkNET::API::LogError("Failed to load assembly from " + state->script_path);
            return false;
        }

        bool ever_found = false;

        // Iterate through all types in the assembly
        for each (Type^ type in assem->GetTypes()) {
            array<System::Reflection::MethodInfo^>^ methods = type->GetMethods(System::Reflection::BindingFlags::Static | System::Reflection::BindingFlags::Public | System::Reflection::BindingFlags::NonPublic);

            for each (System::Reflection::MethodInfo^ method in methods) {
                // EntryPoint attribute
                array<Object^>^ attributes = method->GetCustomAttributes(REFrameworkNET::Attributes::PluginEntryPoint::typeid, true);

                if (attributes->Length > 0) try {
                    REFrameworkNET::API::LogInfo("Found PluginEntryPoint in " + method->Name + " in " + type->FullName);
                    method->Invoke(nullptr, nullptr);
                    ever_found = true;
                    continue;
                } catch(System::Exception^ e) {
                    REFrameworkNET::API::LogError("Failed to invoke PluginEntryPoint in " + method->Name + " in " + type->FullName + ": " + e->Message);
                    continue;
                } catch(const std::exception& e) {
                    REFrameworkNET::API::LogError("Failed to invoke PluginEntryPoint in " + method->Name + " in " + type->FullName + ": " + gcnew System::String(e.what()));
                    continue;
                } catch(...) {
                    REFrameworkNET::API::LogError("Failed to invoke PluginEntryPoint in " + method->Name + " in " + type->FullName + ": Unknown exception caught");
                    continue;
                }

                // MethodHook attribute(s)
                attributes = method->GetCustomAttributes(REFrameworkNET::Attributes::MethodHookAttribute::typeid, true);

                if (attributes->Length > 0) try {
                    REFrameworkNET::API::LogInfo("Found MethodHook in " + method->Name + " in " + type->FullName);
                    auto hookAttr = (REFrameworkNET::Attributes::MethodHookAttribute^)attributes[0];
      
                    if (hookAttr->Install(method)) {
                        REFrameworkNET::API::LogInfo("Installed MethodHook in " + method->Name + " in " + type->FullName);
                    } else {
                        REFrameworkNET::API::LogError("Failed to install MethodHook in " + method->Name + " in " + type->FullName);
                    }
                    continue;
                } catch(System::Exception^ e) {
                    REFrameworkNET::API::LogError("Failed to install MethodHook in " + method->Name + " in " + type->FullName + ": " + e->Message);
                    continue;
                } catch(const std::exception& e) {
                    REFrameworkNET::API::LogError("Failed to install MethodHook in " + method->Name + " in " + type->FullName + ": " + gcnew System::String(e.what()));
                    continue;
                } catch(...) {
                    REFrameworkNET::API::LogError("Failed to install MethodHook in " + method->Name + " in " + type->FullName + ": Unknown exception caught");
                    continue;
                }

                // Callback attribute(s)
                attributes = method->GetCustomAttributes(REFrameworkNET::Attributes::CallbackAttribute::typeid, true);

                if (attributes->Length > 0) try {
                    REFrameworkNET::API::LogInfo("Found Callback in " + method->Name + " in " + type->FullName);
                    auto callbackAttr = (REFrameworkNET::Attributes::CallbackAttribute^)attributes[0];
                    callbackAttr->Install(method);
                    continue;
                } catch(System::Exception^ e) {
                    REFrameworkNET::API::LogError("Failed to install Callback in " + method->Name + " in " + type->FullName + ": " + e->Message);
                    continue;
                } catch(const std::exception& e) {
                    REFrameworkNET::API::LogError("Failed to install Callback in " + method->Name + " in " + type->FullName + ": " + gcnew System::String(e.what()));
                    continue;
                } catch(...) {
                    REFrameworkNET::API::LogError("Failed to install Callback in " + method->Name + " in " + type->FullName + ": Unknown exception caught");
                    continue;
                }
            }
        }

        return ever_found;
    }

    void PluginManager::OnSourceScriptsChanged(System::Object^ sender, System::IO::FileSystemEventArgs^ e) {
        System::Console::WriteLine("Source scripts changed");
        System::Console::WriteLine("File " + e->FullPath + " " + e->ChangeType.ToString());

        if (e->ChangeType == System::IO::WatcherChangeTypes::Created) {
            // Add a new symlink watcher if it's a symlink
            if ((System::IO::File::GetAttributes(e->FullPath) & System::IO::FileAttributes::ReparsePoint) == System::IO::FileAttributes::ReparsePoint) {
                auto link = (gcnew System::IO::FileInfo(e->FullPath))->LinkTarget;

                if (link != nullptr) {
                    System::Console::WriteLine("Found symlink " + link);
                    SetupIndividualFileWatcher(link);
                }
            }
        }

        s_wants_reload_automatic = true;
    }

    void PluginManager::SetupIndividualFileWatcher(System::String^ real_path) {
        auto directory = System::IO::Path::GetDirectoryName(real_path);
        auto filename = System::IO::Path::GetFileName(real_path);

        // Make sure we don't already have a watcher for this symlink
        for each (System::IO::FileSystemWatcher^ watcher in PluginManager::s_symlink_watchers) {
            if (watcher->Path == directory && watcher->Filter == filename) {
                System::Console::WriteLine("Already watching file " + real_path);
                return;
            }
        }

        auto watcher = gcnew System::IO::FileSystemWatcher(directory);
        watcher->Filter = filename;
        watcher->Changed += gcnew System::IO::FileSystemEventHandler(OnSourceScriptsChanged);
        watcher->Created += gcnew System::IO::FileSystemEventHandler(OnSourceScriptsChanged);
        watcher->Deleted += gcnew System::IO::FileSystemEventHandler(OnSourceScriptsChanged);
        watcher->EnableRaisingEvents = true;

        PluginManager::s_symlink_watchers->Add(watcher);
    }

    void PluginManager::SetupSymlinkWatchers(System::String^ p) {
        auto files = System::IO::Directory::GetFiles(p, "*.cs");

        for each (System::String^ file in files) {
            // Check if symlink
            if ((System::IO::File::GetAttributes(file) & System::IO::FileAttributes::ReparsePoint) == System::IO::FileAttributes::None) {
                continue;
            }

            // Resolve symlink to real path
            auto real_path = (gcnew System::IO::FileInfo(file))->LinkTarget;

            if (real_path == nullptr) {
                continue; // only want to watch symlinks.
            }

            System::Console::WriteLine("Found symlink " + real_path);
            SetupIndividualFileWatcher(real_path);
        }
    }

    void PluginManager::SetupFileWatcher() {
        if (PluginManager::s_source_scripts_watcher != nullptr) {
            return;
        }
        
        System::Console::WriteLine("Setting up source scripts watcher...");
        const auto plugins_path = std::filesystem::current_path() / "reframework" / "plugins";
        const auto cs_files_path = plugins_path / "source";

        auto p = gcnew System::String(cs_files_path.wstring().c_str());
        PluginManager::s_source_scripts_watcher = gcnew System::IO::FileSystemWatcher(p);
        PluginManager::s_source_scripts_watcher->Filter = "*.cs";
        PluginManager::s_source_scripts_watcher->IncludeSubdirectories = true;
        PluginManager::s_source_scripts_watcher->Changed += gcnew System::IO::FileSystemEventHandler(OnSourceScriptsChanged);
        PluginManager::s_source_scripts_watcher->Created += gcnew System::IO::FileSystemEventHandler(OnSourceScriptsChanged);
        PluginManager::s_source_scripts_watcher->Deleted += gcnew System::IO::FileSystemEventHandler(OnSourceScriptsChanged);
        PluginManager::s_source_scripts_watcher->EnableRaisingEvents = true;

        // Create individual watchers for each file in the directory as well.
        // This is because the main watcher doesn't pick up on changes to symlinks
        SetupSymlinkWatchers(p);
    }

    void PluginManager::BeginRendering() {
        bool should_reload = s_wants_reload || (s_wants_reload_automatic && s_auto_reload_plugins);
        if (should_reload) {
            s_wants_reload = false;
            s_wants_reload_automatic = false;

            PluginManager::UnloadPlugins();

            try {
                PluginManager::LoadPlugins_FromSourceCode(0, s_dependencies);
            } catch (System::Exception^ e) {
                REFrameworkNET::API::LogError("Failed to reload plugins: " + e->Message);

                auto ex = e;
                while (ex != nullptr) {
                    REFrameworkNET::API::LogError(ex->StackTrace);
                    ex = ex->InnerException;
                }
            } catch (const std::exception& e) {
                REFrameworkNET::API::LogError("Failed to reload plugins: " + gcnew System::String(e.what()));
            } catch (...) {
                REFrameworkNET::API::LogError("Failed to reload plugins: Unknown exception caught");
            }

            PluginManager::LoadPlugins_FromDLLs(0, s_dependencies);
        }

        SetupFileWatcher();
    }

    bool PluginManager::LoadPlugins_FromDLLs(uintptr_t param_raw, System::Collections::Generic::List<System::Reflection::Assembly^>^ deps) try {
        if (PluginManager::s_api_instance == nullptr) {
            PluginManager::s_api_instance = gcnew REFrameworkNET::API(param_raw);
        }

        const auto managed_path = std::filesystem::current_path() / "reframework" / "plugins" / "managed";
        std::filesystem::create_directories(managed_path);

        System::String^ managed_dir = gcnew System::String(managed_path.wstring().c_str());
        auto files = System::IO::Directory::GetFiles(managed_dir, "*.dll");

        bool ever_found = false;

        if (files->Length != 0) {
            bool ever_found = false;

            for each (System::String^ file in files) {
                Console::WriteLine(file);
                auto state = gcnew PluginState(file, false);

                state->assembly = state->load_context->LoadFromAssemblyPath(file);

                if (state->assembly == nullptr) {
                    REFrameworkNET::API::LogError("Failed to load assembly from " + file);
                    state->Unload();
                    continue;
                }

                PluginManager::s_plugin_states->Add(state);

                if (TriggerPluginLoad(state)) {
                    ever_found = true;
                }
            }

            if (!ever_found) {
                REFrameworkNET::API::LogInfo("No Main method found in any DLLs in " + managed_dir);
            }
        } else {
            REFrameworkNET::API::LogInfo("No DLLs found in " + managed_dir);
        }

        return ever_found;
    } catch(System::Exception^ e) {
        REFrameworkNET::API::LogError(e->Message);

        // log stack
        auto ex = e;
        while (ex != nullptr) {
            REFrameworkNET::API::LogError(ex->StackTrace);
            ex = ex->InnerException;
        }

        return false;
    } catch(const std::exception& e) {
        REFrameworkNET::API::LogError(gcnew System::String(e.what()));
        return false;
    } catch(...) {
        REFrameworkNET::API::LogError("Unknown exception caught while loading DLLs");
        return false;
    }


    bool PluginManager::LoadPlugins_FromSourceCode(uintptr_t param_raw, System::Collections::Generic::List<System::Reflection::Assembly^>^ deps) try {
        if (PluginManager::s_api_instance == nullptr) {
            PluginManager::s_api_instance = gcnew REFrameworkNET::API(param_raw);
        }

        REFrameworkNET::API::LogInfo("Attempting to load plugins from source code...");

        const auto plugins_path = std::filesystem::current_path() / "reframework" / "plugins";
        const auto cs_files_path = plugins_path / "source";
        std::filesystem::create_directories(cs_files_path);

        // Set up a filesystem watcher for the source folder
        if (PluginManager::s_source_scripts_watcher == nullptr) {
            auto ptr = System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(s_begin_rendering_delegate).ToPointer();
            REFrameworkNET::API::GetNativeImplementation()->param()->functions->on_pre_application_entry("BeginRendering", (::REFOnPreApplicationEntryCb)ptr);
        }

        System::String^ cs_files_dir = gcnew System::String(cs_files_path.wstring().c_str());

        bool ever_found = false;

        auto files = System::IO::Directory::GetFiles(cs_files_dir, "*.cs");

        if (files->Length == 0) {
            //API::get()->log_error("No C# files found in %s", csFilesDir);
            return false;
        }

        auto self = System::Reflection::Assembly::LoadFrom(System::Reflection::Assembly::GetExecutingAssembly()->Location);

        for each (System::String^ file in files) {
            System::Console::WriteLine(file);

            // Compile the C# file, and then call a function in it (REFrameworkPlugin.Main)
            // This is useful for loading C# plugins that don't want to be compiled into a DLL
            auto bytecode = REFrameworkNET::Compiler::Compile(file, self, deps);

            if (bytecode == nullptr) {
                REFrameworkNET::API::LogError("Failed to compile " + file);
                continue;
            }

            PluginState^ state = gcnew PluginState(file, true);
            state->assembly = state->load_context->LoadFromStream(gcnew System::IO::MemoryStream(bytecode));

            if (state->assembly == nullptr) {
                REFrameworkNET::API::LogError("Failed to load assembly from " + file);
                state->Unload();
                continue;
            }

            //s_dynamic_assemblies->Add(assem);
            s_plugin_states->Add(state);

            REFrameworkNET::API::LogInfo("Compiled " + file);

            if (TriggerPluginLoad(state)) {
                ever_found = true;
            }
        }

        if (!ever_found) {
            Console::WriteLine("No C# plugins with an entry point found in " + cs_files_dir);
        }

        return true;
    } catch(System::Exception^ e) {
        REFrameworkNET::API::LogError(e->Message);

        // log stack
        auto ex = e;
        while (ex != nullptr) {
            REFrameworkNET::API::LogError(ex->StackTrace);
            ex = ex->InnerException;
        }

        return false;
    } catch(const std::exception& e) {
        REFrameworkNET::API::LogError(gcnew System::String(e.what()));
        return false;
    } catch(...) {
        REFrameworkNET::API::LogError("Unknown exception caught while compiling C# files");
        return false;
    }

    void PluginManager::UnloadPlugins() {
        if (PluginManager::s_plugin_states == nullptr || PluginManager::s_plugin_states->Count == 0) {
            REFrameworkNET::API::LogInfo("No plugins to unload");
			return;
		}

        REFrameworkNET::API::LogInfo("Unloading dynamic assemblies...");

        //for each (System::Reflection::Assembly ^ assem in PluginManager::s_dynamic_assemblies) {
        for each (PluginState^ state in PluginManager::s_plugin_states) {
            try {
                auto assem = state->assembly;
                if (assem == nullptr) {
                    continue;
                }

                auto path = state->script_path;
                REFrameworkNET::API::LogInfo("Attempting to initiate first phase unload of " + state->script_path);

				// Look for the PluginExitPoint attribute in the assembly
                for each (Type ^ t in assem->GetTypes()) {
                    auto methods = t->GetMethods(System::Reflection::BindingFlags::Static | System::Reflection::BindingFlags::Public | System::Reflection::BindingFlags::NonPublic);

                    for each (System::Reflection::MethodInfo^ method in methods) {
                        array<Object^>^ attributes = method->GetCustomAttributes(REFrameworkNET::Attributes::PluginExitPoint::typeid, true);

                        if (attributes->Length > 0) {
                            REFrameworkNET::API::LogInfo("Unloading plugin by calling " + method->Name + " in " + t->FullName);
                            method->Invoke(nullptr, nullptr);
                        }
                    }
				}

                state->Unload();
            }
            catch (System::Exception^ e) {
				REFrameworkNET::API::LogError("Failed to unload plugin: " + e->Message);
            }
            catch (const std::exception& e) {
				REFrameworkNET::API::LogError("Failed to unload plugin: " + gcnew System::String(e.what()));
            }
            catch (...) {
				REFrameworkNET::API::LogError("Unknown exception caught while unloading plugin");
			}
		}

        System::GC::Collect();
        System::GC::WaitForPendingFinalizers();
        System::Threading::Thread::Sleep(10);

        bool all_collected_final = false;

        // And now, on the third pass, we wait for the load contexts to be collected (to a reasonable extent)
        for (int i = 0; i < 10; i++) {
            bool all_collected = true;

            // If any of the load contexts are still alive, we wait a bit and try again
            for each (PluginState^ state in PluginManager::s_plugin_states) {
                if (state->IsAlive()) {
                    System::GC::Collect();
                    System::GC::WaitForPendingFinalizers();
                    System::Threading::Thread::Sleep(10);
                    all_collected = false;
                    break;
                } else {
                    REFrameworkNET::API::LogInfo("Successfully unloaded " + state->script_path);
                }
            }

            // If we've gone through all the load contexts and none are alive, we're done
            if (all_collected) {
                all_collected_final = true;
                break;
            }
        }

        if (!all_collected_final) {
            REFrameworkNET::API::LogError("Failed to unload all plugins");
        } else {
            REFrameworkNET::API::LogInfo("Successfully unloaded all plugins");
        }

        s_plugin_states->Clear();
    }

    void PluginManager::ImGuiCallback(::REFImGuiFrameCbData* data) {
        try {
            Callbacks::ImGuiRender::TriggerPre();
        } catch (System::Exception^ e) {
            REFrameworkNET::API::LogError("Failed to trigger ImGuiRender::Pre: " + e->Message);
        } catch (const std::exception& e) {
            REFrameworkNET::API::LogError("Failed to trigger ImGuiRender::Pre: " + gcnew System::String(e.what()));
        } catch (...) {
            REFrameworkNET::API::LogError("Unknown exception caught while triggering ImGuiRender::Pre");
        }

        try {
            Callbacks::ImGuiRender::TriggerPost();
        } catch (System::Exception^ e) {
            REFrameworkNET::API::LogError("Failed to trigger ImGuiRender::Post: " + e->Message);
        } catch (const std::exception& e) {
            REFrameworkNET::API::LogError("Failed to trigger ImGuiRender::Post: " + gcnew System::String(e.what()));
        } catch (...) {
            REFrameworkNET::API::LogError("Unknown exception caught while triggering ImGuiRender::Post");
        }
    }

    void PluginManager::ImGuiDrawUICallback(::REFImGuiFrameCbData* data) {
        // Draw our REFramework.NET menu which has buttons like reload scripts
        ImGuiNET::ImGui::PushID("REFramework.NET");
        if (ImGuiNET::ImGui::CollapsingHeader("REFramework.NET")) {
            if (ImGuiNET::ImGui::Button("Reload Scripts")) {
                s_wants_reload = true;
            }

            ImGuiNET::ImGui::SameLine();

            if (ImGuiNET::ImGui::Button("Unload Scripts")) {
                PluginManager::UnloadPlugins();
            }

            ImGuiNET::ImGui::Checkbox("Auto Reload", s_auto_reload_plugins);

            if (ImGuiNET::ImGui::TreeNode("Debug Stats")) {
                if (ImGuiNET::ImGui::TreeNode("Garbage Collection")) {
                    REFrameworkNET::GarbageCollectionDisplay::Render();
                    ImGuiNET::ImGui::TreePop();
                }

                ManagedObject::Cache<ManagedObject>::DisplayStats();
                ManagedObject::Cache<SystemString>::DisplayStats();
                NativePool<TypeDefinition>::DisplayStats();
                NativePool<Method>::DisplayStats();
                NativePool<Field>::DisplayStats();
                UnifiedObject::ProxyPool::DisplayStats();

                ImGuiNET::ImGui::TreePop();
            }
            
            for each (PluginState^ state in PluginManager::s_plugin_states) {
                state->DisplayOptions();
            }

            for each (PluginState^ state in PluginManager::s_plugin_states_to_remove) {
                PluginManager::s_plugin_states->Remove(state);
            }

            PluginManager::s_plugin_states_to_remove->Clear();
        }

        if (ImGuiNET::ImGui::CollapsingHeader("REFramework.NET Script Generated UI")) {
            try {
                Callbacks::ImGuiDrawUI::TriggerPre();
            } catch (System::Exception^ e) {
                REFrameworkNET::API::LogError("Failed to trigger ImGuiDrawUI::Pre: " + e->Message);
            } catch (const std::exception& e) {
                REFrameworkNET::API::LogError("Failed to trigger ImGuiDrawUI::Pre: " + gcnew System::String(e.what()));
            } catch (...) {
                REFrameworkNET::API::LogError("Unknown exception caught while triggering ImGuiDrawUI::Pre");
            }

            try {
                Callbacks::ImGuiDrawUI::TriggerPost();
            } catch (System::Exception^ e) {
                REFrameworkNET::API::LogError("Failed to trigger ImGuiDrawUI::Post: " + e->Message);
            } catch (const std::exception& e) {
                REFrameworkNET::API::LogError("Failed to trigger ImGuiDrawUI::Post: " + gcnew System::String(e.what()));
            } catch (...) {
                REFrameworkNET::API::LogError("Unknown exception caught while triggering ImGuiDrawUI::Post");
            }
        }

        ImGuiNET::ImGui::PopID();
    }

    void PluginManager::PluginState::Unload() {
        if (load_context != nullptr) {
            ManagedObject::ShuttingDown = true;
            ManagedObject::CleanupKnownCaches();

            REFrameworkNET::Callbacks::Impl::UnsubscribeAssembly(assembly);
            REFrameworkNET::MethodHook::UnsubscribeAssembly(assembly);

            load_context->Unload();
            load_context = nullptr;
            assembly = nullptr;

            System::GC::Collect();
            System::GC::WaitForPendingFinalizers();

            ManagedObject::ShuttingDown = false;
        }
    }

    bool PluginManager::PluginState::SynchronousUnload() {
        Unload();

        for (int i = 0; i < 10; ++i) {
            if (!IsAlive()) {
                return true;
            }

            System::GC::Collect();
            System::GC::WaitForPendingFinalizers();
            System::Threading::Thread::Sleep(10);
        }

        return !IsAlive();
    }

    void PluginManager::PluginState::DisplayOptions() {
        if (ImGuiNET::ImGui::TreeNode(System::IO::Path::GetFileName(script_path))) {
            if (ImGuiNET::ImGui::Button("Unload")) {
                if (SynchronousUnload()) {
                    REFrameworkNET::API::LogInfo("Successfully unloaded " + script_path);
                    // Remove this state from the list
                    //PluginManager::s_plugin_states->Remove(this);
                    s_plugin_states_to_remove->Add(this);
                } else {
                    REFrameworkNET::API::LogError("Failed to unload " + script_path);
                }
            }

            ImGuiNET::ImGui::TreePop();
        }
    }
}