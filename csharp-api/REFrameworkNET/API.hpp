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
public ref class API
{
public:
    API(const REFrameworkPluginInitializeParam* param);
    API(uintptr_t param);
    ~API();

    static REFrameworkNET::TDB^ GetTDB() {
        if (s_api == nullptr) {
            throw gcnew System::InvalidOperationException("API is not initialized.");
        }

        return gcnew REFrameworkNET::TDB(s_api->tdb());
    }

    static REFrameworkNET::ManagedObject^ GetManagedSingleton(System::String^ name) {
        if (s_api == nullptr) {
            throw gcnew System::InvalidOperationException("API is not initialized.");
        }

        auto result = s_api->get_managed_singleton(msclr::interop::marshal_as<std::string>(name));

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew REFrameworkNET::ManagedObject(result);
    }

    static System::Collections::Generic::List<REFrameworkNET::ManagedSingleton^>^ GetManagedSingletons() {
        if (s_api == nullptr) {
            throw gcnew System::InvalidOperationException("API is not initialized.");
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
            throw gcnew System::InvalidOperationException("API is not initialized.");
        }

        auto result = s_api->get_native_singleton(msclr::interop::marshal_as<std::string>(name));

        if (result == nullptr) {
            return nullptr;
        }

        auto t = GetTDB()->GetType(name);

        if (t == nullptr) {
            return nullptr;
        }

        return gcnew NativeObject(result, t);
    }

    static reframework::API* GetNativeImplementation() {
        if (s_api == nullptr) {
            throw gcnew System::InvalidOperationException("API is not initialized.");
        }

        return s_api;
    }

protected:
    void Init_Internal(const REFrameworkPluginInitializeParam* param);

    static reframework::API* s_api;
};
}