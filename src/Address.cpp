#include <cstdio>

#include "Address.hpp"

using namespace std;

Address::Address()
    : m_ptr(nullptr)
{}

Address::Address(void* ptr)
    : m_ptr(ptr)
{}

Address::Address(uintptr_t addr)
    : m_ptr((void*)addr)
{}
