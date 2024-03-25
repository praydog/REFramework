#pragma once

#include <cstdint>

#include "./API.hpp"
#include "./Method.hpp"

namespace REFrameworkNET {
// Wrapper class we create when we create an initial hook
// Additional hook calls on the same method will return the instance we already made
// and additional callbacks will be appended to the existing events
public ref class MethodHook
{
public:
    // Public factory method to create a new hook
    static MethodHook^ Create(Method^ method, bool ignore_jmp) 
    {
        if (s_hooked_methods->ContainsKey(method)) {
            MethodHook^ out = nullptr;
            s_hooked_methods->TryGetValue(method, out);

            return out;
        }

        auto wrapper = gcnew MethodHook(method, ignore_jmp);
        s_hooked_methods->Add(method, wrapper);
        return wrapper;
    }

    MethodHook^ AddPre(Method::REFPreHookDelegate^ callback) 
    {
        OnPreStart += callback;
        return this;
    }

    MethodHook^ AddPost(Method::REFPostHookDelegate^ callback) 
    {
        OnPostStart += callback;
        return this;
    }

private:
    event Method::REFPreHookDelegate^ OnPreStart;
    event Method::REFPostHookDelegate^ OnPostStart;


    // This is never meant to publicly be called
    MethodHook(Method^ method, bool ignore_jmp) 
    {
        m_method = method;
        m_preHookLambda = gcnew REFPreHookDelegateRaw(this, &MethodHook::OnPreStart_Raw);
        m_postHookLambda = gcnew REFPostHookDelegateRaw(this, &MethodHook::OnPostStart_Raw);
        InstallHooks(ignore_jmp);
    }

    ~MethodHook() 
    {
        if (m_hooks_installed) {
            UninstallHooks();
        }
    }

    static System::Collections::Generic::Dictionary<Method^, MethodHook^>^ s_hooked_methods = gcnew System::Collections::Generic::Dictionary<Method^, MethodHook^>();

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