extern "C" {
    #include <reframework/API.h>
}

#include <sstream>
#include <mutex>
#include <filesystem>

#include <Windows.h>
#include <msclr/marshal_cppstd.h>

#include "REFrameworkAPI.hpp"

#include "Plugin.hpp"

using namespace reframework;
using namespace System;
using namespace System::Runtime;

void on_pre_begin_rendering() {

}

void on_post_end_rendering() {

}


namespace REFManagedInternal {
public ref class API {
public:
    static REFramework::API^ instance;
};
}

ref class CSharpAPIImpl {
public:
    static bool ManagedImpl(uintptr_t param_raw) {
        auto self = System::Reflection::Assembly::GetExecutingAssembly();

        // Look for any DLLs in the "managed" directory, load them, then call a function in them (REFrameworkPlugin.Main)
        // This is useful for loading C# plugins
        // Create the REFramework::API class first though (managed)
        REFManagedInternal::API::instance = gcnew REFramework::API(param_raw);
        
        std::filesystem::create_directories(std::filesystem::current_path() / "reframework" / "plugins" / "managed");

        String^ managedDir = gcnew String((std::filesystem::current_path() / "reframework" / "plugins" / "managed").wstring().c_str());

        bool ever_found = false;
        auto files = System::IO::Directory::GetFiles(managedDir, "*.dll");

        if (files->Length == 0) {
            //API::get()->log_error("No DLLs found in %s", managedDir);
            return false;
        }

        for each (String^ file in files) {
            Console::WriteLine(file);
            System::Reflection::Assembly^ assem = System::Reflection::Assembly::LoadFrom(file);

            if (assem == nullptr) {
                //API::get()->log_error("Failed to load assembly from %s", file);
                Console::WriteLine("Failed to load assembly from " + file);
                continue;
            }

            // Iterate through all types in the assembly
            for each (Type^ type in assem->GetTypes()) {
                // Attempt to find the Main method with the expected signature in each type
                System::Reflection::MethodInfo^ mainMethod = type->GetMethod(
                                                                "Main", 
                                                                System::Reflection::BindingFlags::Static | System::Reflection::BindingFlags::Public,
                                                                gcnew array<Type^>{REFramework::API::typeid});

                if (mainMethod != nullptr) {
                    Console::WriteLine("Found Main method in " + file);
                    //API::get()->log_info("Found Main method in %s", file);
                    // If found, invoke the method

                    array<Object^>^ args = gcnew array<Object^>{REFManagedInternal::API::instance};
                    mainMethod->Invoke(nullptr, args);
                    ever_found = true;
                    break; // Optional: break if you only expect one Main method per assembly
                }
            }
        }

        if (!ever_found) {
            //API::get()->log_error("No Main method found in any DLLs in %s", managedDir);
            Console::WriteLine("No Main method found in any DLLs in " + managedDir);
        }

        return true;
    }

private:
};

bool managed_impl(const REFrameworkPluginInitializeParam* param) try {
    // Write to console using C++/CLI
    System::String^ str = "Hello from C++/CLI!";
    System::Console::WriteLine(str);

    const auto functions = param->functions;
    functions->on_pre_application_entry("BeginRendering", on_pre_begin_rendering); // Look at via.ModuleEntry or the wiki for valid names here
    functions->on_post_application_entry("EndRendering", on_post_end_rendering);

    functions->log_error("%s %s", "Hello", "error");
    functions->log_warn("%s %s", "Hello", "warning");
    functions->log_info("%s %s", "Hello", "info");

    // Make sure plugins that are loaded can find a reference to the current assembly
    // Even though "this" is loaded currently, its not in the right context to be found
    // So we have to load ourselves again, and call CSharpAPIImpl.ManagedImpl via a dynamic lookup
    auto self = System::Reflection::Assembly::LoadFrom(System::Reflection::Assembly::GetExecutingAssembly()->Location);

    // Get CSharpAPIImpl type
    auto type = self->GetType("CSharpAPIImpl");

    // Get ManagedImpl method
    auto method = type->GetMethod("ManagedImpl", System::Reflection::BindingFlags::Static | System::Reflection::BindingFlags::Public);

    // Invoke ManagedImpl method
    method->Invoke(nullptr, gcnew array<Object^>{reinterpret_cast<uintptr_t>(param)});

    return true;
} catch (System::Reflection::ReflectionTypeLoadException^ ex) {
    auto loaderExceptions = ex->LoaderExceptions;
    for each (Exception^ innerEx in loaderExceptions) {
        //API::get()->log_error("Loader exception caught: %s", msclr::interop::marshal_as<std::string>(innerEx->Message).c_str());
        System::Console::WriteLine(innerEx->Message);
    }
    // Optionally log the main exception as well
    //API::get()->log_error("ReflectionTypeLoadException caught: %s", msclr::interop::marshal_as<std::string>(ex->Message).c_str());
    System::Console::WriteLine(ex->Message);
    return false;
} catch (System::Exception^ e) {
    //API::get()->log_error("Exception caught: %s", msclr::interop::marshal_as<std::string>(e->Message).c_str());
    System::Console::WriteLine(e->Message);
    return false;
} catch (const std::exception& e) {
    //API::get()->log_error("Exception caught: %s", e.what());
    System::Console::WriteLine(gcnew System::String(e.what()));
    return false;
} catch (...) {
    //API::get()->log_error("Unknown exception caught");
    System::Console::WriteLine("Unknown exception caught");
    return false;
}

extern "C" __declspec(dllexport) bool reframework_plugin_initialize(const REFrameworkPluginInitializeParam* param) {
    // Create a console
    AllocConsole();

    return managed_impl(param);
}

extern "C" __declspec(dllexport) void reframework_plugin_required_version(REFrameworkPluginVersion* version) {
    version->major = REFRAMEWORK_PLUGIN_VERSION_MAJOR;
    version->minor = REFRAMEWORK_PLUGIN_VERSION_MINOR;
    version->patch = REFRAMEWORK_PLUGIN_VERSION_PATCH;

    // Optionally, specify a specific game name that this plugin is compatible with.
    //version->game_name = "MHRISE";
}
