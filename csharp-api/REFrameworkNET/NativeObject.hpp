#pragma once

#include <cstdint>

#include "TypeDefinition.hpp"
#include "InvokeRet.hpp"
#include "UnifiedObject.hpp"

#include "ObjectEnumerator.hpp"

namespace REFrameworkNET {
value struct InvokeRet;

// Native objects are objects that are NOT managed objects
// However, they still have reflection information associated with them
// So this intends to be the "ManagedObject" class for native objects
// So we can easily interact with them in C#
public ref class NativeObject : public REFrameworkNET::UnifiedObject
{
public:
    NativeObject(uintptr_t obj, TypeDefinition^ t){
        if (t == nullptr) {
            throw gcnew System::ArgumentNullException("t");
        }

        m_object = (void*)obj;
        m_type = t;
    }

    NativeObject(void* obj, TypeDefinition^ t) {
        if (t == nullptr) {
            throw gcnew System::ArgumentNullException("t");
        }

        m_object = obj;
        m_type = t;
    }

    // For invoking static methods
    // e.g. NativeObject^ obj = new NativeObject(TypeDefinition::GetType("System.AppDomain"));
    // obj.get_CurrentDomain().GetAssemblies();
    NativeObject(TypeDefinition^ t) {
        if (t == nullptr) {
            throw gcnew System::ArgumentNullException("t");
        }

        m_type = t;
        m_object = nullptr;
    }

    virtual TypeDefinition^ GetTypeDefinition() override {
        return m_type;
    }

    virtual void* Ptr() override {
        return m_object;
    }

    virtual uintptr_t GetAddress() override  {
        return (uintptr_t)m_object;
    }

    generic <typename T>
    virtual T As() override;

private:
    void* m_object{};
    TypeDefinition^ m_type{};
};
}