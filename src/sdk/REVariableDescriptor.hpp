#pragma once

#include "ReClass.hpp"

namespace sdk {
#ifndef RE7
struct PropertyFlags {
    uint32_t type_kind : 5;
    uint32_t type_qual : 2;
    uint32_t type_attr : 3;
    uint32_t size : 20;
    uint32_t managed_str : 1;
    uint32_t reserved : 1;
};
#else
struct PropertyFlags {
    uint32_t type_kind;
    uint32_t type_qual;
    uint32_t type_attr;
    bool managed_str;
};
#endif
} // namespace sdk

namespace utility::reflection_property {
static bool is_static(VariableDescriptor* v) {
    // because not all the reclass headers for old games have updated structs yet
    return ((Address{v}.get(0xC).to<uint32_t>() >> 5) & 1) != 0;
}

static sdk::PropertyFlags get_flags(VariableDescriptor* v) {
#ifndef RE7
    return *(sdk::PropertyFlags*)&v->flags;
#else
    return *(sdk::PropertyFlags*)&v->typeKind;
#endif
}

static uint32_t get_size(VariableDescriptor* v) {
#ifndef RE7
    return get_flags(v).size;
#else
    return v->size;
#endif
}
} // namespace utility::reflection_property