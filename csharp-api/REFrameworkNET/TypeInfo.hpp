#pragma once

#include <reframework/API.hpp>

#pragma managed

namespace REFrameworkNET {
public ref class TypeInfo {
public:
    TypeInfo(reframework::API::TypeInfo* typeInfo) : m_typeInfo(typeInfo) {}
    TypeInfo(::REFrameworkTypeInfoHandle handle) : m_typeInfo(reinterpret_cast<reframework::API::TypeInfo*>(handle)) {}

private:
    reframework::API::TypeInfo* m_typeInfo;
};
};