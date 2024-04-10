#pragma once

#include <reframework/API.hpp>
#include "IObject.hpp"

#include "ObjectEnumerator.hpp"
#include "UnifiedObject.hpp"

#pragma managed

namespace REFrameworkNET {
ref class TypeDefinition;
ref class TypeInfo;
ref class InvokeRet;
ref class ManagedObject;

public ref class ManagedObject : public REFrameworkNET::UnifiedObject
{
public:
    ManagedObject(reframework::API::ManagedObject* obj) : m_object(obj) {
        if (obj != nullptr) {
            AddRef();
        }
    }
    ManagedObject(::REFrameworkManagedObjectHandle handle) : m_object(reinterpret_cast<reframework::API::ManagedObject*>(handle)) {
        if (handle != nullptr) {
            AddRef();
        }
    }

    // Double check if we really want to allow this
    // We might be better off having a global managed object cache
    // instead of AddRef'ing every time we create a new ManagedObject
    ManagedObject(ManagedObject^ obj) : m_object(obj->m_object) {
        if (m_object != nullptr) {
            AddRef();
        }
    }

    ~ManagedObject() {
        if (m_object != nullptr) {
            Release();
        }
    }

    void AddRef() {
        if (m_object == nullptr) {
            return;
        }

        m_object->add_ref();
    }

    void Release() {
        if (m_object == nullptr) {
            return;
        }

        m_object->release();
    }

    static bool IsManagedObject(uintptr_t ptr) {
        if (ptr == 0) {
            return false;
        }

        static auto fn = reframework::API::get()->param()->sdk->managed_object->is_managed_object;
        return fn((void*)ptr);
    }

    static ManagedObject^ ToManagedObject(uintptr_t ptr) {
        if (ptr == 0) {
			return nullptr;
		}

        if (IsManagedObject(ptr)) {
            return gcnew ManagedObject((reframework::API::ManagedObject*)ptr);
        }

        return nullptr;
    }

    static ManagedObject^ FromAddress(uintptr_t ptr) {
        return ToManagedObject(ptr);
    }

    TypeInfo^ GetTypeInfo();

public: // IObject
    virtual void* Ptr() override {
        return (void*)m_object;
    }

    virtual uintptr_t GetAddress() override  {
        return (uintptr_t)m_object;
    }

    virtual TypeDefinition^ GetTypeDefinition() override;

    generic <typename T>
    virtual T As() override;

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

protected:
    reframework::API::ManagedObject* m_object;
};
}