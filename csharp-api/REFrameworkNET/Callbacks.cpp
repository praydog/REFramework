#include "API.hpp"

#include "Callbacks.hpp"

using namespace System;

namespace REFrameworkNET {
namespace Callbacks {
void Impl::Setup(REFrameworkNET::API^ api) {
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

        if (type->GetField("FUNCTION_PRE_CALLBACK_ADDRESS") == nullptr) {
            continue;
        }

        if (type->GetField("FUNCTION_POST_CALLBACK_ADDRESS") == nullptr) {
            continue;
        }

        auto eventName = type->Name;
        auto eventNameStr = msclr::interop::marshal_as<std::string>(eventName);
        auto eventHandlerPre = (uintptr_t)type->GetField("FUNCTION_PRE_CALLBACK_ADDRESS")->GetValue(nullptr);
        auto eventHandlerPost = (uintptr_t)type->GetField("FUNCTION_POST_CALLBACK_ADDRESS")->GetValue(nullptr);
        nativeApi->param()->functions->on_pre_application_entry(eventNameStr.c_str(), (REFOnPreApplicationEntryCb)eventHandlerPre);
        nativeApi->param()->functions->on_post_application_entry(eventNameStr.c_str(), (REFOnPostApplicationEntryCb)eventHandlerPost);
    }
}
}
}