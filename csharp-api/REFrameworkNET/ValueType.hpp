#pragma once

#include <vcclr.h>

#include "UnifiedObject.hpp"
#include "TypeDefinition.hpp"

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

// Native RAII scope: root the wrapper and pin CLR-backed storage before exposing
// its address. Never retain a pointer returned by ValueType::Ptr() and pin later.
// The scope also roots native/managed engine wrappers until the last native use.
class ObjectAddress {
public:
    explicit ObjectAddress(IObject^ object);
    ~ObjectAddress();
    ObjectAddress(const ObjectAddress&) = delete;
    ObjectAddress& operator=(const ObjectAddress&) = delete;

    void* Ptr() const { return m_address; }
    uintptr_t Address() const { return reinterpret_cast<uintptr_t>(m_address); }
    IObject^ Instance() const { return m_object; }

private:
    gcroot<IObject^> m_object;
    intptr_t m_pin{};
    void* m_address{};
};
}
