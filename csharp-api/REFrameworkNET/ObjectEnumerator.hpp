#pragma once

#include "IObject.hpp"

namespace REFrameworkNET {
ref class ObjectEnumerator : public System::Collections::IEnumerator {
private:
    IObject^ m_source;
    IObject^ m_enumerator; // ValueType or ManagedObject returned by GetEnumerator()

    void AcquireEnumerator() {
        m_enumerator = nullptr;
        System::Object^ result = nullptr;
        if (m_source->HandleInvokeMember_Internal("GetEnumerator", gcnew array<System::Object^>{}, result)) {
            m_enumerator = dynamic_cast<IObject^>(result);
        }
    }

public:
    ObjectEnumerator(IObject^ source) : m_source(source) {
        AcquireEnumerator();
    }

    virtual bool MoveNext() {
        if (m_enumerator == nullptr) {
            return false;
        }

        System::Object^ result = nullptr;
        if (m_enumerator->HandleInvokeMember_Internal("MoveNext", gcnew array<System::Object^>{}, result)) {
            if (result != nullptr) {
                return (bool)result;
            }
        }
        return false;
    }

    virtual void Reset() {
        AcquireEnumerator();
    }

    virtual property System::Object^ Current {
        System::Object^ get() {
            if (m_enumerator == nullptr) {
                throw gcnew System::InvalidOperationException("ObjectEnumerator: no enumerator available");
            }

            System::Object^ result = nullptr;
            if (m_enumerator->HandleInvokeMember_Internal("get_Current", gcnew array<System::Object^>{}, result)) {
                return result;
            }

            return nullptr;
        }
    }
};
}
