#include "./API.hpp"
#include "MethodHook.hpp"

using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;

namespace REFrameworkNET {
    void MethodHook::InstallHooks(bool ignore_jmp) 
    {
        if (m_hooks_installed) {
            return;
        }

        REFrameworkNET::API::LogInfo("Creating .NET hook for method: " + m_method->GetName());

        m_hooks_installed = true;
        m_is_static = m_method->IsStatic();
        m_parameters = m_method->GetParameters();

        reframework::API::Method* raw = (reframework::API::Method*)m_method->GetRaw();

        IntPtr preHookPtr = Marshal::GetFunctionPointerForDelegate(m_preHookLambda);
        IntPtr postHookPtr = Marshal::GetFunctionPointerForDelegate(m_postHookLambda);
        m_hook_id = raw->add_hook((REFPreHookFn)preHookPtr.ToPointer(), (REFPostHookFn)postHookPtr.ToPointer(), ignore_jmp);
    }

    void MethodHook::UninstallHooks() {
        if (!m_hooks_installed) {
            return;
        }

        REFrameworkNET::API::LogInfo("Removing .NET hook for method: " + m_method->GetName());

        reframework::API::Method* raw = (reframework::API::Method*)m_method->GetRaw();

        m_hooks_installed = false;
        raw->remove_hook(m_hook_id);
    }

    int32_t MethodHook::OnPreStart_Raw(int argc, void** argv, REFrameworkTypeDefinitionHandle* arg_tys, unsigned long long ret_addr) 
    {
        // Create a System::Span<UInt64> for the arguments
        auto argspan = System::Span<uint64_t>((uint64_t*)argv, argc);

        OnPreStart(argspan);
        System::Console::WriteLine("Hello from" + m_method->GetName() + " pre-hook!");
        return 0; // Or another appropriate value
    }

    void MethodHook::OnPostStart_Raw(void** ret_val, REFrameworkTypeDefinitionHandle ret_ty, unsigned long long ret_addr) {
        //OnPostStart(/* arguments */);
        System::Console::WriteLine("Hello from" + m_method->GetName() + " post-hook!");
    }
}