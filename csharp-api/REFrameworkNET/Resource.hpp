#pragma once

#include <reframework/API.h>

#pragma managed

namespace REFrameworkNET {

ref class ManagedObject;

/// Wraps a native RE Engine resource handle (via.Resource).
/// Returned by API::CreateResource. Supports reference counting and holder creation.
public ref class Resource {
public:
    Resource(REFrameworkResourceHandle handle) : m_handle(handle) {}

    /// The underlying native handle.
    property System::IntPtr Handle {
        System::IntPtr get() { return System::IntPtr(m_handle); }
    }

    /// Increment the native reference count.
    void AddRef();

    /// Decrement the native reference count.
    void Release();

    /// Create a resource holder ManagedObject for the given type name.
    /// typeName is a TypeDefinition name (e.g. "via.render.TextureResource").
    ManagedObject^ CreateHolder(System::String^ typeName);

internal:
    REFrameworkResourceHandle GetHandle() { return m_handle; }

private:
    REFrameworkResourceHandle m_handle;
};

}
