#include <exception>
#include <filesystem>
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

    System::Collections::Generic::List<System::Reflection::Assembly^>^ PluginManager::LoadDependencies() {
        REFrameworkNET::API::LogInfo("Loading managed dependencies...");

        const auto dependencies_path = std::filesystem::current_path() / "reframework" / "plugins" / "managed" / "dependencies";

        std::filesystem::create_directories(dependencies_path);

        auto files = System::IO::Directory::GetFiles(gcnew System::String(dependencies_path.wstring().c_str()), "*.dll");

        auto dependencies_dir = gcnew System::String(dependencies_path.wstring().c_str());
        auto assemblies = gcnew System::Collections::Generic::List<System::Reflection::Assembly^>();

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

    void PluginManager::GenerateReferenceAssemblies(System::Collections::Generic::List<System::Reflection::Assembly^>^ deps) {
        REFrameworkNET::API::LogInfo("Generating reference assemblies...");

        // Look for AssemblyGenerator class in the loaded deps
        for each (System::Reflection::Assembly^ a in deps) {
            if (auto generator = a->GetType("REFrameworkNET.AssemblyGenerator"); generator != nullptr) {
                // Look for Main method in the AssemblyGenerator class
                auto mainMethod = generator->GetMethod(
                                            "Main", 
                                            System::Reflection::BindingFlags::Static | System::Reflection::BindingFlags::Public,
                                            gcnew array<Type^>{REFrameworkNET::API::typeid});

               if (mainMethod != nullptr) {
                    REFrameworkNET::API::LogInfo("Found AssemblyGenerator.Main in " + a->Location);

                    array<Object^>^ args = gcnew array<Object^>{PluginManager::s_api_instance};
                    auto result = (List<Compiler::DynamicAssemblyBytecode^>^)mainMethod->Invoke(nullptr, args);

                    // Append the generated assemblies to the list of deps
                    for each (Compiler::DynamicAssemblyBytecode^ bytes in result) {
                        REFrameworkNET::API::LogInfo("Adding generated assembly to deps...");

                        std::string assembly_name = msclr::interop::marshal_as<std::string>(bytes->AssemblyName);

                        auto path = std::filesystem::current_path() / "reframework" / "plugins" / "managed" / "dependencies" / (assembly_name + "_DYNAMIC.dll");
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

        auto deps = LoadDependencies(); // Pre-loads DLLs in the dependencies folder before loading the plugins

        try {
            GenerateReferenceAssemblies(deps);
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
            LoadPlugins_FromSourceCode(param_raw, deps);
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

        bool ever_found = false;
        auto files = System::IO::Directory::GetFiles(managed_dir, "*.dll");

        if (files->Length == 0) {
            REFrameworkNET::API::LogInfo("No DLLs found in " + managed_dir);
            return false;
        }

        for each (System::String^ file in files) {
            Console::WriteLine(file);
            System::Reflection::Assembly^ assem = System::Reflection::Assembly::LoadFrom(file);

            if (assem == nullptr) {
                REFrameworkNET::API::LogError("Failed to load assembly from " + file);
                continue;
            }

            // Iterate through all types in the assembly
            for each (Type^ type in assem->GetTypes()) {
                // Attempt to find the Main method with the expected signature in each type
                System::Reflection::MethodInfo^ mainMethod = type->GetMethod(
                                                                "Main", 
                                                                System::Reflection::BindingFlags::Static | System::Reflection::BindingFlags::Public,
                                                                gcnew array<Type^>{REFrameworkNET::API::typeid});

                if (mainMethod != nullptr) {
                    REFrameworkNET::API::LogInfo("Found Main method in " + file);

                    array<Object^>^ args = gcnew array<Object^>{PluginManager::s_api_instance};
                    mainMethod->Invoke(nullptr, args);
                    ever_found = true;
                }
            }
        }

        if (!ever_found) {
            REFrameworkNET::API::LogInfo("No Main method found in any DLLs in " + managed_dir);
        }

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

    bool PluginManager::LoadPlugins_FromSourceCode(uintptr_t param_raw, System::Collections::Generic::List<System::Reflection::Assembly^>^ deps) try {
        if (PluginManager::s_api_instance == nullptr) {
            PluginManager::s_api_instance = gcnew REFrameworkNET::API(param_raw);
        }

        System::Console::WriteLine("Test");
        REFrameworkNET::API::LogInfo("Attempting to load plugins from source code...");

        const auto plugins_path = std::filesystem::current_path() / "reframework" / "plugins";
        const auto cs_files_path = plugins_path / "source";
        std::filesystem::create_directories(cs_files_path);

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
            // Dynamically look for DynamicRun.Builder.Compiler.Compile
            /*auto type = intermediary->GetType("DynamicRun.Builder.Compiler");
            if (type == nullptr) {
                REFrameworkNET::API::LogError("Failed to get type DynamicRun.Builder.Compiler");
                continue;
            }
            auto method = type->GetMethod("Compile", System::Reflection::BindingFlags::Static | System::Reflection::BindingFlags::Public);

            if (method == nullptr) {
                REFrameworkNET::API::LogError("Failed to get method DynamicRun.Builder.Compiler.Compile");
                continue;
            }

            auto bytecode = (array<System::Byte>^)method->Invoke(nullptr, gcnew array<Object^>{file, self->Location});*/

            if (bytecode == nullptr) {
                REFrameworkNET::API::LogError("Failed to compile " + file);
                continue;
            }

            auto assem = System::Reflection::Assembly::Load(bytecode);

            if (assem == nullptr) {
                REFrameworkNET::API::LogError("Failed to load assembly from " + file);
                continue;
            }

            REFrameworkNET::API::LogInfo("Compiled " + file);

            // Look for the Main method in the compiled assembly
            for each (Type^ type in assem->GetTypes()) {
                System::Reflection::MethodInfo^ mainMethod = type->GetMethod(
                                                                "Main", 
                                                                System::Reflection::BindingFlags::Static | System::Reflection::BindingFlags::Public,
                                                                gcnew array<Type^>{REFrameworkNET::API::typeid});

                if (mainMethod != nullptr) {
                    Console::WriteLine("Found Main method in " + file);

                    array<Object^>^ args = gcnew array<Object^>{PluginManager::s_api_instance};
                    mainMethod->Invoke(nullptr, args);
                    ever_found = true;
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
}