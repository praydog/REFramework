#pragma once

#include <reframework/API.h>

#pragma managed

#include <msclr/marshal_cppstd.h>

#include "TypeInfo.hpp"
#include "ManagedObject.hpp"
#include "TDB.hpp"
#include "ManagedSingleton.hpp"
#include "NativeSingleton.hpp"
#include "NativeObject.hpp"

#include "Callbacks.hpp"

namespace reframework {
class API;
}

using namespace System;

namespace REFrameworkNET {
public enum LogLevel {
    Info = 0,
    Warning = 1,
    Error = 2,
};

public ref class API
{
public:
    API(const REFrameworkPluginInitializeParam* param);
    API(uintptr_t param);
    ~API();

    ref class APINotInitializedException : public System::InvalidOperationException {
    public:
        APINotInitializedException() : System::InvalidOperationException("API is not initialized.") {}
    };

    static reframework::API* GetNativeImplementation();
    static REFrameworkNET::TDB^ GetTDB();

    static System::Collections::Generic::List<REFrameworkNET::ManagedSingleton^>^ GetManagedSingletons();
    static System::Collections::Generic::List<REFrameworkNET::NativeSingleton^>^ GetNativeSingletons();
    static REFrameworkNET::ManagedObject^ GetManagedSingleton(System::String^ name);
    static NativeObject^ GetNativeSingleton(System::String^ name);

    static void LocalFrameGC() {
        s_api->get_vm_context()->local_frame_gc();
    }

    generic <typename T> where T : ref class
    static T GetNativeSingletonT() {
        auto fullName = T::typeid->FullName;

        auto no = GetNativeSingleton(fullName);
        if (no == nullptr) {
            return T(); // nullptr basically
        }

        return no->As<T>();
    }

    generic <typename T> where T : ref class
    static T GetManagedSingletonT() {
        auto fullName = T::typeid->FullName;
        auto mo = GetManagedSingleton(fullName);
        if (mo == nullptr) {
            return T(); // nullptr basically
        }
        
        return mo->As<T>();
    }

    static LogLevel LogLevel{LogLevel::Info};
    static bool LogToConsole{true};

    static void LogError(System::String^ message);
    static void LogWarning(System::String^ message);

    static void LogInfo(System::String^ message);

protected:
    void Init_Internal(const REFrameworkPluginInitializeParam* param);

    static reframework::API* s_api;
};
}