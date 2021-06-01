#pragma once

#include <cstdint>

#include "ReClass.hpp"

// Manual definitions of REClassInfo because ReClass doesn't have bitfields like this.
namespace sdk {
struct RETypeDefVersion69 {
    // 0 - 8
    uint64_t index : 18;
    uint64_t parent_typeid : 18;
    uint64_t declaring_typeid : 18;
    uint64_t underlying_typeid : 7;
    uint64_t object_typeid : 3;
    uint64_t array_typeid : 18;
    uint64_t element_typeid : 18;
    uint64_t impl_index : 18;
    uint64_t system_typeid : 10;

    uint32_t type_flags;    // 0x008
    uint32_t size;          // 0x0014
    uint32_t fqn_hash;      // 0x0018
    uint32_t type_crc;      // 0x001C
    uint32_t default_ctor;  // 0x0020
    uint32_t vt;            // 0x0024 vtable byte pool
    uint32_t member_method; // 0x0028
    uint32_t member_field;  // 0x002C

    // 0x0030
    uint32_t num_member_prop : 12;
    uint32_t member_prop : 19;

    uint32_t member_event;          // 0x0034
    int32_t interfaces;             // 0x0038
    int32_t generics;               // 0x003C byte pool
    struct sdk::RETypeCLR* type;     // 0x0040
    class REObjectInfo* managed_vt; // 0x0048
};

struct RETypeDefVersion67 {
    // 0-8
    uint64_t index : 17;
    uint64_t unkbitfieldthing : 13;
    uint64_t parent_typeid : 17;
    uint64_t declaring_typeid : 17;

    uint32_t fqn_hash; // murmurhash3
    uint32_t type_crc;
    char pad_0010[8];
    uint32_t name_offset;
    uint32_t namespace_offset;
    uint32_t type_flags;
    uint8_t system_type;
    char pad_0025[1];
    uint8_t object_type;
    char pad_0027[5];
    uint32_t element_size;
    uint32_t size;
    char pad_0034[4];

    uint32_t member_method;
    uint32_t num_member_method;
    uint32_t member_field;
    uint32_t num_member_field;

    uint32_t num_member_prop : 12;
    uint32_t member_prop : 19;

    char pad_004C[4];
    uint32_t interfaces;
    char pad_0054[4];
    uint32_t generics;
    char pad_005C[12];
    void* unk;
    struct sdk::RETypeCLR* type;
    class REObjectInfo* managed_vt;
};
static_assert(sizeof(RETypeDefVersion67) == 0x80);
} // namespace sdk