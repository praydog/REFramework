using System;
using reframework;

public class TypeDefinitionWrapper
{
    private readonly reframework.API.TypeDefinition _original;

    public TypeDefinitionWrapper(API.TypeDefinition original)
    {
        _original = original;
    }


    public UInt32 GetIndex()
    {
        return _original.get_index();
    }

    public UInt32 GetSize()
    {
        return _original.get_size();
    }

    public UInt32 GetValuetypeSize()
    {
        return _original.get_valuetype_size();
    }

    public UInt32 GetFqn()
    {
        return _original.get_fqn();
    }

    public String GetName()
    {
        return _original.get_name();
    }

    public String GetNamespace()
    {
        return _original.get_namespace();
    }

    public String GetFullName()
    {
        return _original.get_full_name();
    }

    public Boolean HasFieldptrOffset()
    {
        return _original.has_fieldptr_offset();
    }

    public Int32 GetFieldptrOffset()
    {
        return _original.get_fieldptr_offset();
    }

    public UInt32 GetNumMethods()
    {
        return _original.get_num_methods();
    }

    public UInt32 GetNumFields()
    {
        return _original.get_num_fields();
    }

    public UInt32 GetNumProperties()
    {
        return _original.get_num_properties();
    }

    public Boolean IsDerivedFrom(String other)
    {
        return _original.is_derived_from(other);
    }

    public Boolean IsDerivedFrom(reframework.API.TypeDefinition other)
    {
        return _original.is_derived_from(other);
    }

    public Boolean IsDerivedFrom(TypeDefinitionWrapper other) {
        return _original.is_derived_from(other._original);
    }

    public Boolean IsValuetype()
    {
        return _original.is_valuetype();
    }

    public Boolean IsEnum()
    {
        return _original.is_enum();
    }

    public Boolean IsByRef()
    {
        return _original.is_by_ref();
    }

    public Boolean IsPointer()
    {
        return _original.is_pointer();
    }

    public Boolean IsPrimitive()
    {
        return _original.is_primitive();
    }

    public UInt32 GetVmObjType()
    {
        return _original.get_vm_obj_type();
    }

    public API.Method FindMethod(String name)
    {
        return _original.find_method(name);
    }

    public API.Field FindField(String name)
    {
        return _original.find_field(name);
    }

    public API.Property FindProperty(String name)
    {
        return _original.find_property(name);
    }

    public List<API.Method> GetMethods()
    {
        return _original.get_methods();
    }

    public List<API.Field> GetFields()
    {
        return _original.get_fields();
    }

    public List<API.Property> GetProperties()
    {
        return _original.get_properties();
    }

    /*public Void* GetInstance()
    {
        return _original.get_instance();
    }

    public Void* CreateInstanceDeprecated()
    {
        return _original.create_instance_deprecated();
    }*/

    public API.ManagedObject CreateInstance(Int32 flags)
    {
        return _original.create_instance(flags);
    }

    public TypeDefinitionWrapper GetParentType()
    {
        return new TypeDefinitionWrapper(_original.get_parent_type());
    }

    public TypeDefinitionWrapper GetDeclaringType()
    {
        return new TypeDefinitionWrapper(_original.get_declaring_type());
    }

    public TypeDefinitionWrapper GetUnderlyingType()
    {
        return new TypeDefinitionWrapper(_original.get_underlying_type());
    }

    public API.TypeInfo GetTypeInfo()
    {
        return _original.get_type_info();
    }

    public API.ManagedObject GetRuntimeType()
    {
        return _original.get_runtime_type();
    }
}
