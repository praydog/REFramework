using System;
using static reframework.API;
public class ManagedObjectWrapper {
    private readonly ManagedObject _original;

    public ManagedObjectWrapper(ManagedObject original) {
        _original = original;
        _original.add_ref();
    }

    ~ManagedObjectWrapper() {
        _original.release();
    }

    public void AddRef() {
        _original.add_ref();
    }

    public void Release() {
        _original.release();
    }

    public TypeDefinitionWrapper GetTypeDefinition() {
        return new TypeDefinitionWrapper(_original.get_type_definition());
    }

    public Boolean IsManagedObject() {
        return _original.is_managed_object();
    }

    public UInt32 GetRefCount() {
        return _original.get_ref_count();
    }

    public UInt32 GetVmObjType() {
        return _original.get_vm_obj_type();
    }

    public TypeInfo GetTypeInfo() {
        return _original.get_type_info();
    }

    /*public Void* GetReflectionProperties() {
        return _original.get_reflection_properties();
    }*/

    /*public ReflectionProperty GetReflectionPropertyDescriptor(basic_string_view<char\, std::char_traits<char>> name) {
        return _original.get_reflection_property_descriptor(name);
    }

    public ReflectionMethod GetReflectionMethodDescriptor(basic_string_view<char\, std::char_traits<char>> name) {
        return _original.get_reflection_method_descriptor(name);
    }*/

    public reframework.DotNetInvokeRet Invoke(String methodName, object[] args) {
        return _original.invoke(methodName, args);
    }
}
