#pragma once

#include <reframework/API.hpp>

#pragma managed

#include "InvokeRet.hpp"
#include "MethodParameter.hpp"

namespace REFrameworkNET {
ref class MethodHook;

public enum class PreHookResult : int32_t {
    Continue = 0,
    Skip = 1,
};

public ref class Method : public System::IEquatable<Method^>
{
public:
    Method(reframework::API::Method* method) : m_method(method) {}

    void* GetRaw() {
        return m_method;
    }

    REFrameworkNET::InvokeRet^ Invoke(System::Object^ obj, array<System::Object^>^ args);
    bool HandleInvokeMember_Internal(System::Object^ obj, System::String^ methodName, array<System::Object^>^ args, System::Object^% result);

    void* GetFunctionPtr() {
        return m_method->get_function_raw();
    }

    System::String^ GetName() {
        return gcnew System::String(m_method->get_name());
    }

    property System::String^ Name {
        System::String^ get() {
            return GetName();
        }
    }

    TypeDefinition^ GetDeclaringType() {
        auto result = m_method->get_declaring_type();

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew TypeDefinition(result);
    }

    property TypeDefinition^ DeclaringType {
        TypeDefinition^ get() {
            return GetDeclaringType();
        }
    }

    TypeDefinition^ GetReturnType() {
        auto result = m_method->get_return_type();

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew TypeDefinition(result);
    }

    property TypeDefinition^ ReturnType {
        TypeDefinition^ get() {
            return GetReturnType();
        }
    }

    uint32_t GetNumParams() {
        return m_method->get_num_params();
    }

    property uint32_t NumParams {
        uint32_t get() {
            return GetNumParams();
        }
    }

    System::Collections::Generic::List<REFrameworkNET::MethodParameter^>^ GetParameters() {
        const auto params = m_method->get_params();

        auto ret = gcnew System::Collections::Generic::List<REFrameworkNET::MethodParameter^>();

        for (auto& p : params) {
            ret->Add(gcnew REFrameworkNET::MethodParameter(p));
        }

        return ret;
    }

    property System::Collections::Generic::List<REFrameworkNET::MethodParameter^>^ Parameters {
        System::Collections::Generic::List<REFrameworkNET::MethodParameter^>^ get() {
            return GetParameters();
        }
    }

    uint32_t GetIndex() {
        return m_method->get_index();
    }

    property uint32_t Index {
        uint32_t get() {
            return GetIndex();
        }
    }

    int32_t GetVirtualIndex() {
        return m_method->get_virtual_index();
    }

    property int32_t VirtualIndex {
        int32_t get() {
            return GetVirtualIndex();
        }
    }

    bool IsStatic() {
        return m_method->is_static();
    }

    uint16_t GetFlags() {
        return m_method->get_flags();
    }

    property uint16_t Flags {
        uint16_t get() {
            return GetFlags();
        }
    }

    uint16_t GetImplFlags() {
        return m_method->get_impl_flags();
    }

    property uint16_t ImplFlags {
        uint16_t get() {
            return GetImplFlags();
        }
    }

    uint32_t GetInvokeID() {
        return m_method->get_invoke_id();
    }

    property uint32_t InvokeID {
        uint32_t get() {
            return GetInvokeID();
        }
    }

    // More palatable C# versions
    delegate PreHookResult REFPreHookDelegate(System::Collections::Generic::List<System::Object^>^ args);
    delegate void REFPostHookDelegate();

    MethodHook^ AddHook(bool ignore_jmp);

public:
    virtual bool Equals(System::Object^ other) override {
        if (System::Object::ReferenceEquals(this, other)) {
            return true;
        }

        if (System::Object::ReferenceEquals(other, nullptr)) {
            return false;
        }

        if (other->GetType() != Method::typeid) {
            return false;
        }

        return m_method == safe_cast<Method^>(other)->m_method;
    }

    virtual bool Equals(Method^ other) {
        if (System::Object::ReferenceEquals(this, other)) {
            return true;
        }

        if (System::Object::ReferenceEquals(other, nullptr)) {
            return false;
        }

        return m_method == other->m_method;
    }

    static bool operator ==(Method^ left, Method^ right) {
        if (System::Object::ReferenceEquals(left, right)) {
            return true;
        }

        if (System::Object::ReferenceEquals(left, nullptr) || System::Object::ReferenceEquals(right, nullptr)) {
            return false;
        }

        return left->m_method == right->m_method;
    }

    static bool operator !=(Method^ left, Method^ right) {
        return !(left == right);
    }

    virtual int GetHashCode() override {
        return (gcnew System::UIntPtr((uintptr_t)m_method))->GetHashCode();
    }

private:
    reframework::API::Method* m_method;
};
}