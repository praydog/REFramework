#pragma once

class REType;

namespace utility::re_type {
uint32_t get_vm_type(::REType* t);
uint32_t get_value_type_size(::REType* t);
} // namespace utility::re_type