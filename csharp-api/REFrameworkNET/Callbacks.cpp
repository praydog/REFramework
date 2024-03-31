#include "API.hpp"

#include "Callbacks.hpp"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;

namespace REFrameworkNET {
namespace Callbacks {
void Impl::Setup(REFrameworkNET::API^ api) {
    if (s_setup) {
        Console::WriteLine("REFrameworkNET.Callbacks SetupCallbacks called but already setup.");
        return;
    }

    s_setup = true;

    Console::WriteLine("REFrameworkNET.Callbacks SetupCallbacks called.");

    reframework::API* nativeApi = api->GetNativeImplementation();
    System::Reflection::Assembly^ assembly = System::Reflection::Assembly::GetExecutingAssembly();
    array<Type^>^ types = assembly->GetTypes();

    // Look for REFrameworkNET.Callbacks.* classes
    for each (Type^ type in types) {
        if (type->Namespace != "REFrameworkNET.Callbacks") {
            continue;
        }

        if (!type->IsClass) {
            continue;
        }

        const auto wantedFlags = System::Reflection::BindingFlags::NonPublic | System::Reflection::BindingFlags::Static;

        if (type->GetField("TriggerPostDelegate", wantedFlags) == nullptr || type->GetField("TriggerPreDelegate", wantedFlags) == nullptr) {
            continue;
        }

        auto eventName = type->Name;
        auto eventNameStr = msclr::interop::marshal_as<std::string>(eventName);
        auto triggerPost = type->GetField("TriggerPostDelegate", wantedFlags)->GetValue(nullptr);
        auto triggerPre = type->GetField("TriggerPreDelegate", wantedFlags)->GetValue(nullptr);

        if (triggerPre == nullptr || triggerPost == nullptr) {
            System::Console::WriteLine("REFrameworkNET.Callbacks SetupCallbacks: TriggerPreDelegate or TriggerPostDelegate is null for " + eventName);
            continue;
        }

        IntPtr preHookPtr = Marshal::GetFunctionPointerForDelegate(triggerPre);
        IntPtr postHookPtr = Marshal::GetFunctionPointerForDelegate(triggerPost);
        nativeApi->param()->functions->on_pre_application_entry(eventNameStr.c_str(), (REFOnPreApplicationEntryCb)preHookPtr.ToPointer());
        nativeApi->param()->functions->on_post_application_entry(eventNameStr.c_str(), (REFOnPostApplicationEntryCb)postHookPtr.ToPointer());
    }
}
}
}