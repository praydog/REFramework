#pragma managed

#include <reframework/API.hpp>

#include "./API.hpp"


REFrameworkNET::API::API(uintptr_t param)
    : m_api{ reframework::API::initialize(param) }
{
    Console::WriteLine("REFrameworkNET.API Constructor called.");
}

REFrameworkNET::API::~API()
{
    Console::WriteLine("REFrameworkNET.API Destructor called.");
}

reframework::API^ REFrameworkNET::API::Get() {
    return m_api;
}