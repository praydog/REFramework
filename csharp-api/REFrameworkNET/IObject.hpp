#pragma once

#include <cstdint>

namespace REFrameworkNET {
ref class TypeDefinition;
ref struct InvokeRet;

// Base interface of ManagedObject and NativeObject
public interface class IObject {
    TypeDefinition^ GetTypeDefinition();
    void* Ptr();
    uintptr_t GetAddress();

    virtual InvokeRet^ Invoke(System::String^ methodName, array<System::Object^>^ args);
    bool HandleInvokeMember_Internal(System::String^ methodName, array<System::Object^>^ args, System::Object^% result);
};
}