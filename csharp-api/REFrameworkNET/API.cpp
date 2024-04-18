#pragma managed

#include <reframework/API.hpp>

#include "./API.hpp"


REFrameworkNET::API::API(const REFrameworkPluginInitializeParam* param)
{
    Init_Internal(param);
}

REFrameworkNET::API::API(uintptr_t param)
{
    Init_Internal(reinterpret_cast<const REFrameworkPluginInitializeParam*>(param));
}

void REFrameworkNET::API::Init_Internal(const REFrameworkPluginInitializeParam* param)
{
    if (s_api != nullptr) {
        Console::WriteLine("REFrameworkNET.API Init_Internal called but API is already initialized.");
        return;
    }

    Console::WriteLine("REFrameworkNET.API Init_Internal called.");
    s_api = reframework::API::initialize(param).get();
    Callbacks::Impl::Setup(this);
    Console::WriteLine("REFrameworkNET.API Init_Internal finished.");
}

REFrameworkNET::API::~API()
{
    Console::WriteLine("REFrameworkNET.API Destructor called.");
}


reframework::API* REFrameworkNET::API::GetNativeImplementation() {
    if (s_api == nullptr) {
        throw gcnew APINotInitializedException();
    }

    return s_api;
}

REFrameworkNET::TDB^ REFrameworkNET::API::GetTDB() {
    if (s_api == nullptr) {
        throw gcnew APINotInitializedException();
    }

    return gcnew REFrameworkNET::TDB(s_api->tdb());
}

System::Collections::Generic::List<REFrameworkNET::ManagedSingleton^>^ REFrameworkNET::API::GetManagedSingletons() {
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
            REFrameworkNET::ManagedObject::Get<REFrameworkNET::ManagedObject>(singleton.instance),
            TypeDefinition::GetInstance(singleton.t),
            gcnew REFrameworkNET::TypeInfo(singleton.type_info)
        ));
    }

    return result;
}

System::Collections::Generic::List<REFrameworkNET::NativeSingleton^>^ REFrameworkNET::API::GetNativeSingletons() {
    if (s_api == nullptr) {
        throw gcnew APINotInitializedException();
    }

    auto singletons = s_api->get_native_singletons();
    auto result = gcnew System::Collections::Generic::List<REFrameworkNET::NativeSingleton^>();

    for (auto& singleton : singletons) {
        if (singleton.instance == nullptr) {
            continue;
        }

        // Not supported for now
        if (singleton.t == nullptr) {
            continue;
        }

        auto nativeObject = gcnew REFrameworkNET::NativeObject(singleton.instance, TypeDefinition::GetInstance(singleton.t));

        result->Add(gcnew REFrameworkNET::NativeSingleton(
            gcnew REFrameworkNET::NativeObject(singleton.instance, TypeDefinition::GetInstance(singleton.t)),
            singleton.type_info != nullptr ? gcnew REFrameworkNET::TypeInfo(singleton.type_info) : nullptr
        ));
    }

    return result;
}

REFrameworkNET::ManagedObject^ REFrameworkNET::API::GetManagedSingleton(System::String^ name) {
    if (s_api == nullptr) {
        throw gcnew APINotInitializedException();
    }

    auto result = s_api->get_managed_singleton(msclr::interop::marshal_as<std::string>(name));

    if (result == nullptr) {
        return nullptr;
    }

    return REFrameworkNET::ManagedObject::Get(result);
}


REFrameworkNET::NativeObject^ REFrameworkNET::API::GetNativeSingleton(System::String^ name) {
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

void REFrameworkNET::API::LogError(System::String^ message) {
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

void REFrameworkNET::API::LogWarning(System::String^ message) {
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

void REFrameworkNET::API::LogInfo(System::String^ message) {
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