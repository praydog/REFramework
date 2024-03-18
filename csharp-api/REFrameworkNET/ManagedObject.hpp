#pragma once

#include <reframework/API.hpp>

#pragma managed

namespace REFrameworkNET {
ref class TypeDefinition;

public ref class ManagedObject {
public:
    ManagedObject(reframework::API::ManagedObject* obj) : m_object(obj) {
        AddRef();
    }
    ManagedObject(::REFrameworkManagedObjectHandle handle) : m_object(reinterpret_cast<reframework::API::ManagedObject*>(handle)) {
        AddRef();
    }

    ~ManagedObject() {
        if (m_object != nullptr) {
            Release();
        }
    }

    void AddRef() {
        m_object->add_ref();
    }

    void Release() {
        m_object->release();
    }

    void* Ptr() {
        return (void*)m_object;
    }

    uintptr_t GetAddress() {
        return (uintptr_t)m_object;
    }

    TypeDefinition^ GetTypeDefinition();

private:
    reframework::API::ManagedObject* m_object;
};
}