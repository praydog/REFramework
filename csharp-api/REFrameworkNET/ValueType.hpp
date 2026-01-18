#pragma once

#include "UnifiedObject.hpp"
#include "TypeDefinition.hpp"

namespace REFrameworkNET {
public ref class ValueType : public UnifiedObject {
public:
    ValueType(TypeDefinition^ t) 
    {
        m_data = gcnew array<uint8_t>(t->ValueTypeSize);
        m_type = t;
        pin_ptr<uint8_t> data = &m_data[0];
    }

    /// <returns>The type of the object</returns>
    virtual TypeDefinition^ GetTypeDefinition() override {
        return m_type;
    }

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

private:
    array<uint8_t>^ m_data{};
    TypeDefinition^ m_type{};
};
}