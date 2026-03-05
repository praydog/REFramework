#pragma managed

#include <reframework/API.hpp>

#include "Resource.hpp"
#include "ManagedObject.hpp"
#include "API.hpp"

void REFrameworkNET::Resource::AddRef() {
    if (m_handle == nullptr) return;
    static const auto fn = REFrameworkNET::API::GetNativeImplementation()->sdk()->resource->add_ref;
    fn(m_handle);
}

void REFrameworkNET::Resource::Release() {
    if (m_handle == nullptr) return;
    static const auto fn = REFrameworkNET::API::GetNativeImplementation()->sdk()->resource->release;
    fn(m_handle);
}

REFrameworkNET::ManagedObject^ REFrameworkNET::Resource::CreateHolder(System::String^ typeName) {
    if (m_handle == nullptr) return nullptr;

    auto nativeTypeName = msclr::interop::marshal_as<std::string>(typeName);
    static const auto fn = REFrameworkNET::API::GetNativeImplementation()->sdk()->resource->create_holder;
    auto result = fn(m_handle, nativeTypeName.c_str());

    if (result == nullptr) return nullptr;

    return REFrameworkNET::ManagedObject::Get((reframework::API::ManagedObject*)result);
}
