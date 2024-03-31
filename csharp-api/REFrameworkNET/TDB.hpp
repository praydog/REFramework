#pragma once

#include <reframework/API.hpp>

#pragma managed

#include <msclr/marshal_cppstd.h>
#include "TypeDefinition.hpp"
#include "Method.hpp"
#include "Field.hpp"
#include "Property.hpp"

using namespace System;
using namespace System::Collections;

namespace REFrameworkNET {
public ref class TDB {
private:
    reframework::API::TDB* m_tdb;

public:
    generic <class T> where T : ref class
    ref class TypeCacher {
    public:
        static REFrameworkNET::TypeDefinition^ GetCachedType() {
            if (s_cachedType == nullptr) {
                return nullptr;
            }

            return gcnew REFrameworkNET::TypeDefinition(s_cachedType);
        }
        
    private:
        static reframework::API::TypeDefinition* s_cachedType = TDB::GetTypeDefinitionPtr<T>();
    };

public:
    TDB(reframework::API::TDB* tdb) : m_tdb(tdb) {}

    static TDB^ Get();

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

    /// <summary>
    /// Get a type by its name.
    /// </summary>
    /// <param name="name">The name of the type.</param>
    /// <returns>The type definition if found, otherwise null.</returns>
    /// <remarks>Not cached.</remarks>
    TypeDefinition^ GetType(System::String^ name) {
        return FindType(name);
    }

    /// <summary>
    /// Get a type by its interface type, generally from a reference assembly. Must match the name exactly.
    /// </summary>
    /// <param name="name">The name of the type.</param>
    /// <returns>The type definition (casted to an interface) if found, otherwise null.</returns>)
    /// <remarks>Cached.</remarks>
    generic <class T> where T : ref class
    T GetTypeT() {
        auto t = TypeCacher<T>::GetCachedType();

        if (t == nullptr) {
            return T();
        }

        return t->As<T>();
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

protected:
    generic <class T> where T : ref class
    static reframework::API::TypeDefinition* GetTypeDefinitionPtr();

public:
    ref class TypeDefinitionIterator : public IEnumerator {
    private:
        TDB^ m_parent;
        int m_currentIndex;
        TypeDefinition^ m_currentType;

    public:
        TypeDefinitionIterator(TDB^ parent) : m_parent(parent), m_currentIndex(-1), m_currentType(nullptr) {}

        // Required by IEnumerator
        virtual bool MoveNext() {
            if (m_currentIndex < static_cast<int>(m_parent->GetNumTypes()) - 1) {
                m_currentIndex++;
                m_currentType = m_parent->GetType(m_currentIndex);
                return true;
            } else {
                return false;
            }
        }

        virtual void Reset() {
            m_currentIndex = -1;
            m_currentType = nullptr;
        }

        property Object^ Current {
            virtual Object^ get() = IEnumerator::Current::get {
                return m_currentType;
            }
        }
    };

    ref class TypeDefinitionCollection : public IEnumerable {
    private:
        TDB^ m_parent;

    public:
        TypeDefinitionCollection(TDB^ parent) : m_parent(parent) {}

        virtual IEnumerator^ GetEnumerator() {
            return gcnew TypeDefinitionIterator(m_parent);
        }
    };

    property TypeDefinitionCollection^ Types {
        TypeDefinitionCollection^ get() {
            return gcnew TypeDefinitionCollection(this);
        }
    }
};
}