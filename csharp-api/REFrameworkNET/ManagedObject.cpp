#include "TypeDefinition.hpp"
#include "TypeInfo.hpp"
#include "InvokeRet.hpp"
#include "Method.hpp"

#include "ManagedObject.hpp"

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
}