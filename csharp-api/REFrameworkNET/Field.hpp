#pragma once

#include <reframework/API.hpp>

#include "TypeDefinition.hpp"

#pragma managed

namespace REFrameworkNET {
ref class TypeDefinition;

public ref class Field {
public:
    Field(const reframework::API::Field* field) : m_field(field) {}
    Field(::REFrameworkFieldHandle handle) : m_field(reinterpret_cast<const reframework::API::Field*>(handle)) {}

    System::String^ GetName() {
        return gcnew System::String(m_field->get_name());
    }

    TypeDefinition^ GetDeclaringType() {
        auto t = m_field->get_declaring_type();

        if (t == nullptr) {
            return nullptr;
        }

        return gcnew TypeDefinition(t);
    }

    TypeDefinition^ GetType() {
        auto t = m_field->get_type();

        if (t == nullptr) {
            return nullptr;
        }

        return gcnew TypeDefinition(t);
    }

    uint32_t GetOffsetFromBase() {
        return m_field->get_offset_from_base();
    }

    uint32_t GetOffsetFromFieldPtr() {
        return m_field->get_offset_from_fieldptr();
    }

    uint32_t GetFlags() {
        return m_field->get_flags();
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

private:
    const reframework::API::Field* m_field;
};
}