using System;
using reframework;

public class APIWrapper
{
    private readonly reframework.API _original;

    public APIWrapper(API original)
    {
        _original = original;
    }

    public API.REFrameworkPluginInitializeParam* Param()
    {
        return _original.param();
    }

    public API.REFrameworkSDKData* Sdk()
    {
        return _original.sdk();
    }

    public API.TDB Tdb()
    {
        return _original.tdb();
    }

    public API.ResourceManager ResourceManager()
    {
        return _original.resource_manager();
    }

    public API.REFramework_ Reframework()
    {
        return _original.reframework();
    }

    public API.VMContext GetVmContext()
    {
        return _original.get_vm_context();
    }

    public ManagedObject Typeof(String name)
    {
        return _original.Invoke("typeof", name);
    }

    public ManagedObject GetManagedSingleton(String name)
    {
        return _original.get_managed_singleton(name);
    }

    public Void* GetNativeSingleton(String name)
    {
        return _original.get_native_singleton(name);
    }
}
