#pragma once

#include <reframework/API.hpp>

#pragma managed

namespace REFrameworkNET {
ref class TypeDefinition;
ref class TypeInfo;
ref class InvokeRet;

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

    static bool IsManagedObject(uintptr_t ptr) {
        static auto fn = reframework::API::get()->param()->sdk->managed_object->is_managed_object;
        return fn((void*)ptr);
    }

    TypeDefinition^ GetTypeDefinition();
    TypeInfo^ GetTypeInfo();

    REFrameworkNET::InvokeRet^ Invoke(System::String^ methodName, array<System::Object^>^ args);

    // TODO methods:
    /*public Void* GetReflectionProperties() {
        return _original.get_reflection_properties();
    }*/

    /*public ReflectionProperty GetReflectionPropertyDescriptor(basic_string_view<char\, std::char_traits<char>> name) {
        return _original.get_reflection_property_descriptor(name);
    }

    public ReflectionMethod GetReflectionMethodDescriptor(basic_string_view<char\, std::char_traits<char>> name) {
        return _original.get_reflection_method_descriptor(name);
    }*/

private:
    reframework::API::ManagedObject* m_object;
};
}