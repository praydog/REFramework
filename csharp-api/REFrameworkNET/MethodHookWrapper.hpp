#pragma once

#include <cstdint>

#include "./API.hpp"
#include "./Method.hpp"

namespace REFrameworkNET {
// Wrapper class we create when we create an initial hook
// Additional hook calls on the same method will return the instance we already made
// and additional callbacks will be appended to the existing events
public ref class MethodHookWrapper
{
public:
    // Public factory method to create a new hook
    static MethodHookWrapper^ Create(Method^ method, bool ignore_jmp) 
    {
        if (s_hooked_methods->ContainsKey(method)) {
            return s_hooked_methods[method];
        }

        auto wrapper = gcnew MethodHookWrapper(method, ignore_jmp);
        s_hooked_methods->Add(method, wrapper);
        return wrapper;
    }

    MethodHookWrapper^ AddPre(Method::REFPreHookDelegate^ callback) 
    {
        OnPreStart += callback;
        return this;
    }

    MethodHookWrapper^ AddPost(Method::REFPostHookDelegate^ callback) 
    {
        OnPostStart += callback;
        return this;
    }

private:
    event Method::REFPreHookDelegate^ OnPreStart;
    event Method::REFPostHookDelegate^ OnPostStart;


    // This is never meant to publicly be called
    MethodHookWrapper(Method^ method, bool ignore_jmp) 
    {
        m_method = method;
        m_preHookLambda = gcnew REFPreHookDelegateRaw(this, &MethodHookWrapper::OnPreStart_Raw);
        m_postHookLambda = gcnew REFPostHookDelegateRaw(this, &MethodHookWrapper::OnPostStart_Raw);
        InstallHooks(ignore_jmp);
    }

    ~MethodHookWrapper() 
    {
        if (m_hooks_installed) {
            UninstallHooks();
        }
    }

    static System::Collections::Generic::Dictionary<Method^, MethodHookWrapper^>^ s_hooked_methods = gcnew System::Collections::Generic::Dictionary<Method^, MethodHookWrapper^>();

    delegate int32_t REFPreHookDelegateRaw(int argc, void** argv, REFrameworkTypeDefinitionHandle* arg_tys, unsigned long long ret_addr);
    delegate void REFPostHookDelegateRaw(void** ret_val, REFrameworkTypeDefinitionHandle ret_ty, unsigned long long ret_addr);

    void InstallHooks(bool ignore_jmp);
    void UninstallHooks();

    int32_t OnPreStart_Raw(int argc, void** argv, REFrameworkTypeDefinitionHandle* arg_tys, unsigned long long ret_addr);
    void OnPostStart_Raw(void** ret_val, REFrameworkTypeDefinitionHandle ret_ty, unsigned long long ret_addr);

    Method^ m_method{};
    uint32_t m_hook_id{};
    bool m_hooks_installed{false};

    REFPreHookDelegateRaw^ m_preHookLambda{};
    REFPostHookDelegateRaw^ m_postHookLambda{};
};
}