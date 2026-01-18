#include "API.hpp"
#include "SystemString.hpp"

#include "VM.hpp"

namespace REFrameworkNET {
SystemString^ VM::CreateString(::System::String^ str) {
    static auto fn = REFrameworkNET::API::GetNativeImplementation()->sdk()->functions->create_managed_string;

    pin_ptr<const wchar_t> p = PtrToStringChars(str);
    const auto chars = reinterpret_cast<const wchar_t*>(p);
    
    auto objHandle = fn(chars);

    if (objHandle == nullptr) {
        REFrameworkNET::API::LogWarning("Failed to create managed string");
        return nullptr;
    }

    return ManagedObject::Get<SystemString>(objHandle);
}

SystemString^ VM::CreateString(std::wstring_view str) {
    static auto fn = REFrameworkNET::API::GetNativeImplementation()->sdk()->functions->create_managed_string;
    auto objHandle = fn(str.data());

    if (objHandle == nullptr) {
        REFrameworkNET::API::LogWarning("Failed to create managed string");
        return nullptr;
    }

    return ManagedObject::Get<SystemString>(objHandle);
}

SystemString^ VM::CreateString(std::string_view str) {
    static auto fn = REFrameworkNET::API::GetNativeImplementation()->sdk()->functions->create_managed_string_normal;
    auto objHandle = fn(str.data());

    if (objHandle == nullptr) {
        REFrameworkNET::API::LogWarning("Failed to create managed string");
        return nullptr;
    }

    return ManagedObject::Get<SystemString>(objHandle);
}
}