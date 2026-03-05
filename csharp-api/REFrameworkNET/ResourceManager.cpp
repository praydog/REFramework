#pragma managed

#include <reframework/API.hpp>

#include "ResourceManager.hpp"
#include "Resource.hpp"
#include "ManagedObject.hpp"
#include "API.hpp"

REFrameworkNET::Resource^ REFrameworkNET::ResourceManager::CreateResource(System::String^ typeName, System::String^ name) {
    auto nativeTypeName = msclr::interop::marshal_as<std::string>(typeName);
    auto nativeName = msclr::interop::marshal_as<std::string>(name);

    static const auto fn = REFrameworkNET::API::GetNativeImplementation()->sdk()->resource_manager->create_resource;
    auto result = fn(m_handle, nativeTypeName.c_str(), nativeName.c_str());

    if (result == nullptr) {
        return nullptr;
    }

    return gcnew REFrameworkNET::Resource(result);
}

REFrameworkNET::ManagedObject^ REFrameworkNET::ResourceManager::CreateUserData(System::String^ typeName, System::String^ name) {
    auto nativeTypeName = msclr::interop::marshal_as<std::string>(typeName);
    auto nativeName = msclr::interop::marshal_as<std::string>(name);

    static const auto fn = REFrameworkNET::API::GetNativeImplementation()->sdk()->resource_manager->create_userdata;
    auto result = fn(m_handle, nativeTypeName.c_str(), nativeName.c_str());

    if (result == nullptr) {
        return nullptr;
    }

    return REFrameworkNET::ManagedObject::Get((reframework::API::ManagedObject*)result);
}
