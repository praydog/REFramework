#pragma once

#include <string>

namespace utility {
namespace exceptions {
using AddressNameResolver = std::string(*)(uintptr_t);
void set_address_name_resolver(AddressNameResolver resolver);
void dump_callstack(struct ::_EXCEPTION_POINTERS* exception);
}
}