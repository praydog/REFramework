#pragma once

#include <reframework/API.hpp>

#pragma managed

#include <msclr/marshal_cppstd.h>
#include "TypeDefinition.hpp"
#include "Method.hpp"
#include "Field.hpp"
#include "Property.hpp"

namespace REFrameworkNET {
public ref class TDB {
public:
    TDB(reframework::API::TDB* tdb) : m_tdb(tdb) {}

    uintptr_t GetAddress() {
        return (uintptr_t)m_tdb;
    }

    uint32_t GetNumTypes() {
        return m_tdb->get_num_types();
    }

    uint32_t GetNumMethods() {
        return m_tdb->get_num_methods();
    }

    uint32_t GetNumFields() {
        return m_tdb->get_num_fields();
    }

    uint32_t GetNumProperties() {
        return m_tdb->get_num_properties();
    }

    uint32_t GetStringsSize() {
        return m_tdb->get_strings_size();
    }

    uint32_t GetRawDataSize() {
        return m_tdb->get_raw_data_size();
    }

    /*public SByte* GetStringDatabase() {
        return m_tdb->get_string_database();
    }

    public Byte* GetRawDatabase() {
        return m_tdb->get_raw_database();
    }*/

    System::Span<uint8_t>^ GetRawData() {
        return gcnew System::Span<uint8_t>(m_tdb->get_raw_database(), m_tdb->get_raw_data_size());
    }

    System::String^ GetString(uint32_t index) {
        if (index >= m_tdb->get_strings_size()) {
            return nullptr;
        }

        return gcnew System::String(m_tdb->get_string_database() + index);
    }

    TypeDefinition^ GetType(uint32_t index) {
        auto result = m_tdb->get_type(index);

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew TypeDefinition(result);
    }

    TypeDefinition^ FindType(System::String^ name) {
        auto result = m_tdb->find_type(msclr::interop::marshal_as<std::string>(name));

        if (result == nullptr) {
            return nullptr;
        }
        
        return gcnew TypeDefinition(result);
    }

    TypeDefinition^ GetType(System::String^ name) {
        return FindType(name);
    }

    TypeDefinition^ FindTypeByFqn(uint32_t fqn) {
        auto result = m_tdb->find_type_by_fqn(fqn);

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew TypeDefinition(result);
    }

    Method^ GetMethod(uint32_t index) {
        return gcnew Method(m_tdb->get_method(index));
    }

    Method^ FindMethod(System::String^ type_name, System::String^ name) {
        auto result = m_tdb->find_method(msclr::interop::marshal_as<std::string>(type_name), msclr::interop::marshal_as<std::string>(name));

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew Method(result);
    }

    Method^ GetMethod(System::String^ type_name, System::String^ name) {
        return FindMethod(type_name, name);
    }

    Field^ GetField(uint32_t index) {
        auto result = m_tdb->get_field(index);

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew Field(result);
    }

    Field^ FindField(System::String^ type_name, System::String^ name) {
        auto result = m_tdb->find_field(msclr::interop::marshal_as<std::string>(type_name), msclr::interop::marshal_as<std::string>(name));

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew Field(result);
    }

    Field^ GetField(System::String^ type_name, System::String^ name) {
        return FindField(type_name, name);
    }

    Property^ GetProperty(uint32_t index) {
        auto result = m_tdb->get_property(index);

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew Property(result);
    }

private:
    reframework::API::TDB* m_tdb;
};
}