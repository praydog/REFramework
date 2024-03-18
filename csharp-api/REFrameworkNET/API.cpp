#pragma managed

#include <reframework/API.hpp>

#include "./API.hpp"


REFrameworkNET::API::API(const REFrameworkPluginInitializeParam* param)
{
    Init_Internal(param);
}

REFrameworkNET::API::API(uintptr_t param)
{
    Init_Internal(reinterpret_cast<const REFrameworkPluginInitializeParam*>(param));
}

void REFrameworkNET::API::Init_Internal(const REFrameworkPluginInitializeParam* param)
{
    Console::WriteLine("REFrameworkNET.API Init_Internal called.");
    s_api = reframework::API::initialize(param).get();
    Callbacks::Impl::Setup(this);
    Console::WriteLine("REFrameworkNET.API Init_Internal finished.");
}

REFrameworkNET::API::~API()
{
    Console::WriteLine("REFrameworkNET.API Destructor called.");
}