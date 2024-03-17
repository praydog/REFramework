#include <reframework/API.hpp>

#include "REFrameworkAPI.hpp"


REFramework::API::API(uintptr_t param)
    : m_api{ reframework::API::initialize(param) }
{
    Console::WriteLine("Constructor called.");
}


REFramework::API::~API()
{
    Console::WriteLine("Destructor called.");
}

void REFramework::API::Test() {
    Console::WriteLine("Test called.");
    m_api->log_info("Hello from C++/CLI!");
}

reframework::API^ REFramework::API::Get() {
    return m_api;
}