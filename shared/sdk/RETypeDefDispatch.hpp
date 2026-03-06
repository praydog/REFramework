#pragma once

// Runtime dispatch for RETypeDefinition / REMethodDef / REField / REParamDef
// bitfield accessors.
//
// Each TDB version struct now has hardcoded bit widths in RETypeDefinition.hpp.
// This header provides:
//   1. tdb69:: namespace with 18-bit method/field/param structs
//   2. Runtime dispatch macros (TDEF_FIELD, TMETH_FIELD, etc.)
//   3. Helper predicates (needs_18bit, needs_v71, needs_plain_impl)

#include <cstdint>

#ifdef REFRAMEWORK_UNIVERSAL
#include "GameIdentity.hpp"
#endif

class REObjectInfo;

namespace sdk {

struct RETypeCLR;
struct RETypeDefVersion69;  // defined in RETypeDefinition.hpp
struct RETypeDefVersion71;  // defined in RETypeDefinition.hpp
struct RETypeDefVersion67;  // defined in RETypeDefinition.hpp (DMC5 / TDB < 69)

// ============================================================================
// Forward declarations for tdb69 sub-structs (defined in RETypeDB.hpp)
// Used by dispatch macros for TDB 69-70 (18-bit TYPE_INDEX_BITS)
// ============================================================================
namespace tdb69 {
    struct REMethodDefinition;
    struct REField;
    struct REParameterDef;
} // namespace tdb69

// Forward declarations for tdb67 sub-structs (TDB < 69, pre-impl split)
namespace tdb67 {
    struct REMethodDefinition;
    struct REField;
    struct REMethodParamDef;
} // namespace tdb67

// Backwards-compat aliases — old code used tdb_bits18::* names
namespace tdb_bits18 {
    using REMethodDef69 = tdb69::REMethodDefinition;
    using REField69     = tdb69::REField;
    using REParamDef69  = tdb69::REParameterDef;
    using RETypeDefVersion69 = sdk::RETypeDefVersion69;
} // namespace tdb_bits18


// ============================================================================
// Runtime dispatch macros and helpers
// ============================================================================
#ifdef REFRAMEWORK_UNIVERSAL

namespace tdb_dispatch {

// TDB < 69 has no TypeImpl split — fields like name_offset, num_member_method
// live directly on RETypeDefinition (Version67 struct).
inline bool needs_pre_impl() {
    return sdk::GameIdentity::get().tdb_ver() < 69;
}

// TDB 69-70 uses 18-bit TYPE_INDEX_BITS.  TDB 71+ uses 19-bit.
inline bool needs_18bit() {
    return sdk::GameIdentity::get().tdb_ver() < 71;
}

// TDB 71-73 has struct stride 0x48 (RETypeDefVersion71, no unk_new_tdb74_uint64).
// TDB 69-70 and TDB 74+ have stride 0x50.
inline bool needs_v71_stride() {
    const auto v = sdk::GameIdentity::get().tdb_ver();
    return v >= 71 && v < 74;
}

// TDB 83+ uses 28-bit bitfields for RETypeImpl name_offset/namespace_offset.
// TDB < 83 uses plain int32_t. Other fields are identical.
inline bool needs_plain_impl() {
    return sdk::GameIdentity::get().tdb_ver() < 83;
}

} // namespace tdb_dispatch

// TDEF_FIELD: Read a bitfield from a RETypeDefinition pointer with runtime dispatch.
// Use for fields that exist in ALL TDB version structs (member_method, member_field,
// member_prop, num_member_prop, fqn_hash, type, managed_vt, size, type_flags,
// system_type, object_type, default_ctor, vt, events, interfaces, generics, etc.).
//   TDB < 69:  cast to RETypeDefVersion67 (pre-impl, all fields on the typedef)
//   TDB 69-70: cast to RETypeDefVersion69 (18-bit fields, impl split)
//   TDB 71-73: cast to RETypeDefVersion71 (19-bit, no extra uint64)
//   TDB 74+:   use compiled-in V84 layout
#define TDEF_FIELD(ptr, field) \
    (sdk::tdb_dispatch::needs_pre_impl() \
        ? reinterpret_cast<const sdk::RETypeDefVersion67*>(ptr)->field \
        : sdk::tdb_dispatch::needs_18bit() \
            ? reinterpret_cast<const sdk::RETypeDefVersion69*>(ptr)->field \
            : sdk::tdb_dispatch::needs_v71_stride() \
                ? reinterpret_cast<const sdk::RETypeDefVersion71*>(ptr)->field \
                : (ptr)->field)

// TDEF_FIELD_69: Read a bitfield that only exists in TDB >= 69 structs (impl_index,
// array_typeid, element_typeid, etc.). Does NOT dispatch to RETypeDefVersion67.
// Caller MUST gate with tdb_ver() >= 69 check.
#define TDEF_FIELD_69(ptr, field) \
    (sdk::tdb_dispatch::needs_18bit() \
        ? reinterpret_cast<const sdk::RETypeDefVersion69*>(ptr)->field \
        : sdk::tdb_dispatch::needs_v71_stride() \
            ? reinterpret_cast<const sdk::RETypeDefVersion71*>(ptr)->field \
            : (ptr)->field)

// TDEF_FIELD_PRE_IMPL: Access fields that only exist in TDB < 69 type definitions
// (before the impl split). Fields: num_member_method, num_member_field, name_offset,
// namespace_offset, element_size. These fields were moved to RETypeImpl in TDB 69+.
// Caller MUST gate with tdb_ver() < 69 check — calling on TDB >= 69 is undefined.
#define TDEF_FIELD_PRE_IMPL(ptr, field) \
    (reinterpret_cast<const sdk::RETypeDefVersion67*>(ptr)->field)

// TDEF_FIELD_SET: Write a bitfield. Only used for object_type currently.
#define TDEF_FIELD_SET(ptr, field, value) \
    do { \
        if (sdk::tdb_dispatch::needs_pre_impl()) \
            const_cast<sdk::RETypeDefVersion67*>( \
                reinterpret_cast<const sdk::RETypeDefVersion67*>(ptr))->field = (value); \
        else if (sdk::tdb_dispatch::needs_18bit()) \
            const_cast<sdk::RETypeDefVersion69*>( \
                reinterpret_cast<const sdk::RETypeDefVersion69*>(ptr))->field = (value); \
        else if (sdk::tdb_dispatch::needs_v71_stride()) \
            const_cast<sdk::RETypeDefVersion71*>( \
                reinterpret_cast<const sdk::RETypeDefVersion71*>(ptr))->field = (value); \
        else \
            (ptr)->field = (value); \
    } while(0)

// TIMPL_FIELD: Read a field from an RETypeImpl pointer with runtime dispatch.
// TDB 83+ uses 28-bit bitfields for name_offset/namespace_offset packed in int64_t.
// TDB < 83 uses plain int32_t for name_offset/namespace_offset.
// Usage: TIMPL_FIELD(impl, name_offset) or TIMPL_FIELD(impl, namespace_offset)
#define TIMPL_FIELD(ref, field) \
    (sdk::tdb_dispatch::needs_plain_impl() \
        ? reinterpret_cast<const sdk::tdb82::RETypeImpl*>(&(ref))->field \
        : (ref).field)

// TIMPL_DISPATCH: Read any field from RETypeImpl with full per-version dispatch.
// Unlike TIMPL_FIELD (which only handles name_offset/namespace_offset),
// this dispatches to the correct RETypeImpl struct for EVERY TDB version.
// Required for fields whose offset differs across versions (e.g. num_member_methods,
// num_member_fields) where tdb69 has a fundamentally different struct layout.
// Returns the field value cast to the specified type for consistency.
#define TIMPL_DISPATCH(ret_type, ref, field) \
    [&]() -> ret_type { \
        switch (sdk::GameIdentity::get().tdb_ver()) { \
        case 69: case 70: return static_cast<ret_type>(reinterpret_cast<const sdk::tdb69::RETypeImpl*>(&(ref))->field); \
        case 71: case 72: return static_cast<ret_type>(reinterpret_cast<const sdk::tdb71::RETypeImpl*>(&(ref))->field); \
        case 73:          return static_cast<ret_type>(reinterpret_cast<const sdk::tdb73::RETypeImpl*>(&(ref))->field); \
        case 74:          return static_cast<ret_type>(reinterpret_cast<const sdk::tdb74::RETypeImpl*>(&(ref))->field); \
        case 81:          return static_cast<ret_type>(reinterpret_cast<const sdk::tdb81::RETypeImpl*>(&(ref))->field); \
        case 82:          return static_cast<ret_type>(reinterpret_cast<const sdk::tdb82::RETypeImpl*>(&(ref))->field); \
        case 83:          return static_cast<ret_type>(reinterpret_cast<const sdk::tdb83::RETypeImpl*>(&(ref))->field); \
        default:          return static_cast<ret_type>((ref).field); \
        } \
    }()

// TMETH_FIELD: Read a bitfield from a REMethodDefinition pointer with runtime dispatch.
//   TDB 69-70: cast to tdb69::REMethodDefinition (18-bit TYPE_INDEX_BITS)
//   TDB 71+:   use compiled-in layout
// NOTE: For TDB < 69 fields (vtable_index, flags, impl_flags, num_params, etc.),
//       cast directly to tdb67::REMethodDefinition — those fields don't exist in tdb69+.
//       For declaring_typeid, use tmeth_declaring_typeid() below instead.
#define TMETH_FIELD(ptr, field) \
    (sdk::tdb_dispatch::needs_18bit() \
        ? reinterpret_cast<const sdk::tdb69::REMethodDefinition*>(ptr)->field \
        : (ptr)->field)


// TFIELD_FIELD: Read a bitfield from a REField pointer with runtime dispatch.
//   TDB 69-70: cast to tdb69::REField (18-bit TYPE_INDEX_BITS)
//   TDB 71+:   use compiled-in layout
// NOTE: For TDB < 69 fields, cast directly to tdb67::REField.
//       For declaring_typeid, use tfield_declaring_typeid() below instead.
#define TFIELD_FIELD(ptr, field) \
    (sdk::tdb_dispatch::needs_18bit() \
        ? reinterpret_cast<const sdk::tdb69::REField*>(ptr)->field \
        : (ptr)->field)


// TPARAM_FIELD: Read a bitfield from a REParameterDef pointer with runtime dispatch.
//   TDB 69-70: cast to tdb69::REParameterDef
//   TDB 71+:   use compiled-in layout
// NOTE: TDB < 69 does not use REParameterDef — params are in the byte pool.
#define TPARAM_FIELD(ptr, field) \
    (sdk::tdb_dispatch::needs_18bit() \
        ? reinterpret_cast<const sdk::tdb69::REParameterDef*>(ptr)->field \
        : (ptr)->field)
#else // Non-universal builds: no dispatch needed, direct access.

#define TDEF_FIELD(ptr, field) ((ptr)->field)
#define TDEF_FIELD_69(ptr, field) ((ptr)->field)
#define TDEF_FIELD_PRE_IMPL(ptr, field) ((ptr)->field)
#define TDEF_FIELD_SET(ptr, field, value) ((ptr)->field = (value))
#define TMETH_FIELD(ptr, field) ((ptr)->field)
#define TFIELD_FIELD(ptr, field) ((ptr)->field)
#define TPARAM_FIELD(ptr, field) ((ptr)->field)
#define TIMPL_FIELD(ref, field) ((ref).field)

#endif // REFRAMEWORK_UNIVERSAL

} // namespace sdk
