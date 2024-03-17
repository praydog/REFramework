using System;
using reframework;

public class APIWrapper
{
    private readonly reframework.API _original;

    public APIWrapper(API original)
    {
        _original = original;
    }

    public TDBWrapper GetTDB()
    {
        return new TDBWrapper(_original.tdb());
    }

    public API.ResourceManager GetResourceManager()
    {
        return _original.resource_manager();
    }

    public API.REFramework_ Get()
    {
        return _original.reframework();
    }

    public API.VMContext GetVMContext()
    {
        return _original.get_vm_context();
    }

    public API.ManagedObject TypeOf(String name)
    {
        return (API.ManagedObject)_original.GetType().InvokeMember("typeof", System.Reflection.BindingFlags.Public, null, _original, new object[]{ _original, name });
    }

    public API.ManagedObject GetManagedSingleton(String name)
    {
        return _original.get_managed_singleton(name);
    }

    public List<API.DotNetManagedSingleton> GetManagedSingletons() {
        return _original.get_managed_singletons();
    }

    public System.UIntPtr GetNativeSingleton(String name)
    {
        return (System.UIntPtr)_original.get_native_singleton(name);
    }
}
