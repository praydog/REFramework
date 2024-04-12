#pragma once

#include <cstdint>

namespace REFrameworkNET {
ref struct InvokeRet;

public interface class IProxyable {
    void* Ptr();
    uintptr_t GetAddress();
    virtual bool IsProxy();

    bool HandleInvokeMember_Internal(System::String^ methodName, array<System::Object^>^ args, System::Object^% result);
    bool HandleInvokeMember_Internal(uint32_t methodIndex, array<System::Object^>^ args, System::Object^% result);
    bool HandleInvokeMember_Internal(System::Object^ methodObj, array<System::Object^>^ args, System::Object^% result);
    
    bool HandleTryGetMember_Internal(System::String^ fieldName, System::Object^% result);
};
}