#pragma once

#include "API.hpp"

namespace REFrameworkNET {
public ref class PluginLoadContext : public System::Runtime::Loader::AssemblyLoadContext {
public:
    PluginLoadContext()
        : System::Runtime::Loader::AssemblyLoadContext("REFrameworkNET.PluginLoadContext", true)
    {

    }

    System::Reflection::Assembly^ Load(System::Reflection::AssemblyName^ assemblyName) override
    {
        // Load the Microsoft.CSharp assembly into our plugin's AssemblyLoadContext
        // If we don't do this, we will fail to unload the assembly when any dynamic operation is performed (like creating a dynamic object)
        // For some reason this DLL keeps a reference to the assembly that created a dynamic when creating these objects
        // Relevant github issue: https://github.com/dotnet/runtime/issues/71629
        if (assemblyName->Name == "Microsoft.CSharp")
        {
            System::Console::WriteLine("Searching in " + System::Runtime::InteropServices::RuntimeEnvironment::GetRuntimeDirectory()  + " for Microsoft.CSharp.dll");
            auto dir = System::Runtime::InteropServices::RuntimeEnvironment::GetRuntimeDirectory();
            if (auto assem = LoadFromAssemblyPath(System::IO::Path::Combine(dir, "Microsoft.CSharp.dll")); assem != nullptr) {
                return assem;
            }

            REFrameworkNET::API::LogError("Failed to load Microsoft.CSharp.dll, dynamic operations may cause unloading issues");
        }

        REFrameworkNET::API::LogInfo(this->Name + " is requesting " + assemblyName->Name + ", redirecting to default context");

        return nullptr;
    }

private:
};
}