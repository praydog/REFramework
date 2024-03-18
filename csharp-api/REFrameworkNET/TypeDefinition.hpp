#pragma once

#include <reframework/API.hpp>

#pragma managed

#include <msclr/marshal_cppstd.h>

namespace REFrameworkNET {
ref class ManagedObject;
ref class Method;
ref class Field;
ref class Property;
ref class TypeInfo;

public ref class TypeDefinition
{
public:
    TypeDefinition(reframework::API::TypeDefinition* td) : m_type(td) {}
    TypeDefinition(::REFrameworkTypeDefinitionHandle handle) : m_type(reinterpret_cast<reframework::API::TypeDefinition*>(handle)) {}

    operator reframework::API::TypeDefinition*() {
        return (reframework::API::TypeDefinition*)m_type;
    }

    uint32_t GetIndex()
    {
        return m_type->get_index();
    }

    uint32_t GetSize()
    {
        return m_type->get_size();
    }

    uint32_t GetValuetypeSize()
    {
        return m_type->get_valuetype_size();
    }

    uint32_t GetFqn()
    {
        return m_type->get_fqn();
    }

    System::String^ GetName()
    {
        return gcnew System::String(m_type->get_name());
    }

    System::String^ GetNamespace()
    {
        return gcnew System::String(m_type->get_namespace());
    }

    System::String^ GetFullName()
    {
        return gcnew System::String(m_type->get_full_name().c_str());
    }

    bool HasFieldPtrOffset()
    {
        return m_type->has_fieldptr_offset();
    }

    int32_t GetFieldPtrOffset()
    {
        return m_type->get_fieldptr_offset();
    }

    uint32_t GetNumMethods()
    {
        return m_type->get_num_methods();
    }

    uint32_t GetNumFields()
    {
        return m_type->get_num_fields();
    }

    uint32_t GetNumProperties()
    {
        return m_type->get_num_properties();
    }

    bool IsDerivedFrom(System::String^ other)
    {
        return m_type->is_derived_from(msclr::interop::marshal_as<std::string>(other).c_str());
    }

    bool IsDerivedFrom(TypeDefinition^ other)
    {
        return m_type->is_derived_from(other);
    }

    bool IsValueType()
    {
        return m_type->is_valuetype();
    }

    bool IsEnum()
    {
        return m_type->is_enum();
    }

    bool IsByRef()
    {
        return m_type->is_by_ref();
    }

    bool IsPointer()
    {
        return m_type->is_pointer();
    }

    bool IsPrimitive()
    {
        return m_type->is_primitive();
    }

    uint32_t GetVmObjType()
    {
        return m_type->get_vm_obj_type();
    }

    REFrameworkNET::Method^ FindMethod(System::String^ name);
    Field^ FindField(System::String^ name);
    Property^ FindProperty(System::String^ name);

    System::Collections::Generic::List<Method^>^ GetMethods();
    System::Collections::Generic::List<Field^>^ GetFields();
    System::Collections::Generic::List<Property^>^ GetProperties();

    ManagedObject^ CreateInstance(int32_t flags);
    
    TypeDefinition^ GetParentType()
    {
        auto result = m_type->get_parent_type();

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew TypeDefinition(result);
    }

    TypeDefinition^ GetDeclaringType()
    {
        auto result = m_type->get_declaring_type();

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew TypeDefinition(result);
    }

    TypeDefinition^ GetUnderlyingType()
    {
        auto result = m_type->get_underlying_type();

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew TypeDefinition(result);
    }

    REFrameworkNET::TypeInfo^ GetTypeInfo();
    ManagedObject^ GetRuntimeType();

    /*Void* GetInstance()
    {
        return m_type->get_instance();
    }

    Void* CreateInstanceDeprecated()
    {
        return m_type->create_instance_deprecated();
    }*/

private:
    reframework::API::TypeDefinition* m_type;
};
}