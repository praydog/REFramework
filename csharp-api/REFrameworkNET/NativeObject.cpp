#include "InvokeRet.hpp"
#include "Method.hpp"
#include "NativeObject.hpp"
#include "Proxy.hpp"

#include "API.hpp"

namespace REFrameworkNET {
InvokeRet^ NativeObject::Invoke(System::String^ methodName, array<System::Object^>^ args) {
    auto t = this->GetTypeDefinition();
    
    if (t == nullptr) {
        return nullptr;
    }

    auto m = t->GetMethod(methodName);

    if (m == nullptr) {
        return nullptr;
    }

    return m->Invoke(this, args);
}

bool NativeObject::HandleInvokeMember_Internal(System::String^ methodName, array<System::Object^>^ args, System::Object^% result) {
    auto t = this->GetTypeDefinition();
    
    if (t == nullptr) {
        return false;
    }

    auto m = t->GetMethod(methodName);

    if (m == nullptr) {   
        REFrameworkNET::API::LogInfo("Method not found: " + methodName);
        return false;
    }

    return m->HandleInvokeMember_Internal(this, methodName, args, result);
}

bool NativeObject::TryInvokeMember(System::Dynamic::InvokeMemberBinder^ binder, array<System::Object^>^ args, System::Object^% result)
{
    return HandleInvokeMember_Internal(binder->Name, args, result);
}

generic <typename T>
T NativeObject::As() {
    return NativeProxy<T>::Create(this);
}
}