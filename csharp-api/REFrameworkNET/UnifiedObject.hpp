#pragma once

#include "IObject.hpp"
#include "ObjectEnumerator.hpp"

namespace REFrameworkNET {
ref class TypeDefinition;
ref struct InvokeRet;

// UnifiedObject is the base class that ManagedObject and NativeObject will derive from
// It will have several shared methods but some unimplemented methods that will be implemented in the derived classes
public ref class UnifiedObject abstract : public System::Dynamic::DynamicObject, public System::Collections::IEnumerable, public REFrameworkNET::IObject {
public:
    virtual bool IsProxy() {
        return false;
    }

    // These methods will be implemented in the derived classes
    virtual void* Ptr() abstract = 0;
    virtual uintptr_t GetAddress() abstract = 0;
    virtual REFrameworkNET::TypeDefinition^ GetTypeDefinition() abstract = 0;

    generic <typename T>
    virtual T As() abstract = 0;
    
    // Shared methods
    virtual bool HandleInvokeMember_Internal(System::String^ methodName, array<System::Object^>^ args, System::Object^% result);
    virtual bool HandleInvokeMember_Internal(uint32_t methodIndex, array<System::Object^>^ args, System::Object^% result);
    virtual bool HandleInvokeMember_Internal(System::Object^ methodObj, array<System::Object^>^ args, System::Object^% result);
    
    virtual bool HandleTryGetMember_Internal(System::String^ fieldName, System::Object^% result);

    virtual REFrameworkNET::InvokeRet^ Invoke(System::String^ methodName, array<System::Object^>^ args);
    virtual bool TryInvokeMember(System::Dynamic::InvokeMemberBinder^ binder, array<System::Object^>^ args, System::Object^% result) override;
    virtual bool TryGetMember(System::Dynamic::GetMemberBinder^ binder, System::Object^% result) override;
    virtual bool TrySetMember(System::Dynamic::SetMemberBinder^ binder, System::Object^ value) override;
    
    virtual System::Object^ Call(System::String^ methodName, ... array<System::Object^>^ args) {
        System::Object^ result = nullptr;
        HandleInvokeMember_Internal(methodName, args, result);

        return result;
    }

    virtual System::Object^ GetField(System::String^ fieldName) {
        System::Object^ result = nullptr;
        if (!HandleTryGetMember_Internal(fieldName, result)) {
            if (!HandleInvokeMember_Internal("get_" + fieldName, gcnew array<System::Object^>{}, result)) {
                // TODO? what else can we do here?
            }
        }

        return result;
    }

public:
    virtual bool Equals(System::Object^ other) override {
        if (System::Object::ReferenceEquals(this, other)) {
            return true;
        }

        if (System::Object::ReferenceEquals(other, nullptr)) {
            return false;
        }

        if (!other->GetType()->IsSubclassOf(IObject::typeid)) {
            return false;
        }

        return Ptr() == safe_cast<IObject^>(other)->Ptr();
    }

    // Override equality operator
    virtual bool Equals(IObject^ other) {
        if (System::Object::ReferenceEquals(this, other)) {
            return true;
        }

        if (System::Object::ReferenceEquals(other, nullptr)) {
            return false;
        }

        return Ptr() == other->Ptr();
    }

    static bool operator ==(UnifiedObject^ left, UnifiedObject^ right) {
        if (System::Object::ReferenceEquals(left, right)) {
            return true;
        }

        if (System::Object::ReferenceEquals(left, nullptr) || System::Object::ReferenceEquals(right, nullptr)) {
            return false;
        }

        return left->Ptr() == right->Ptr();
    }

    static bool operator !=(UnifiedObject^ left, UnifiedObject^ right) {
        return !(left == right);
    }

public:
    // IEnumerable implementation
    virtual System::Collections::IEnumerator^ GetEnumerator() {
        return gcnew REFrameworkNET::ObjectEnumerator(this);
    }
};
}