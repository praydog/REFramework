#pragma once

#include "ReClass.hpp"

namespace utility::reflection_property {
    static bool is_static(VariableDescriptor* v) {
        // because not all the reclass headers for old games have updated structs yet
        return ((Address{ v }.get(0xC).to<uint32_t>() >> 5) & 1) != 0;
    }
}