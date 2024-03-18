using reframework;
using System;
using static reframework.API;
public class MethodWrapper {
    private readonly Method _original;

    public MethodWrapper(Method original) {
        _original = original;
    }

    public DotNetInvokeRet Invoke(Object obj, Object[] args) {
        return _original.invoke(obj, args);
    }

    /*public Void* GetFunctionRaw() {
        return _original.get_function_raw();
    }*/

    public string GetName() {
        return _original.get_name();
    }

    public TypeDefinitionWrapper GetDeclaringType() {
        return new TypeDefinitionWrapper(_original.get_declaring_type());
    }

    public TypeDefinitionWrapper GetReturnType() {
        return new TypeDefinitionWrapper(_original.get_return_type());
    }

    public UInt32 GetNumParams() {
        return _original.get_num_params();
    }

    public List<Method.DotNetMethodParameter> GetParameters() {
        return _original.get_params();
    }

    public UInt32 GetIndex() {
        return _original.get_index();
    }

    public Int32 GetVirtualIndex() {
        return _original.get_virtual_index();
    }

    public Boolean IsStatic() {
        return _original.is_static();
    }

    public UInt16 GetFlags() {
        return _original.get_flags();
    }

    public UInt16 GetImplFlags() {
        return _original.get_impl_flags();
    }

    public UInt32 GetInvokeID() {
        return _original.get_invoke_id();
    }

    // hmm...
    /*public UInt32 AddHook(pre_fn, post_fn, Boolean ignore_jmp) {
        return _original.add_hook(pre_fn, post_fn, ignore_jmp);
    }*/

    public void RemoveHook(UInt32 hook_id) {
        _original.remove_hook(hook_id);
    }
}
