#include <exception>
#include <filesystem>

#include "PluginManager.hpp"

namespace REFrameworkNET {
    // Executed initially when we get loaded via LoadLibrary
    // which is not in the correct context to actually load the managed plugins
    bool PluginManager::LoadPlugins_FromDefaultContext(const REFrameworkPluginInitializeParam* param) try {
        if (s_initialized) {
            return true;
        }

        PluginManager::s_initialized = true;

        // Write to console using C++/CLI
        System::String^ str = "Hello from C++/CLI!";
        System::Console::WriteLine(str);

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
        method->Invoke(nullptr, gcnew array<Object^>{reinterpret_cast<uintptr_t>(param)});

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
        return false;
    } catch (const std::exception& e) {
        System::Console::WriteLine(gcnew System::String(e.what()));
        return false;
    } catch (...) {
        System::Console::WriteLine("Unknown exception caught");
        return false;
    }

    // meant to be executed in the correct context
    // after loading "ourselves" via System::Reflection::Assembly::LoadFrom
    bool PluginManager::LoadPlugins(uintptr_t param_raw) {
        if (PluginManager::s_initialized) {
            return true;
        }

        PluginManager::s_initialized = true;

        auto self = System::Reflection::Assembly::GetExecutingAssembly();

        // Look for any DLLs in the "managed" directory, load them, then call a function in them (REFrameworkPlugin.Main)
        // This is useful for loading C# plugins
        // Create the REFramework::API class first though (managed)
        PluginManager::s_api_instance = gcnew REFrameworkNET::API(param_raw);
        
        const auto managed_path = std::filesystem::current_path() / "reframework" / "plugins" / "managed";
        std::filesystem::create_directories(managed_path);

        String^ managed_dir = gcnew String(managed_path.wstring().c_str());

        bool ever_found = false;
        auto files = System::IO::Directory::GetFiles(managed_dir, "*.dll");

        if (files->Length == 0) {
            //API::get()->log_error("No DLLs found in %s", managedDir);
            return false;
        }

        for each (String^ file in files) {
            Console::WriteLine(file);
            System::Reflection::Assembly^ assem = System::Reflection::Assembly::LoadFrom(file);

            if (assem == nullptr) {
                Console::WriteLine("Failed to load assembly from " + file);
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
                    Console::WriteLine("Found Main method in " + file);

                    array<Object^>^ args = gcnew array<Object^>{PluginManager::s_api_instance};
                    mainMethod->Invoke(nullptr, args);
                    ever_found = true;
                }
            }
        }

        if (!ever_found) {
            Console::WriteLine("No Main method found in any DLLs in " + managed_dir);
        }

        return true;
    }
}