#pragma once

#include <reframework/API.hpp>

#include "TypeDefinition.hpp"
#include "NativePool.hpp"

#pragma managed

namespace REFrameworkNET {
ref class TypeDefinition;

public ref class Field : public System::IEquatable<Field^> 
{
public:
    static Field^ GetInstance(reframework::API::Field* fd) {
        return NativePool<Field>::GetInstance((uintptr_t)fd, s_createFromPointer);
    }

    static Field^ GetInstance(::REFrameworkFieldHandle handle) {
        return NativePool<Field>::GetInstance((uintptr_t)handle, s_createFromPointer);
    }

private:
    static Field^ createFromPointer(uintptr_t ptr) {
        return gcnew Field((reframework::API::Field*)ptr);
    }

    static NativePool<Field>::CreatorDelegate^ s_createFromPointer = gcnew NativePool<Field>::CreatorDelegate(createFromPointer);


private:
    Field(const reframework::API::Field* field) : m_field(field) {}
    Field(::REFrameworkFieldHandle handle) : m_field(reinterpret_cast<const reframework::API::Field*>(handle)) {}

public:
    System::String^ GetName() {
        return gcnew System::String(m_field->get_name());
    }

    property System::String^ Name {
        System::String^ get() {
            return GetName();
        }
    }

    TypeDefinition^ GetDeclaringType() {
        auto t = m_field->get_declaring_type();

        if (t == nullptr) {
            return nullptr;
        }

        return TypeDefinition::GetInstance(t);
    }

    property TypeDefinition^ DeclaringType {
        TypeDefinition^ get() {
            return GetDeclaringType();
        }
    }

    TypeDefinition^ GetType() {
        auto t = m_field->get_type();

        if (t == nullptr) {
            return nullptr;
        }

        return TypeDefinition::GetInstance(t);
    }

    property TypeDefinition^ Type {
        TypeDefinition^ get() {
            return GetType();
        }
    }

    uint32_t GetOffsetFromBase() {
        return m_field->get_offset_from_base();
    }

    property uint32_t OffsetFromBase {
        uint32_t get() {
            return GetOffsetFromBase();
        }
    }

    uint32_t GetOffsetFromFieldPtr() {
        return m_field->get_offset_from_fieldptr();
    }

    property uint32_t OffsetFromFieldPtr {
        uint32_t get() {
            return GetOffsetFromFieldPtr();
        }
    }

    uint32_t GetFlags() {
        return m_field->get_flags();
    }

    property uint32_t Flags {
        uint32_t get() {
            return GetFlags();
        }
    }

    bool IsStatic() {
        return m_field->is_static();
    }

    bool IsLiteral() {
        return m_field->is_literal();
    }

    uintptr_t GetInitDataPtr() {
        return (uintptr_t)m_field->get_init_data();
    }

    uintptr_t GetDataRaw(uintptr_t obj, bool isValueType) {
        return (uintptr_t)m_field->get_data_raw((void*)obj, isValueType);
    }

    // I have no idea if this will work correctly
    template<typename T>
    T& GetData(uintptr_t obj, bool isValueType) {
        return m_field->get_data<T>((void*)obj, isValueType);
    }

    // For .NET
    generic <typename T>
    T GetDataT(uintptr_t obj, bool isValueType);
    
    /// <summary>
    /// Get the field data as a boxed object
    /// </summary>
    /// <param name="obj">The object to get the field data from</param>
    /// <param name="isValueType">Whether the object holding the field is a value type</param>
    /// <returns>The field data as a boxed object</returns>
    System::Object^ GetDataBoxed(uintptr_t obj, bool isValueType);

    /// <summary>
    /// Get the field data as a boxed object, but proxies the object to the target type
    /// </summary>
    /// <param name="targetReturnType">The target type to proxy the object to.</param>
    /// <param name="obj">The object to get the field data from</param>
    /// <param name="isValueType">Whether the object holding the field is a value type</param>
    /// <returns>The field data as a boxed object</returns>
    System::Object^ GetDataBoxed(System::Type^ targetReturnType, uintptr_t obj, bool isValueType);

    /// <summary>
    /// Set the field data from a boxed object
    /// </summary>
    /// <param name="obj">The object to set the field data on</param>
    /// <param name="value">The value to set the field data to</param>
    /// <param name="isValueType">Whether the object holding the field is a value type</param>
    void SetDataBoxed(uintptr_t obj, System::Object^ value, bool isValueType);

    uint32_t GetIndex() {
        return m_field->get_index();
    }

    property uint32_t Index {
    public:
        uint32_t get() {
            return GetIndex();
        }
    }

public:
    virtual bool Equals(System::Object^ other) override {
        if (System::Object::ReferenceEquals(this, other)) {
            return true;
        }

        if (System::Object::ReferenceEquals(other, nullptr)) {
            return false;
        }

        if (other->GetType() != Field::typeid) {
            return false;
        }

        return m_field == safe_cast<Field^>(other)->m_field;
    }

    virtual bool Equals(Field^ other) {
        if (System::Object::ReferenceEquals(this, other)) {
            return true;
        }

        if (System::Object::ReferenceEquals(other, nullptr)) {
            return false;
        }

        return m_field == other->m_field;
    }

    static bool operator ==(Field^ left, Field^ right) {
        if (System::Object::ReferenceEquals(left, right)) {
            return true;
        }

        if (System::Object::ReferenceEquals(left, nullptr) || System::Object::ReferenceEquals(right, nullptr)) {
            return false;
        }

        return left->m_field == right->m_field;
    }

    static bool operator !=(Field^ left, Field^ right) {
        return !(left == right);
    }

    virtual int GetHashCode() override {
        return (gcnew System::UIntPtr((uintptr_t)m_field))->GetHashCode();
    }

private:
    const reframework::API::Field* m_field;
};
}