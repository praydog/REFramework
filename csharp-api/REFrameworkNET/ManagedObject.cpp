#include <string_view>

#include "TypeDefinition.hpp"
#include "TypeInfo.hpp"
#include "InvokeRet.hpp"
#include "Method.hpp"

#include "ManagedObject.hpp"
#include "Proxy.hpp"

#include "API.hpp"

#include "Utility.hpp"

namespace REFrameworkNET {
    ManagedObject^ ManagedObject::Globalize() {
        if (m_object == nullptr || IsGlobalized()) {
            return this;
        }

        AddRef();
        return this;
    }

    void ManagedObject::AddRef() {
        if (m_object == nullptr) {
            return;
        }

#ifdef REFRAMEWORK_VERBOSE
        if (!IsGlobalized()) {
            System::Console::WriteLine("WARNING: Globalizing managed object " + GetTypeDefinition()->FullName + " with negative ref count: " + ref_count);
        }
#endif

        m_object->add_ref();
    }

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

    generic <typename T>
    T ManagedObject::As() {
        return ManagedProxy<T>::Create(this);
    }
}