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

        if (auto pre = type->GetField("PreImplementation", wantedFlags); pre != nullptr) {
            s_knownStaticEvents->Add(pre);
        }

        if (auto post = type->GetField("PostImplementation", wantedFlags); post != nullptr) {
            s_knownStaticEvents->Add(post);
        }

        if (type->Name == "ImGuiRender") {
            Console::WriteLine("Skipping ImGuiRender");
            continue;
        }

        IntPtr preHookPtr = Marshal::GetFunctionPointerForDelegate(triggerPre);
        IntPtr postHookPtr = Marshal::GetFunctionPointerForDelegate(triggerPost);
        nativeApi->param()->functions->on_pre_application_entry(eventNameStr.c_str(), (REFOnPreApplicationEntryCb)preHookPtr.ToPointer());
        nativeApi->param()->functions->on_post_application_entry(eventNameStr.c_str(), (REFOnPostApplicationEntryCb)postHookPtr.ToPointer());
    }
}

void Impl::UnsubscribeAssembly(System::Reflection::Assembly^ assembly) {
    s_unloading = true;

    if (s_knownStaticEvents->Count == 0) {
        REFrameworkNET::API::LogError("REFrameworkNET.Callbacks UnsubscribeAssembly: No known static events to unsubscribe from.");
        s_unloading = false;
        return;
    }

    for each (System::Reflection::FieldInfo ^ ei in s_knownStaticEvents) {
        while (true) {
            auto pre = ei->GetValue(nullptr);

            if (pre == nullptr) {
                break;
            }

            auto del = (System::Delegate^)pre;
            auto invocationList = del->GetInvocationList();

            bool set = false;

            for each (System::Delegate ^ d in invocationList) {
                // Get the assembly that the delegate is from
                auto target = d->Method;
                auto targetAssembly = target->DeclaringType->Assembly;

                if (targetAssembly == assembly) {
                    System::Console::WriteLine("REFrameworkNET.Callbacks UnsubscribeAssembly: Removing " + target->Name + " from " + ei->Name);
                    auto newDel = (BaseCallback::Delegate^)del - (BaseCallback::Delegate^)d;
                    ei->SetValue(nullptr, newDel);
                    set = true;
                    break;
                }
            }

            if (!set) {
                break;
            }
        }

        while (true) {
            auto post = ei->GetValue(nullptr);

            if (post == nullptr) {
                break;
            }

            auto del = (System::Delegate^)post;
            auto invocationList = del->GetInvocationList();

            bool set = false;

            for each (System::Delegate ^ d in invocationList) {
                // Get the assembly that the delegate is from
                auto target = d->Method;
                auto targetAssembly = target->DeclaringType->Assembly;

                if (targetAssembly == assembly) {
                    System::Console::WriteLine("REFrameworkNET.Callbacks UnsubscribeAssembly: Removing " + target->Name + " from " + ei->Name);
                    auto newDel = (BaseCallback::Delegate^)del - (BaseCallback::Delegate^)d;
                    ei->SetValue(nullptr, newDel);
                    set = true;
                    break;
                }
            }

            if (!set) {
                break;
            }
        }
    }

    s_unloading = false;
}
}
}