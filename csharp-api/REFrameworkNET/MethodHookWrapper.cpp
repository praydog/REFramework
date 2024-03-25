#include "./API.hpp"
#include "MethodHookWrapper.hpp"

using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;

namespace REFrameworkNET {
    void MethodHookWrapper::InstallHooks(bool ignore_jmp) 
    {
        if (m_hooks_installed) {
            return;
        }

        REFrameworkNET::API::LogInfo("Creating .NET hook for method: " + m_method->GetName());

        m_hooks_installed = true;

        reframework::API::Method* raw = (reframework::API::Method*)m_method->GetRaw();

        IntPtr preHookPtr = Marshal::GetFunctionPointerForDelegate(m_preHookLambda);
        IntPtr postHookPtr = Marshal::GetFunctionPointerForDelegate(m_postHookLambda);
        m_hook_id = raw->add_hook((REFPreHookFn)preHookPtr.ToPointer(), (REFPostHookFn)postHookPtr.ToPointer(), ignore_jmp);
    }

    void MethodHookWrapper::UninstallHooks() {
        if (!m_hooks_installed) {
            return;
        }

        reframework::API::Method* raw = (reframework::API::Method*)m_method->GetRaw();

        m_hooks_installed = false;
        raw->remove_hook(m_hook_id);
    }

    int32_t MethodHookWrapper::OnPreStart_Raw(int argc, void** argv, REFrameworkTypeDefinitionHandle* arg_tys, unsigned long long ret_addr) 
    {
        OnPreStart(gcnew List<System::Object^>()); // todo: pass the arguments
        System::Console::WriteLine("Hello from" + m_method->GetName() + " pre-hook!");
        return 0; // Or another appropriate value
    }

    void MethodHookWrapper::OnPostStart_Raw(void** ret_val, REFrameworkTypeDefinitionHandle ret_ty, unsigned long long ret_addr) {
        //OnPostStart(/* arguments */);
        System::Console::WriteLine("Hello from" + m_method->GetName() + " post-hook!");
    }
}