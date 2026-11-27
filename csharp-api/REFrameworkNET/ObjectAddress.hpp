#pragma once

#include <cstdint>
#include <vcclr.h>

// Include after defining IObject, or importing it from REFramework.NET.dll.
namespace REFrameworkNET {
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
