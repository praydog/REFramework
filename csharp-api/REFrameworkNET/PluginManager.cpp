#include <exception>
#include <filesystem>

#include "Attributes/Plugin.hpp"
#include "PluginManager.hpp"

using namespace System;
using namespace System::IO;
using namespace System::Reflection;
using namespace System::Collections::Generic;
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

    void PluginManager::GenerateReferenceAssemblies(List<System::Reflection::Assembly^>^ deps) {
        REFrameworkNET::API::LogInfo("Generating reference assemblies...");

        auto generatedFolder = std::filesystem::current_path() / "reframework" / "plugins" / "managed" / "generated";
        std::filesystem::create_directories(generatedFolder);

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
        
        const auto managed_path = std::filesystem::current_path() / "reframework" / "plugins" / "managed";
        std::filesystem::create_directories(managed_path);

        System::String^ managed_dir = gcnew System::String(managed_path.wstring().c_str());
        auto files = System::IO::Directory::GetFiles(managed_dir, "*.dll");

        if (files->Length != 0) {
            bool ever_found = false;

            for each (System::String^ file in files) {
                Console::WriteLine(file);
                System::Reflection::Assembly^ assem = System::Reflection::Assembly::LoadFrom(file);

                if (assem == nullptr) {
                    REFrameworkNET::API::LogError("Failed to load assembly from " + file);
                    continue;
                }

                // Iterate through all types in the assembly
                for each (Type^ type in assem->GetTypes()) {
                    array<System::Reflection::MethodInfo^>^ methods = type->GetMethods(System::Reflection::BindingFlags::Static | System::Reflection::BindingFlags::Public);

                    for each (System::Reflection::MethodInfo^ method in methods) {
                        array<Object^>^ attributes = method->GetCustomAttributes(REFrameworkNET::Attributes::PluginEntryPoint::typeid, true);

                        if (attributes->Length > 0) {
                            REFrameworkNET::API::LogInfo("Found PluginEntryPoint in " + method->Name + " in " + type->FullName);
                            method->Invoke(nullptr, nullptr);
                            ever_found = true;
                        }
                    }
                }
            }

            if (!ever_found) {
                REFrameworkNET::API::LogInfo("No Main method found in any DLLs in " + managed_dir);
            }
        } else {
            REFrameworkNET::API::LogInfo("No DLLs found in " + managed_dir);
        }

        // Unload dynamic assemblies (testing)
        //UnloadDynamicAssemblies();

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

        s_wants_reload = true;
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
        if (s_wants_reload) {
            s_wants_reload = false;

            PluginManager::UnloadDynamicAssemblies();
            PluginManager::LoadPlugins_FromSourceCode(0, s_dependencies);
        }

        SetupFileWatcher();
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
        s_default_context = gcnew PluginLoadContext();

        for each (System::String^ file in files) {
            System::Console::WriteLine(file);

            // Compile the C# file, and then call a function in it (REFrameworkPlugin.Main)
            // This is useful for loading C# plugins that don't want to be compiled into a DLL
            auto bytecode = REFrameworkNET::Compiler::Compile(file, self, deps);

            if (bytecode == nullptr) {
                REFrameworkNET::API::LogError("Failed to compile " + file);
                continue;
            }

            auto assem = s_default_context->LoadFromStream(gcnew System::IO::MemoryStream(bytecode));
            //auto assem = System::Reflection::Assembly::Load(bytecode);

            if (assem == nullptr) {
                REFrameworkNET::API::LogError("Failed to load assembly from " + file);
                continue;
            }

            s_dynamic_assemblies->Add(assem);

            REFrameworkNET::API::LogInfo("Compiled " + file);

            // Look for the Main method in the compiled assembly
            for each (Type^ type in assem->GetTypes()) {
                array<System::Reflection::MethodInfo^>^ methods = type->GetMethods(System::Reflection::BindingFlags::Static | System::Reflection::BindingFlags::Public | System::Reflection::BindingFlags::NonPublic);

                for each (System::Reflection::MethodInfo^ method in methods) {
                    array<Object^>^ attributes = method->GetCustomAttributes(REFrameworkNET::Attributes::PluginEntryPoint::typeid, true);

                    if (attributes->Length > 0) {
                        REFrameworkNET::API::LogInfo("Found PluginEntryPoint in " + method->Name + " in " + type->FullName);
                        method->Invoke(nullptr, nullptr);
                        ever_found = true;
                    }
                }
            }
        }

        if (!ever_found) {
            Console::WriteLine("No C# files compiled in " + cs_files_dir);
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

    void PluginManager::UnloadDynamicAssemblies() {
        if (PluginManager::s_dynamic_assemblies == nullptr || PluginManager::s_dynamic_assemblies->Count == 0) {
            REFrameworkNET::API::LogInfo("No dynamic assemblies to unload");
			return;
		}

        REFrameworkNET::API::LogInfo("Unloading dynamic assemblies...");

        for each (System::Reflection::Assembly ^ assem in PluginManager::s_dynamic_assemblies) {
            if (assem == nullptr) {
				continue;
			}

            try {
				// Look for the Unload method in the target assembly which takes an REFrameworkNET.API instance
                for each (Type ^ t in assem->GetTypes()) {
					auto method = t->GetMethod("OnUnload", System::Reflection::BindingFlags::Static | System::Reflection::BindingFlags::Public);

                    if (method != nullptr) {
                        REFrameworkNET::API::LogInfo("Unloading dynamic assembly by calling " + method->Name + " in " + t->FullName);
						method->Invoke(nullptr, nullptr);
					}

                    Callbacks::Impl::UnsubscribeAssembly(assem);
				}
            }
            catch (System::Exception^ e) {
				REFrameworkNET::API::LogError("Failed to unload dynamic assembly: " + e->Message);
            }
            catch (const std::exception& e) {
				REFrameworkNET::API::LogError("Failed to unload dynamic assembly: " + gcnew System::String(e.what()));
            }
            catch (...) {
				REFrameworkNET::API::LogError("Unknown exception caught while unloading dynamic assembly");
			}
		}

        s_dynamic_assemblies->Clear();

        // make weak ref to default context
        if (s_dynamic_assemblies != nullptr) {
            System::WeakReference^ weakRef = gcnew System::WeakReference(s_default_context);
            PluginManager::s_default_context->Unload();
            PluginManager::s_default_context = nullptr;

            bool unloaded = false;

            for (int i = 0; i < 10; i++) {
                if (weakRef->IsAlive) {
                    System::GC::Collect();
                    System::GC::WaitForPendingFinalizers();
                    System::Threading::Thread::Sleep(10);
                } else {
                    unloaded = true;
                    System::Console::WriteLine("Successfully unloaded default context");
                    break;
                }
            }

            if (!unloaded) {
                System::Console::WriteLine("Failed to unload default context");
            }
        }
    }

    void PluginManager::ImGuiCallback(::REFImGuiFrameCbData* data) {
        //System::Console::WriteLine("ImGuiCallback called");

        // marshal to intptr
        /*auto context = System::IntPtr(data->context);
        auto mallocFn = System::IntPtr(data->malloc_fn);
        auto freeFn = System::IntPtr(data->free_fn);
        auto user_data = System::IntPtr(data->user_data);

        ImGuiNET::ImGui::SetCurrentContext(context);
        ImGuiNET::ImGui::SetAllocatorFunctions(mallocFn, freeFn, user_data);*/

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
                PluginManager::UnloadDynamicAssemblies();
                PluginManager::LoadPlugins_FromSourceCode(0, s_dependencies);
            }

            ImGuiNET::ImGui::SameLine();

            if (ImGuiNET::ImGui::Button("Unload Scripts")) {
                PluginManager::UnloadDynamicAssemblies();
            }
        }

        ImGuiNET::ImGui::PopID();
    }
}