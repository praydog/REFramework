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
    delegate PreHookResult PreHookDelegate(System::Span<uint64_t> args);
    delegate void PostHookDelegate(uint64_t% retval);

    //delegate PreHookResult PreHookEasyDelegate(REFrameworkNET::NativeObject^ thisPtr, System::Object^% arg1, System::Object^% arg2, System::Object^% arg3, System::Object^% arg4);

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

    MethodHook^ AddPre(PreHookDelegate^ callback) 
    {
        OnPreStart += callback;
        return this;
    }

    MethodHook^ AddPost(PostHookDelegate^ callback) 
    {
        OnPostStart += callback;
        return this;
    }

    static System::Collections::Generic::List<MethodHook^>^ GetAllHooks() {
        auto out = gcnew System::Collections::Generic::List<MethodHook^>();
        for each (auto kvp in s_hooked_methods) {
            out->Add(kvp.Value);
        }

        return out;
    }

internal:
    static void UnsubscribeAssembly(System::Reflection::Assembly^ assembly);

    event PreHookDelegate^ OnPreStart {
        void add(PreHookDelegate^ callback) {
            OnPreStartImpl = (PreHookDelegate^)System::Delegate::Combine(OnPreStartImpl, callback);
        }
        void remove(PreHookDelegate^ callback) {
            OnPreStartImpl = (PreHookDelegate^)System::Delegate::Remove(OnPreStartImpl, callback);
        }
        REFrameworkNET::PreHookResult raise(System::Span<uint64_t> args) {
            if (OnPreStartImpl != nullptr) {
                REFrameworkNET::PreHookResult result = REFrameworkNET::PreHookResult::Continue;

                for each (PreHookDelegate^ callback in OnPreStartImpl->GetInvocationList()) {
                    try {
                        auto currentResult = callback(args);
                        if (currentResult != REFrameworkNET::PreHookResult::Continue) {
                            result = currentResult;
                        }
                    } catch (System::Exception^ e) {
                        REFrameworkNET::API::LogError("Exception in pre-hook callback for method: " + m_method->GetName() + " - " + e->Message);
                    } catch (...) {
                        REFrameworkNET::API::LogError("Unknown exception in pre-hook callback for method: " + m_method->GetName());
                    }
                }

                return result;
            }

            return REFrameworkNET::PreHookResult::Continue;
        }
    };

    event PostHookDelegate^ OnPostStart {
        void add(PostHookDelegate^ callback) {
            OnPostStartImpl = (PostHookDelegate^)System::Delegate::Combine(OnPostStartImpl, callback);
        }
        void remove(PostHookDelegate^ callback) {
            OnPostStartImpl = (PostHookDelegate^)System::Delegate::Remove(OnPostStartImpl, callback);
        }
        void raise(uint64_t% retval) {
            if (OnPostStartImpl != nullptr) {
                try {
                    OnPostStartImpl(retval);
                } catch (System::Exception^ e) {
                    REFrameworkNET::API::LogError("Exception in post-hook callback for method: " + m_method->GetName() + " - " + e->Message);
                } catch (...) {
                    REFrameworkNET::API::LogError("Unknown exception in post-hook callback for method: " + m_method->GetName());
                }
            }
        }
    };

    PreHookDelegate^ OnPreStartImpl;
    PostHookDelegate^ OnPostStartImpl;


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
    
    // Cached info at install time
    bool m_is_static{false};
    System::Collections::Generic::List<REFrameworkNET::MethodParameter^>^ m_parameters{};

    REFPreHookDelegateRaw^ m_preHookLambda{};
    REFPostHookDelegateRaw^ m_postHookLambda{};
};
}