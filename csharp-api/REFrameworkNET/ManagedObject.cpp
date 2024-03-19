#include "TypeDefinition.hpp"
#include "TypeInfo.hpp"
#include "InvokeRet.hpp"
#include "Method.hpp"

#include "ManagedObject.hpp"

#include "API.hpp"

namespace REFrameworkNET {
    TypeDefinition^ ManagedObject::GetTypeDefinition() {
        auto result = m_object->get_type_definition();

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew TypeDefinition(result);
    }

    TypeInfo^ ManagedObject::GetTypeInfo() {
        auto result = m_object->get_type_info();

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew TypeInfo(result);
    }

    REFrameworkNET::InvokeRet^ ManagedObject::Invoke(System::String^ methodName, array<System::Object^>^ args) {
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

    bool ManagedObject::TryInvokeMember(System::Dynamic::InvokeMemberBinder^ binder, array<System::Object^>^ args, System::Object^% result)
    {
        auto methodName = binder->Name;
        auto t = this->GetTypeDefinition();

        if (t == nullptr) {
            return false;
        }

        auto method = t->FindMethod(methodName);

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