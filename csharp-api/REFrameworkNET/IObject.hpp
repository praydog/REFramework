#pragma once

#include <cstdint>

#include "IProxyable.hpp"

namespace REFrameworkNET {
ref class TypeDefinition;
interface class IProxy;
value struct InvokeRet;

// Base interface of ManagedObject and NativeObject
public interface class IObject : public IProxyable, public System::IEquatable<IObject^> {
    InvokeRet^ Invoke(System::String^ methodName, array<System::Object^>^ args);
    System::Object^ Call(System::String^ methodName, ... array<System::Object^>^ args);
    System::Object^ GetField(System::String^ fieldName);

    TypeDefinition^ GetTypeDefinition();

    // For interface types
    generic <typename T>
    T As();

    IProxy^ GetProxy(System::Type^ proxyType);
};
}