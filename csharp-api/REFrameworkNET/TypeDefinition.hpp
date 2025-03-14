#pragma once

#include <reframework/API.hpp>

#pragma managed

#include <msclr/marshal_cppstd.h>
#include "IObject.hpp"
#include "NativePool.hpp"

namespace REFrameworkNET {
ref class ManagedObject;
ref class NativeObject;
ref class Method;
ref class Field;
ref class Property;
ref class TypeInfo;
ref class ValueType;
value struct InvokeRet;

/// <summary>
/// A shorthand enum for determining how a <see cref="TypeDefinition"/> is used in the VM.
/// </summary>
public enum class VMObjType : uint32_t {
    NULL_ = 0,
    Object = 1,
    Array = 2,
    String = 3,
    Delegate = 4,
    ValType = 5,
};


/// <summary>
/// Represents a type in the RE Engine's IL2CPP metadata.
/// Equivalent to System.RuntimeTypeHandle in .NET.
/// </summary>
/// <remarks>
/// </remarks>
public ref class TypeDefinition : public System::Dynamic::DynamicObject,
                               public System::IEquatable<TypeDefinition ^>
{
public:
    static TypeDefinition^ GetInstance(reframework::API::TypeDefinition* td) {
        return NativePool<TypeDefinition>::GetInstance((uintptr_t)td, s_createFromPointer);
    }

    static TypeDefinition^ GetInstance(::REFrameworkTypeDefinitionHandle handle) {
        return NativePool<TypeDefinition>::GetInstance((uintptr_t)handle, s_createFromPointer);
    }

private:
    static TypeDefinition^ createFromPointer(uintptr_t ptr) {
        return gcnew TypeDefinition((reframework::API::TypeDefinition*)ptr);
    }

    static NativePool<TypeDefinition>::CreatorDelegate^ s_createFromPointer = gcnew NativePool<TypeDefinition>::CreatorDelegate(createFromPointer);

    TypeDefinition(reframework::API::TypeDefinition* td) : m_type(td) {}
    TypeDefinition(::REFrameworkTypeDefinitionHandle handle) : m_type(reinterpret_cast<reframework::API::TypeDefinition*>(handle)) {}

public:
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

public:
    System::String^ GetFullName() {
        if (m_cachedFullName == nullptr) {
            m_lock->EnterWriteLock();

            try {
                if (m_cachedFullName == nullptr) {
                    m_cachedFullName = gcnew System::String(m_type->get_full_name().c_str());
                }
            } finally {
                m_lock->ExitWriteLock();
            }
        }

        return m_cachedFullName;
    }

    property System::String^ FullName {
        System::String^ get() {
            return GetFullName();
        }
    }

public:
    size_t GetFNV64Hash();

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
        if (!m_cachedIsValueType.HasValue) {
            m_lock->EnterWriteLock();

            try {
                if (!m_cachedIsValueType.HasValue) {
                    m_cachedIsValueType = m_type->is_valuetype();
                }
            } finally {
                m_lock->ExitWriteLock();
            }
        }

        return m_cachedIsValueType.Value;
    }

    bool IsEnum()
    {
        if (!m_cachedIsEnum.HasValue) {
            m_lock->EnterWriteLock();

            try {
                if (!m_cachedIsEnum.HasValue) {
                    m_cachedIsEnum = m_type->is_enum();
                }
            } finally {
                m_lock->ExitWriteLock();
            }
        }

        return m_cachedIsEnum.Value;
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

    /// <summary>
    /// <para>
    /// THIS IS IMPORTANT! Please refer to <see cref="ManagedObject::Globalize"/>
    /// Objects returned from CreateInstance are NOT globalized
    /// meaning you cannot assign them to a global variable, static field, or some other long term storage
    /// because they will be quickly destroyed by the Garbage Collector.
    /// You need to manually call ManagedObject::Globalize if you intend to keep the object around
    /// </para>
    /// </summary>
    /// <param name="flags">The flags to use when creating the instance.</param>
    /// <returns>A new <see cref="ManagedObject"/> instance of type <see cref="TypeDefinition"/>.</returns>
    ManagedObject^ CreateInstance(int32_t flags);
    REFrameworkNET::ValueType^ CreateValueType();
    
    TypeDefinition^ GetParentType()
    {
        auto result = m_type->get_parent_type();

        if (result == nullptr) {
            return nullptr;
        }

        return TypeDefinition::GetInstance(result);
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

        return TypeDefinition::GetInstance(result);
    }

    property TypeDefinition^ DeclaringType {
        TypeDefinition^ get() {
            return GetDeclaringType();
        }
    }

    /// <returns>The underlying type of this <see cref="TypeDefinition"/>.</returns>
    /// <remarks>Usually used for enums.</remarks>
    TypeDefinition^ GetUnderlyingType()
    {
        if (m_underlyingType == nullptr) {
            m_lock->EnterWriteLock();

            try {
                auto result = m_type->get_underlying_type();

                if (result != nullptr) {
                    m_underlyingType = TypeDefinition::GetInstance(result);
                }
            } finally {
                m_lock->ExitWriteLock();
            }
        }

        return m_underlyingType;
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

    /// <returns>The System.Type for this <see cref="TypeDefinition"/>.</returns>
    ManagedObject^ GetRuntimeType();
    property ManagedObject^ RuntimeType {
        ManagedObject^ get() {
            return GetRuntimeType();
        }
    }

    bool HasAttribute(REFrameworkNET::ManagedObject^ runtimeAttribute, bool inherit);
    bool IsGenericTypeDefinition();
    bool IsGenericType();
    array<TypeDefinition^>^ GetGenericArguments();

    property array<TypeDefinition^>^ GenericArguments {
        array<TypeDefinition^>^ get() {
            return GetGenericArguments();
        }
    }

    TypeDefinition^ GetElementType();

    property TypeDefinition^ ElementType {
        TypeDefinition^ get() {
            return GetElementType();
        }
    }

    System::Collections::Generic::List<REFrameworkNET::ManagedObject^>^ GetRuntimeMethods();

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

    /// <summary>
    /// Invokes a <see cref="Method"/> with the given arguments.
    /// </summary>
    /// <param name="obj">The object to invoke the method on. Null for static methods.</param>
    /// <param name="args">The arguments to pass to the method.</param>
    /// <returns>The return value of the method. 128 bytes in size internally.</returns>
    /// <remarks>
    /// Generally should not be used unless you know what you're doing.
    /// Use the other invoke method to automatically convert the return value correctly into a usable object.
    /// </remarks>
    virtual REFrameworkNET::InvokeRet^ Invoke(System::String^ methodName, array<System::Object^>^ args);

    /// <summary>
    /// Invokes a <see cref="Method"/> with the given arguments.
    /// </summary>
    /// <param name="obj">The object to invoke the method on. Null for static methods.</param>
    /// <param name="args">The arguments to pass to the method.</param>
    /// <param name="result">The return value of the method. REFramework will attempt to convert this into a usable object.</param>
    /// <returns>True if the method was successfully found and invoked, false otherwise.</returns>
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
    System::Threading::ReaderWriterLockSlim^ m_lock{gcnew System::Threading::ReaderWriterLockSlim(System::Threading::LockRecursionPolicy::SupportsRecursion)};

    // Cached values
    ManagedObject^ m_runtimeType{nullptr};
    TypeDefinition^ m_underlyingType{nullptr};
    TypeDefinition^ m_elementType{nullptr};
    System::String^ m_cachedFullName{nullptr};
    size_t m_cachedFNV64Hash{0};
    
    System::Nullable<bool> m_cachedIsValueType{};
    System::Nullable<bool> m_cachedIsEnum{};

    System::Collections::Generic::List<REFrameworkNET::Method^>^ m_methods{nullptr};
    System::Collections::Generic::List<REFrameworkNET::Field^>^ m_fields{nullptr};
    System::Collections::Generic::List<REFrameworkNET::Property^>^ m_properties{nullptr};

    System::Collections::Generic::List<REFrameworkNET::ManagedObject^>^ m_runtimeMethods{nullptr};
};
}