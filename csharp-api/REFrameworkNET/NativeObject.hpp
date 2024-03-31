#pragma once

#include <cstdint>

#include "TypeDefinition.hpp"
#include "InvokeRet.hpp"
#include "IObject.hpp"

namespace REFrameworkNET {
ref class InvokeRet;

// Native objects are objects that are NOT managed objects
// However, they still have reflection information associated with them
// So this intends to be the "ManagedObject" class for native objects
// So we can easily interact with them in C#
public ref class NativeObject : public System::Dynamic::DynamicObject, public System::Collections::IEnumerable, public REFrameworkNET::IObject
{
public:
    NativeObject(uintptr_t obj, TypeDefinition^ t){
        if (t == nullptr) {
            throw gcnew System::ArgumentNullException("t");
        }

        m_object = (void*)obj;
        m_type = t;
    }

    NativeObject(void* obj, TypeDefinition^ t) {
        if (t == nullptr) {
            throw gcnew System::ArgumentNullException("t");
        }

        m_object = obj;
        m_type = t;
    }

    // For invoking static methods
    // e.g. NativeObject^ obj = new NativeObject(TypeDefinition::GetType("System.AppDomain"));
    // obj.get_CurrentDomain().GetAssemblies();
    NativeObject(TypeDefinition^ t) {
        if (t == nullptr) {
            throw gcnew System::ArgumentNullException("t");
        }

        m_type = t;
        m_object = nullptr;
    }

    virtual TypeDefinition^ GetTypeDefinition() {
        return m_type;
    }

    virtual void* Ptr() {
        return m_object;
    }

    virtual uintptr_t GetAddress() {
        return (uintptr_t)m_object;
    }

    virtual bool IsProxy() {
        return false;
    }

    virtual bool IsProperObject() {
        return true;
    }

    virtual InvokeRet^ Invoke(System::String^ methodName, array<System::Object^>^ args);

    virtual bool HandleInvokeMember_Internal(System::String^ methodName, array<System::Object^>^ args, System::Object^% result);
    virtual bool TryInvokeMember(System::Dynamic::InvokeMemberBinder^ binder, array<System::Object^>^ args, System::Object^% result) override;

    generic <typename T>
    virtual T As();

public:
    // IEnumerable implementation
    virtual System::Collections::IEnumerator^ GetEnumerator() {
        return gcnew NativeObjectEnumerator(this);
    }

private:
    ref class NativeObjectEnumerator : public System::Collections::IEnumerator
    {
        int position = -1;
        NativeObject^ nativeObject;

    public:
        NativeObjectEnumerator(NativeObject^ nativeObject) {
            this->nativeObject = nativeObject;
        }

        // IEnumerator implementation
        virtual bool MoveNext() {
            int itemCount = GetItemCount();
            if (position < itemCount - 1) {
                position++;
                return true;
            }
            return false;
        }

        virtual void Reset() {
            position = -1;
        }

        virtual property System::Object^ Current {
            System::Object^ get() {
                if (position == -1 || position >= GetItemCount()) {
                    throw gcnew System::InvalidOperationException();
                }

                System::Object^ result = nullptr;
                if (nativeObject->HandleInvokeMember_Internal("get_Item", gcnew array<System::Object^>{ position }, result)) {
                    return result;
                }

                return nullptr;
            }
        }

    private:
        int GetItemCount() {
            //return nativeObject->Invoke("get_Count", gcnew array<System::Object^>{})->DWord;
            System::Object^ result = nullptr;

            if (nativeObject->HandleInvokeMember_Internal("get_Count", gcnew array<System::Object^>{}, result)) {
                return (int)result;
            }

            if (nativeObject->HandleInvokeMember_Internal("get_Length", gcnew array<System::Object^>{}, result)) {
                return (int)result;
            }

            return 0;
        }
    };

private:
    void* m_object{};
    TypeDefinition^ m_type{};
};
}