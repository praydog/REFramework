#pragma once

#include "ReClass.hpp"

namespace sdk {
struct PropertyFlags {
    uint32_t type_kind : 5;
    uint32_t type_qual : 2;
    uint32_t type_attr : 3;
    uint32_t size : 20;
    uint32_t managed_str : 1;
    uint32_t reserved : 1;
};
} // namespace sdk

namespace utility::reflection_property {
static bool is_static(VariableDescriptor* v) {
    // because not all the reclass headers for old games have updated structs yet
    return ((Address{v}.get(0xC).to<uint32_t>() >> 5) & 1) != 0;
}
} // namespace utility::reflection_property