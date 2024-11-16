#pragma once

#include <cstdint>

#include "TypeDefinition.hpp"
#include "InvokeRet.hpp"
#include "UnifiedObject.hpp"

#include "ObjectEnumerator.hpp"

namespace REFrameworkNET {
value struct InvokeRet;

/// <summary>
// Native objects are objects that are NOT managed objects <br/>
// However, they still have reflection information associated with them <br/>
// So this intends to be the "ManagedObject" class for native objects <br/>
// So we can easily interact with them in C# <br/>
/// </summary>
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

    /// <returns>The type of the object</returns>
    virtual TypeDefinition^ GetTypeDefinition() override {
        return m_type;
    }

    /// <returns>The address of the object as a void* pointer</returns>
    virtual void* Ptr() override {
        return m_object;
    }

    /// <returns>The address of the object</returns>
    virtual uintptr_t GetAddress() override  {
        return (uintptr_t)m_object;
    }

    generic <typename T>
    virtual T As() override;

    /// <summary>
    /// Creates a NativeObject wrapper over the given address
    /// </summary>
    /// <remarks>
    /// This function can be very dangerous if used incorrectly. <br/>
    /// Always double check that you are feeding the correct address <br/>
    /// and type to this function. <br/>
    /// </remarks>
    /// <param name="obj">The address of the object</param>
    /// <param name="t">The type of the object</param>
    /// <returns>A NativeObject wrapper over the given address</returns>
    static NativeObject^ FromAddress(uintptr_t obj, TypeDefinition^ t) {
        return gcnew NativeObject(obj, t);
    }

    virtual void AddProxy(System::Type^ type, IProxy^ proxy) override {
        // Nothing, don't bother
    }

protected:
    void* m_object{};
    TypeDefinition^ m_type{};
};
}