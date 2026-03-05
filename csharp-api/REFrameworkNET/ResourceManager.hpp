#pragma once

#include <reframework/API.h>

#pragma managed

#include "Resource.hpp"

namespace REFrameworkNET {

ref class ManagedObject;

/// Wraps the native RE Engine ResourceManager.
/// Obtain via API::GetResourceManager().
public ref class ResourceManager {
public:
    ResourceManager(REFrameworkResourceManagerHandle handle) : m_handle(handle) {}

    /// Creates a new resource from a PAK path.
    /// typeName is a via.typeinfo.TypeInfo name (NOT a TypeDefinition name).
    /// name is the resource path in the PAK (e.g. "path/to/resource.tex").
    /// Returns nullptr if the type is not found or creation fails.
    Resource^ CreateResource(System::String^ typeName, System::String^ name);

    /// Creates a new userdata managed object.
    /// typeName is a via.typeinfo.TypeInfo name (NOT a TypeDefinition name).
    /// name is the resource path / identifier.
    /// Returns nullptr if the type is not found or creation fails.
    ManagedObject^ CreateUserData(System::String^ typeName, System::String^ name);

private:
    REFrameworkResourceManagerHandle m_handle;
};

}
