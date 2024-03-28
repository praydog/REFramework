#pragma once

#include <cstdint>

#include "IProxyable.hpp"

namespace REFrameworkNET {
ref class TypeDefinition;
ref struct InvokeRet;

// Base interface of ManagedObject and NativeObject
public interface class IObject : public IProxyable {
    InvokeRet^ Invoke(System::String^ methodName, array<System::Object^>^ args);

    TypeDefinition^ GetTypeDefinition();

    // For interface types
    generic <typename T>
    T As();
};
}