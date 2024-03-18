#pragma once

#include <reframework/API.hpp>

#pragma managed

#include "InvokeRet.hpp"
#include "MethodParameter.hpp"

namespace REFrameworkNET {
public ref class Method {
public:
    Method(reframework::API::Method* method) : m_method(method) {}

    REFrameworkNET::InvokeRet^ Invoke(System::Object^ obj, array<System::Object^>^ args);

    /*Void* GetFunctionRaw() {
        return m_method->get_function_raw();
    }*/

    System::String^ GetName() {
        return gcnew System::String(m_method->get_name());
    }

    TypeDefinition^ GetDeclaringType() {
        auto result = m_method->get_declaring_type();

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew TypeDefinition(result);
    }

    TypeDefinition^ GetReturnType() {
        auto result = m_method->get_return_type();

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew TypeDefinition(result);
    }

    uint32_t GetNumParams() {
        return m_method->get_num_params();
    }

    System::Collections::Generic::List<REFrameworkNET::MethodParameter^>^ GetParameters() {
        const auto params = m_method->get_params();

        auto ret = gcnew System::Collections::Generic::List<REFrameworkNET::MethodParameter^>();

        for (auto& p : params) {
            ret->Add(gcnew REFrameworkNET::MethodParameter(p));
        }

        return ret;
    }

    uint32_t GetIndex() {
        return m_method->get_index();
    }

    int32_t GetVirtualIndex() {
        return m_method->get_virtual_index();
    }

    bool IsStatic() {
        return m_method->is_static();
    }

    uint16_t GetFlags() {
        return m_method->get_flags();
    }

    uint16_t GetImplFlags() {
        return m_method->get_impl_flags();
    }

    uint32_t GetInvokeID() {
        return m_method->get_invoke_id();
    }

    // hmm...
    /*UInt32 AddHook(pre_fn, post_fn, Boolean ignore_jmp) {
        return m_method->add_hook(pre_fn, post_fn, ignore_jmp);
    }*/

    void RemoveHook(uint32_t hook_id) {
        m_method->remove_hook(hook_id);
    }

private:
    reframework::API::Method* m_method;
};
}