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

        return gcnew TypeDefinition(t);
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

        return gcnew TypeDefinition(t);
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
    T GetDataT(uintptr_t obj, bool isValueType) {
        const auto raw_data = GetDataRaw(obj, isValueType);
        if (raw_data == 0) {
            return T();
        }
        
        if (this->Type->IsValueType()) {
            return (T)System::Runtime::InteropServices::Marshal::PtrToStructure(System::IntPtr((void*)raw_data), T::typeid);
        }

        throw gcnew System::NotImplementedException();
    }

private:
    const reframework::API::Field* m_field;
};
}