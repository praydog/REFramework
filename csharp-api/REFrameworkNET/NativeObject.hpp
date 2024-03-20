#pragma once

#include <cstdint>

#include "TypeDefinition.hpp"

namespace REFrameworkNET {
ref class InvokeRet;

// Native objects are objects that are NOT managed objects
// However, they still have reflection information associated with them
// So this intends to be the "ManagedObject" class for native objects
// So we can easily interact with them in C#
public ref class NativeObject : public System::Dynamic::DynamicObject
{
public:
    NativeObject(uintptr_t obj, TypeDefinition^ t){
        m_object = (void*)obj;
        m_type = t;
    }

    NativeObject(void* obj, TypeDefinition^ t){
        m_object = obj;
        m_type = t;
    }

    TypeDefinition^ GetTypeDefinition() {
        return m_type;
    }

    void* Ptr() {
        return m_object;
    }

    uintptr_t GetAddress() {
        return (uintptr_t)m_object;
    }

    InvokeRet^ Invoke(System::String^ methodName, array<System::Object^>^ args);

    bool HandleInvokeMember_Internal(System::String^ methodName, array<System::Object^>^ args, System::Object^% result);
    virtual bool TryInvokeMember(System::Dynamic::InvokeMemberBinder^ binder, array<System::Object^>^ args, System::Object^% result) override;

private:
    void* m_object{};
    TypeDefinition^ m_type{};
};
}