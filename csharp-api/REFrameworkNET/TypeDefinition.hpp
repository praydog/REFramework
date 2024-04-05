#pragma once

#include <reframework/API.hpp>

#pragma managed

#include <msclr/marshal_cppstd.h>
#include "IObject.hpp"

namespace REFrameworkNET {
ref class ManagedObject;
ref class NativeObject;
ref class Method;
ref class Field;
ref class Property;
ref class TypeInfo;
ref struct InvokeRet;

public enum VMObjType {
    NULL_ = 0,
    Object = 1,
    Array = 2,
    String = 3,
    Delegate = 4,
    ValType = 5,
};

public
    ref class TypeDefinition : public System::Dynamic::DynamicObject,
                               public System::IEquatable<TypeDefinition ^>
{
public:
    TypeDefinition(reframework::API::TypeDefinition* td) : m_type(td) {}
    TypeDefinition(::REFrameworkTypeDefinitionHandle handle) : m_type(reinterpret_cast<reframework::API::TypeDefinition*>(handle)) {}

    operator reframework::API::TypeDefinition*() {
        return (reframework::API::TypeDefinition*)m_type;
    }

    property NativeObject^ Statics {
        NativeObject^ get();
    }

    TypeDefinition^ Clone() {
		return gcnew TypeDefinition(m_type);
	}


    uint32_t GetIndex()
    {
        return m_type->get_index();
    }

    property uint32_t Index {
        uint32_t get() {
            return GetIndex();
        }
    }

    uint32_t GetSize()
    {
        return m_type->get_size();
    }

    property uint32_t Size {
        uint32_t get() {
            return GetSize();
        }
    }

    uint32_t GetValueTypeSize()
    {
        return m_type->get_valuetype_size();
    }

    property uint32_t ValueTypeSize {
        uint32_t get() {
            return GetValueTypeSize();
        }
    }

    uint32_t GetFQN()
    {
        return m_type->get_fqn();
    }

    property uint32_t FQN {
        uint32_t get() {
            return GetFQN();
        }
    }

    System::String^ GetName()
    {
        if (m_type->get_name() == nullptr) {
            return nullptr;
        }

        return gcnew System::String(m_type->get_name());
    }

    property System::String^ Name {
        System::String^ get() {
            return GetName();
        }
    }

    System::String^ GetNamespace()
    {
        if (m_type->get_namespace() == nullptr) {
            return nullptr;
        }

        return gcnew System::String(m_type->get_namespace());
    }

    property System::String^ Namespace {
        System::String^ get() {
            return GetNamespace();
        }
    }

    System::String^ GetFullName()
    {
        return gcnew System::String(m_type->get_full_name().c_str());
    }

    property System::String^ FullName {
        System::String^ get() {
            return GetFullName();
        }
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

    VMObjType GetVMObjType()
    {
        return (VMObjType)m_type->get_vm_obj_type();
    }

    REFrameworkNET::Method^ FindMethod(System::String^ name);
    Field^ FindField(System::String^ name);
    Property^ FindProperty(System::String^ name);

    // Get versions
    // The find versions just line up with the Lua API, the Get versions look more like C#
    REFrameworkNET::Method^ GetMethod(System::String^ name) {
        return FindMethod(name);
    }

    Field^ GetField(System::String^ name) {
        return FindField(name);
    }

    Property^ GetProperty(System::String^ name) {
        return FindProperty(name);
    }

    System::Collections::Generic::List<Method^>^ GetMethods();
    System::Collections::Generic::List<Field^>^ GetFields();
    System::Collections::Generic::List<Property^>^ GetProperties();

    property System::Collections::Generic::List<Method^>^ Methods {
        System::Collections::Generic::List<Method^>^ get() {
            return GetMethods();
        }
    }

    property System::Collections::Generic::List<Field^>^ Fields {
        System::Collections::Generic::List<Field^>^ get() {
            return GetFields();
        }
    }

    property System::Collections::Generic::List<Property^>^ Properties {
        System::Collections::Generic::List<Property^>^ get() {
            return GetProperties();
        }
    }

    ManagedObject^ CreateInstance(int32_t flags);
    
    TypeDefinition^ GetParentType()
    {
        auto result = m_type->get_parent_type();

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew TypeDefinition(result);
    }

    property TypeDefinition^ ParentType {
        TypeDefinition^ get() {
            return GetParentType();
        }
    }

    TypeDefinition^ GetDeclaringType()
    {
        auto result = m_type->get_declaring_type();

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew TypeDefinition(result);
    }

    property TypeDefinition^ DeclaringType {
        TypeDefinition^ get() {
            return GetDeclaringType();
        }
    }

    TypeDefinition^ GetUnderlyingType()
    {
        auto result = m_type->get_underlying_type();

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew TypeDefinition(result);
    }

    property TypeDefinition^ UnderlyingType {
        TypeDefinition^ get() {
            return GetUnderlyingType();
        }
    }

    REFrameworkNET::TypeInfo^ GetTypeInfo();
    property REFrameworkNET::TypeInfo^ TypeInfo {
        REFrameworkNET::TypeInfo^ get() {
            return GetTypeInfo();
        }
    }

    ManagedObject^ GetRuntimeType();
    property ManagedObject^ RuntimeType {
        ManagedObject^ get() {
            return GetRuntimeType();
        }
    }

    bool HasAttribute(REFrameworkNET::ManagedObject^ runtimeAttribute, bool inherit);

    /*Void* GetInstance()
    {
        return m_type->get_instance();
    }

    Void* CreateInstanceDeprecated()
    {
        return m_type->create_instance_deprecated();
    }*/

// IObject methods
public:
    virtual void* Ptr() {
        return (void*)m_type;
    }

    virtual uintptr_t GetAddress() {
        return (uintptr_t)m_type;
    }

    virtual REFrameworkNET::InvokeRet^ Invoke(System::String^ methodName, array<System::Object^>^ args);
    virtual bool HandleInvokeMember_Internal(System::String^ methodName, array<System::Object^>^ args, System::Object^% result);

    generic <typename T>
    virtual T As();

// DynamicObject methods
public:
    virtual bool TryInvokeMember(System::Dynamic::InvokeMemberBinder^ binder, array<System::Object^>^ args, System::Object^% result) override;

// IEquatable methods
public:
    virtual bool Equals(System::Object^ other) override {
        if (System::Object::ReferenceEquals(this, other)) {
            return true;
        }

        if (System::Object::ReferenceEquals(other, nullptr)) {
            return false;
        }

        if (other->GetType() != TypeDefinition::typeid) {
            return false;
        }

        return m_type == safe_cast<TypeDefinition^>(other)->m_type;
    }

    virtual bool Equals(TypeDefinition^ other) {
        if (System::Object::ReferenceEquals(this, other)) {
            return true;
        }

        if (System::Object::ReferenceEquals(other, nullptr)) {
            return false;
        }

        return m_type == other->m_type;
    }

    static bool operator ==(TypeDefinition^ left, TypeDefinition^ right) {
        if (System::Object::ReferenceEquals(left, right)) {
            return true;
        }

        if (System::Object::ReferenceEquals(left, nullptr) || System::Object::ReferenceEquals(right, nullptr)) {
            return false;
        }

        return left->m_type == right->m_type;
    }

    static bool operator !=(TypeDefinition^ left, TypeDefinition^ right) {
        return !(left == right);
    }

    virtual int GetHashCode() override {
        return (gcnew System::UIntPtr((uintptr_t)m_type))->GetHashCode();
    }

private:
    reframework::API::TypeDefinition* m_type;
};
}