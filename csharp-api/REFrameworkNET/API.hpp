#pragma once

#include <reframework/API.h>

#pragma managed

#include <msclr/marshal_cppstd.h>

#include "TypeInfo.hpp"
#include "ManagedObject.hpp"
#include "TDB.hpp"
#include "ManagedSingleton.hpp"
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

    static REFrameworkNET::TDB^ GetTDB() {
        if (s_api == nullptr) {
            throw gcnew APINotInitializedException();
        }

        return gcnew REFrameworkNET::TDB(s_api->tdb());
    }

    static REFrameworkNET::ManagedObject^ GetManagedSingleton(System::String^ name) {
        if (s_api == nullptr) {
            throw gcnew APINotInitializedException();
        }

        auto result = s_api->get_managed_singleton(msclr::interop::marshal_as<std::string>(name));

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew REFrameworkNET::ManagedObject(result);
    }

    static System::Collections::Generic::List<REFrameworkNET::ManagedSingleton^>^ GetManagedSingletons() {
        if (s_api == nullptr) {
            throw gcnew APINotInitializedException();
        }

        auto singletons = s_api->get_managed_singletons();
        
        auto result = gcnew System::Collections::Generic::List<REFrameworkNET::ManagedSingleton^>();

        for (auto& singleton : singletons) {
            if (singleton.instance == nullptr) {
                continue;
            }

            result->Add(gcnew REFrameworkNET::ManagedSingleton(
                gcnew REFrameworkNET::ManagedObject(singleton.instance),
                gcnew REFrameworkNET::TypeDefinition(singleton.t),
                gcnew REFrameworkNET::TypeInfo(singleton.type_info)
            ));
        }

        return result;
    }

    static NativeObject^ GetNativeSingleton(System::String^ name) {
        if (s_api == nullptr) {
            throw gcnew APINotInitializedException();
        }

        auto result = s_api->get_native_singleton(msclr::interop::marshal_as<std::string>(name));

        if (result == nullptr) {
            return nullptr;
        }

        auto t = REFrameworkNET::API::GetTDB()->GetType(name);

        if (t == nullptr) {
            return nullptr;
        }

        return gcnew NativeObject(result, t);
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

    static reframework::API* GetNativeImplementation() {
        if (s_api == nullptr) {
            throw gcnew APINotInitializedException();
        }

        return s_api;
    }

    static LogLevel LogLevel{LogLevel::Info};
    static bool LogToConsole{true};

    static void LogError(System::String^ message) {
        if (s_api == nullptr) {
            throw gcnew APINotInitializedException();
        }

        if (LogLevel <= LogLevel::Error) {
            s_api->log_error(msclr::interop::marshal_as<std::string>(message).c_str());

            if (LogToConsole) {
                System::Console::WriteLine(message);
            }
        }
    }

    static void LogWarning(System::String^ message) {
        if (s_api == nullptr) {
            throw gcnew APINotInitializedException();
        }

        if (LogLevel <= LogLevel::Warning) {
            s_api->log_warn(msclr::interop::marshal_as<std::string>(message).c_str());

            if (LogToConsole) {
                System::Console::WriteLine(message);
            }
        }
    }

    static void LogInfo(System::String^ message) {
        if (s_api == nullptr) {
            throw gcnew APINotInitializedException();
        }

        if (LogLevel <= LogLevel::Info) {
            s_api->log_info(msclr::interop::marshal_as<std::string>(message).c_str());

            if (LogToConsole) {
                System::Console::WriteLine(message);
            }
        }
    }

protected:
    void Init_Internal(const REFrameworkPluginInitializeParam* param);

    static reframework::API* s_api;
};
}