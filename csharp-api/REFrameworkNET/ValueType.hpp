#pragma once

#include "UnifiedObject.hpp"
#include "TypeDefinition.hpp"
#include "ObjectAddress.hpp"

namespace REFrameworkNET {
public ref class ValueType : public UnifiedObject {
public:
    ValueType(TypeDefinition^ t) 
    {
        m_data = gcnew array<uint8_t>(t->ValueTypeSize);
        m_type = t;
    }

    /// <returns>The type of the object</returns>
    virtual TypeDefinition^ GetTypeDefinition() override {
        return m_type;
    }

    // The returned pointer is not pinned after this call. Internal consumers
    // must use ObjectAddress for the entire native operation. External callers
    // of this low-level API must retain their own pin on the backing storage.
    /// <returns>The address of the object as a void* pointer</returns>
    virtual void* Ptr() override {
        pin_ptr<uint8_t> data = &m_data[0];
        return data;
    }

    /// <returns>The address of the object</returns>
    virtual uintptr_t GetAddress() override  {
        return (uintptr_t)Ptr();
    }

    virtual void AddProxy(System::Type^ type, IProxy^ proxy) override {
        // Nothing, don't bother
    }

    generic <typename T>
    virtual T As() override;

    generic <typename T>
    where T : ref class
    static T New();

internal:
    property array<uint8_t>^ Data {
        array<uint8_t>^ get() { return m_data; }
    }

private:
    array<uint8_t>^ m_data{};
    TypeDefinition^ m_type{};
};

}
