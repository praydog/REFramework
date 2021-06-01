#pragma once

#include <cstdint>

#include "RENativeArray.hpp"

#include "ReClass.hpp"

// Structures for CLR types
// Same structure across all the games so we don't care about the ReClass header
namespace sdk {
struct DeserializeSequence {
    uint32_t code : 8;
    uint32_t size : 8;
    uint32_t align : 8;
    uint32_t depth : 6;
    uint32_t is_array : 1;
    uint32_t is_static : 1;

    uint32_t offset;
    class ::REClassInfo* native_type;
};

struct RETypeCLR : public ::REType {
    sdk::NativeArray<sdk::DeserializeSequence> deserializers;
    class ::REType* native_type;
    char* name2;
};
} // namespace sdk