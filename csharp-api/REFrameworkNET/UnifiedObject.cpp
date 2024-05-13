#include "API.hpp"

#include "TypeDefinition.hpp"
#include "UnifiedObject.hpp"

#include "./Utility.hpp"

namespace REFrameworkNET {
    REFrameworkNET::InvokeRet^ UnifiedObject::Invoke(System::String^ methodName, array<System::Object^>^ args) {
        // Get method
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

    bool UnifiedObject::HandleInvokeMember_Internal(System::String^ methodName, array<System::Object^>^ args, System::Object^% result) {
        auto t = this->GetTypeDefinition();

        if (t == nullptr) {
            return false;
        }

        auto method = t->FindMethod(methodName);

        if (method != nullptr)
        {
            // Re-used with UnifiedObject::TryInvokeMember
            return method->HandleInvokeMember_Internal(this, args, result);
        }

        REFrameworkNET::API::LogInfo("Method not found: " + methodName);

        result = nullptr;
        return false;
    }

    bool UnifiedObject::HandleInvokeMember_Internal(uint32_t methodIndex, array<System::Object^>^ args, System::Object^% result) {
        auto method = REFrameworkNET::TDB::Get()->GetMethod(methodIndex);

        if (method != nullptr)
        {
            // Re-used with UnifiedObject::TryInvokeMember
            return method->HandleInvokeMember_Internal(this, args, result);
        }

        result = nullptr;
        return false;
    }

    bool UnifiedObject::HandleInvokeMember_Internal(System::Object^ methodObj, array<System::Object^>^ args, System::Object^% result) {
        auto method = dynamic_cast<REFrameworkNET::Method^>(methodObj);
        if (method != nullptr)
        {
            // Re-used with UnifiedObject::TryInvokeMember
            return method->HandleInvokeMember_Internal(this, args, result);
        }

        result = nullptr;
        return false;
    }

    bool UnifiedObject::TryInvokeMember(System::Dynamic::InvokeMemberBinder^ binder, array<System::Object^>^ args, System::Object^% result)
    {
        auto methodName = binder->Name;
        return HandleInvokeMember_Internal(methodName, args, result);
    }

    bool UnifiedObject::TryGetMember(System::Dynamic::GetMemberBinder^ binder, System::Object^% result) {
        auto memberName = binder->Name;
        return HandleTryGetMember_Internal(memberName, result);
    }

    bool UnifiedObject::HandleTryGetMember_Internal(System::String^ memberName, System::Object^% result) {
        auto t = this->GetTypeDefinition();

        if (t == nullptr) {
            return false;
        }

        auto field = t->FindField(memberName);

        if (field != nullptr)
        {
            result = field->GetDataBoxed(this->GetAddress(), !this->IsManaged() && this->GetTypeDefinition()->IsValueType());

            return true;
        }

        /*auto property = t->FindProperty(memberName);

        if (property != nullptr)
        {
            result = property->GetValue(this);
            return true;
        }*/

        REFrameworkNET::API::LogInfo("Member not found: " + memberName);

        result = nullptr;
        return false;
    }

    bool UnifiedObject::TrySetMember(System::Dynamic::SetMemberBinder^ binder, System::Object^ value)
    {
        auto memberName = binder->Name;
        auto t = this->GetTypeDefinition();

        if (t == nullptr) {
            return false;
        }

        auto field = t->FindField(memberName);

        if (field != nullptr)
        {
            field->SetDataBoxed(this->GetAddress(), value, !this->IsManaged() && this->GetTypeDefinition()->IsValueType());
            return true;
        }

        return false;
    }
}