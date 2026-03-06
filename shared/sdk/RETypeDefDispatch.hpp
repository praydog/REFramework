#pragma once

// Runtime dispatch for RETypeDefinition bitfield accessors.
// The universal build compiles RETypeDefVersion* structs with TYPE_INDEX_BITS=19.
// Games with TDB < 73 use 18-bit indices, causing bitfield misalignment.
// This header provides the 18-bit struct and accessor macros for runtime dispatch.

#include <cstdint>

#ifdef REFRAMEWORK_UNIVERSAL
#include "GameIdentity.hpp"
#endif

class REObjectInfo;

namespace sdk {

struct RETypeCLR;

// ============================================================================
// V69 with hardcoded 18-bit TYPE_INDEX_BITS / 18-bit FIELD_BITS
// Used by: RE2, RE3, RE7, RE8, MHRISE_TDB70 (TDB 69-70)
// sizeof must be 0x50 to match the types array stride.
// ============================================================================
namespace tdb_bits18 {

struct RETypeDefVersion69 {
    // First uint64_t: 18+18+18+7+3 = 64 bits exact
    uint64_t index : 18;
    uint64_t parent_typeid : 18;
    uint64_t declaring_typeid : 18;
    uint64_t underlying_typeid : 7;
    uint64_t object_type : 3;

    // Second uint64_t: 18+18+18+10 = 64 bits exact
    uint64_t array_typeid : 18;
    uint64_t element_typeid : 18;
    uint64_t impl_index : 18;
    uint64_t system_typeid : 10;

    // 0x10: non-bitfield uint32_t — same offsets regardless of TYPE_INDEX_BITS
    uint32_t type_flags;
    uint32_t size;
    uint32_t fqn_hash;
    uint32_t type_crc;
    uint32_t default_ctor;
    uint32_t vt;
    uint32_t member_method;
    uint32_t member_field;

    // 0x30
    uint32_t num_member_prop : 12;
    uint32_t member_prop : 18;

    // 0x34
    uint32_t member_event;
    int32_t interfaces;
    int32_t generics;

    // 0x40
    struct sdk::RETypeCLR* type;
    class ::REObjectInfo* managed_vt;
};


// tdb69 REMethodDefinition: 16 bytes (vs tdb84: 12 bytes)
// Used by RE2, RE3, RE7, RE8, MHRISE_TDB70 (TDB 69-70)
struct REMethodDef69 {
    uint64_t declaring_typeid : 18;
    uint64_t impl_id : 20;
    uint64_t params : 26;
    void* function;
};
static_assert(sizeof(REMethodDef69) == 16);

// tdb69 REField: 8 bytes (same stride as tdb84, different bit layout)
struct REField69 {
    uint64_t declaring_typeid : 18;
    uint64_t impl_id : 20;
    uint64_t offset : 26;
};
static_assert(sizeof(REField69) == 8);

// tdb69 REParamDef: 12 bytes (type_id is 18 bits, not 19)
struct REParamDef69 {
    uint16_t attributes_id;
    uint16_t init_data_index;
    uint32_t name_offset : 30;
    uint32_t modifier : 2;
    uint32_t type_id : 18;
    uint32_t flags : 14;
};
static_assert(sizeof(REParamDef69) == 12);
} // namespace tdb_bits18

// ============================================================================
// Runtime dispatch macros and helpers
// ============================================================================
#ifdef REFRAMEWORK_UNIVERSAL

namespace tdb_dispatch {

inline bool needs_18bit() {
    const auto tdb_ver = sdk::GameIdentity::get().tdb_ver();
    return tdb_ver < 73;
}

} // namespace tdb_dispatch

// TDEF_FIELD: Read a bitfield from a RETypeDefinition pointer with runtime dispatch.
// For TDB >= 73, the compiled V84 layout is correct.
// For TDB < 73 (RE2/RE3/RE8/RE7/MHRISE_TDB70), cast to the 18-bit V69 layout.
// Usage: TDEF_FIELD(this, impl_index) or TDEF_FIELD(m_parent, member_method)
#define TDEF_FIELD(ptr, field) \
    (sdk::tdb_dispatch::needs_18bit() \
        ? reinterpret_cast<const sdk::tdb_bits18::RETypeDefVersion69*>(ptr)->field \
        : (ptr)->field)

// TDEF_FIELD_SET: Write a bitfield. Only used for object_type currently.
#define TDEF_FIELD_SET(ptr, field, value) \
    do { \
        if (sdk::tdb_dispatch::needs_18bit()) \
            const_cast<sdk::tdb_bits18::RETypeDefVersion69*>( \
                reinterpret_cast<const sdk::tdb_bits18::RETypeDefVersion69*>(ptr))->field = (value); \
        else \
            (ptr)->field = (value); \
    } while(0)


// TMETH_FIELD: Read a bitfield from a REMethodDefinition pointer with runtime dispatch.
// For TDB < 73 (tdb69), casts to REMethodDef69.
// Fields common to both: declaring_typeid, impl_id
#define TMETH_FIELD(ptr, field) \
    (sdk::tdb_dispatch::needs_18bit() \
        ? reinterpret_cast<const sdk::tdb_bits18::REMethodDef69*>(ptr)->field \
        : (ptr)->field)

// TFIELD_FIELD: Read a bitfield from a REField pointer with runtime dispatch.
// For TDB < 73 (tdb69), casts to REField69.
// Fields common to both: declaring_typeid, impl_id
#define TFIELD_FIELD(ptr, field) \
    (sdk::tdb_dispatch::needs_18bit() \
        ? reinterpret_cast<const sdk::tdb_bits18::REField69*>(ptr)->field \
        : (ptr)->field)

// TPARAM_FIELD: Read a bitfield from a REParameterDef pointer with runtime dispatch.
// For TDB < 73 (tdb69), casts to REParamDef69.
#define TPARAM_FIELD(ptr, field) \
    (sdk::tdb_dispatch::needs_18bit() \
        ? reinterpret_cast<const sdk::tdb_bits18::REParamDef69*>(ptr)->field \
        : (ptr)->field)
#else // Non-universal builds: no dispatch needed, direct access.

#define TDEF_FIELD(ptr, field) ((ptr)->field)
#define TDEF_FIELD_SET(ptr, field, value) ((ptr)->field = (value))
#define TMETH_FIELD(ptr, field) ((ptr)->field)
#define TFIELD_FIELD(ptr, field) ((ptr)->field)
#define TPARAM_FIELD(ptr, field) ((ptr)->field)

#endif // REFRAMEWORK_UNIVERSAL

} // namespace sdk
