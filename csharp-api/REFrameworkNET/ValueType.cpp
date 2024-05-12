#include "Proxy.hpp"
#include "TDB.hpp"
#include "ValueType.hpp"

namespace REFrameworkNET {
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