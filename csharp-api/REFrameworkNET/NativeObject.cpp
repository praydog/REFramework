#include "InvokeRet.hpp"
#include "Method.hpp"
#include "NativeObject.hpp"

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

bool NativeObject::TryInvokeMember(System::Dynamic::InvokeMemberBinder^ binder, array<System::Object^>^ args, System::Object^% result)
{
    auto methodName = binder->Name;
    auto method = m_type->FindMethod(methodName);

    if (method != nullptr)
    {
        // Re-used with ManagedObject::TryInvokeMember
        return method->HandleInvokeMember_Internal(this, binder, args, result);
    }

    REFrameworkNET::API::LogInfo("Method not found: " + methodName);

    result = nullptr;
    return false;
}
}