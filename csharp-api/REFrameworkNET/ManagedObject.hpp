#pragma once

#include <reframework/API.hpp>
#include "IObject.hpp"

#pragma managed

namespace REFrameworkNET {
ref class TypeDefinition;
ref class TypeInfo;
ref class InvokeRet;
ref class ManagedObject;

public ref class ManagedObject : public System::Dynamic::DynamicObject, public System::IEquatable<ManagedObject^>, public REFrameworkNET::IObject
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
        m_object->add_ref();
    }

    void Release() {
        m_object->release();
    }

    virtual void* Ptr() {
        return (void*)m_object;
    }

    virtual uintptr_t GetAddress() {
        return (uintptr_t)m_object;
    }

    virtual bool IsProxy() {
        return false;
    }

    virtual bool IsProperObject() {
        return true;
    }

    virtual bool Equals(System::Object^ other) override {
        if (System::Object::ReferenceEquals(this, other)) {
            return true;
        }

        if (System::Object::ReferenceEquals(other, nullptr)) {
            return false;
        }

        if (other->GetType() != ManagedObject::typeid) {
            return false;
        }

        return Ptr() == safe_cast<ManagedObject^>(other)->Ptr();
    }

    // Override equality operator
    virtual bool Equals(ManagedObject^ other) {
        if (System::Object::ReferenceEquals(this, other)) {
            return true;
        }

        if (System::Object::ReferenceEquals(other, nullptr)) {
            return false;
        }

        return Ptr() == other->Ptr();
    }

    static bool operator ==(ManagedObject^ left, ManagedObject^ right) {
        if (System::Object::ReferenceEquals(left, right)) {
            return true;
        }

        if (System::Object::ReferenceEquals(left, nullptr) || System::Object::ReferenceEquals(right, nullptr)) {
            return false;
        }

        return left->Ptr() == right->Ptr();
    }

    static bool operator !=(ManagedObject^ left, ManagedObject^ right) {
        return !(left == right);
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

    virtual TypeDefinition^ GetTypeDefinition();
    TypeInfo^ GetTypeInfo();

    virtual bool HandleInvokeMember_Internal(System::String^ methodName, array<System::Object^>^ args, System::Object^% result);

    virtual REFrameworkNET::InvokeRet^ Invoke(System::String^ methodName, array<System::Object^>^ args);
    virtual bool TryInvokeMember(System::Dynamic::InvokeMemberBinder^ binder, array<System::Object^>^ args, System::Object^% result) override;
    virtual bool TryGetMember(System::Dynamic::GetMemberBinder^ binder, System::Object^% result) override;
    virtual bool TrySetMember(System::Dynamic::SetMemberBinder^ binder, System::Object^ value) override;

    generic <typename T>
    virtual T As();

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