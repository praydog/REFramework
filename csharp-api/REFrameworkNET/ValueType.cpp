#include "Proxy.hpp"
#include "TDB.hpp"
#include "ValueType.hpp"

namespace REFrameworkNET {
ObjectAddress::ObjectAddress(IObject^ object) : m_object(object) {
    while (auto proxy = dynamic_cast<IProxy^>(object)) {
        object = proxy->GetInstance();
    }
    m_object = object;
    if (object == nullptr) {
        return;
    }

    if (auto value = dynamic_cast<ValueType^>(object)) {
        using namespace System::Runtime::InteropServices;
        auto pin = GCHandle::Alloc(value->Data, GCHandleType::Pinned);
        // If acquiring the address throws, this constructor has no destructor.
        try {
            m_address = pin.AddrOfPinnedObject().ToPointer();
            m_pin = GCHandle::ToIntPtr(pin).ToInt64();
        } catch (...) {
            pin.Free();
            throw;
        }
    } else {
        m_address = object->Ptr();
    }
}

ObjectAddress::~ObjectAddress() {
    if (m_pin != 0) {
        System::Runtime::InteropServices::GCHandle::FromIntPtr(System::IntPtr(m_pin)).Free();
    }
}

generic <typename T>
T ValueType::As() {
    return AnyProxy<T>::Create(this);
}

generic <typename T>
T ValueType::New() {
    auto t = TDB::TypeCacher<T>::GetCachedType();
    auto valueType = gcnew ValueType(t);
    return valueType->As<T>();
}
}
