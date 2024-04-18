#include <string_view>

#include "TypeDefinition.hpp"
#include "TypeInfo.hpp"
#include "InvokeRet.hpp"
#include "Method.hpp"

#include "ManagedObject.hpp"
#include "Proxy.hpp"
#include "SystemString.hpp"

#include "API.hpp"

#include "Utility.hpp"

using namespace ImGuiNET;

namespace REFrameworkNET {
    void ManagedObject::CleanupKnownCaches() {
        Cache<ManagedObject>::CleanupAll();
        Cache<SystemString>::CleanupAll();
    }

    void ManagedObject::Internal_Finalize() {
        if (m_object == nullptr) {
            return;
        }

        ReleaseIfGlobalized();

        // Only if we are not marked as weak are we allowed to release the object, even if the object is globalized
        if (m_cached) {
            if (!ShuttingDown) {
                m_finalizerDelegate(this);
            } else {
                // Nothing?
            }
        }
    }

    ManagedObject^ ManagedObject::Globalize() {
        if (m_object == nullptr) {
            return this;
        }

        if (IsGlobalized()) {
            // If someone wants to globalize a weak reference, upgrade it to a concrete reference
            if (m_weak) {
                AddRef();
            }

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
        m_weak = false; // Once we add a concrete reference, we can't be weak anymore
    }

    void ManagedObject::Release() {
        if (m_object == nullptr || m_weak || !m_initialized) {
            return;
        }

#ifdef REFRAMEWORK_VERBOSE
        System::Console::WriteLine("Releasing a reference to" + GetTypeDefinition()->FullName);
#endif

        m_object->release();
        m_weak = true;
    }

    TypeDefinition^ ManagedObject::GetTypeDefinition() {
        auto result = m_object->get_type_definition();

        if (result == nullptr) {
            return nullptr;
        }

        return TypeDefinition::GetInstance(result);
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