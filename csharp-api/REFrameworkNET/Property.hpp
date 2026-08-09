#pragma once

#include <reframework/API.hpp>

#pragma managed

namespace REFrameworkNET {
public ref class Property {
public:
    Property(const reframework::API::Property* prop) : m_prop(prop) {}

private:
    const reframework::API::Property* m_prop;
};
}