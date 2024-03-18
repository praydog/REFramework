#pragma managed

#include <reframework/API.hpp>

#include "./API.hpp"


REFrameworkNET::API::API(const REFrameworkPluginInitializeParam* param)
{
    Console::WriteLine("REFrameworkNET.API Constructor called.");
    s_api = reframework::API::initialize(param).get();
}

REFrameworkNET::API::API(uintptr_t param)
{
    Console::WriteLine("REFrameworkNET.API Constructor called.");
    s_api = reframework::API::initialize((const REFrameworkPluginInitializeParam*)param).get();
}

REFrameworkNET::API::~API()
{
    Console::WriteLine("REFrameworkNET.API Destructor called.");
}