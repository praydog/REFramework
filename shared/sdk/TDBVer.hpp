#pragma once

#include <cstdint>

// ============================================================================
// TDB Version Configuration
// ============================================================================
//
// In the monolithic build (REFRAMEWORK_UNIVERSAL), TDB_VER is set to the
// maximum supported version so ALL code paths compile. Runtime dispatch uses
// sdk::GameIdentity::get().tdb_ver() for actual version-specific logic.
//
// In legacy per-game builds, TDB_VER is set based on the game macro as before.
// ============================================================================

#ifdef REFRAMEWORK_UNIVERSAL

// Monolithic build: compile ALL version-dependent code paths.
// Use the maximum TDB version so every #if TDB_VER >= X passes.
#define TDB_VER 84
#define TYPE_INDEX_BITS 19
#define FIELD_BITS 20

#else // Legacy per-game builds

#ifdef DMC5
#define TDB_VER 67
#elif defined(PRAGMATA)
#define TDB_VER 84
#elif defined(RE9)
#define TDB_VER 83
#elif defined(MHSTORIES3)
#define TDB_VER 82
#elif defined(MHWILDS)
#define TDB_VER 81
#elif defined(DD2)
#define TDB_VER 73
#elif defined(SF6)
#define TDB_VER 71
#elif defined (RE4)
#define TDB_VER 71
#elif defined(MHRISE)
#ifdef MHRISE_TDB70
#define TDB_VER 70
#else
#define TDB_VER 71
#endif
#elif defined(RE8)
#define TDB_VER 69
#elif RE3
#ifdef RE3_TDB67
#define TDB_VER 67
#else
#define TDB_VER 70
#endif
#elif RE2
#ifdef RE2_TDB66
#define TDB_VER 66
#else
#define TDB_VER 70
#endif
#elif RE7
#ifdef RE7_TDB49
#define TDB_VER 49
#else
#define TDB_VER 70
#endif
#endif

// Legacy builds: TYPE_INDEX_BITS / FIELD_BITS from game macros
#ifndef TYPE_INDEX_BITS
#if defined(PRAGMATA) || defined(RE9) || defined(MHSTORIES3) || defined(MHWILDS)
#define TYPE_INDEX_BITS 19
#define FIELD_BITS 20
#elif defined(DD2) || defined(SF6) || defined(RE4)
#define TYPE_INDEX_BITS 19
#define FIELD_BITS 19
#elif defined(MHRISE)
#ifdef MHRISE_TDB70
#define TYPE_INDEX_BITS 18
#define FIELD_BITS 18
#else
#define TYPE_INDEX_BITS 19
#define FIELD_BITS 19
#endif
#elif defined(RE8)
#define TYPE_INDEX_BITS 18
#define FIELD_BITS 18
#elif defined(DMC5) || defined(RE3_TDB67)
#define TYPE_INDEX_BITS 17
#define FIELD_BITS 17
#elif defined(RE2_TDB66) || defined(RE7_TDB49)
#define TYPE_INDEX_BITS 16
#define FIELD_BITS 16
#elif defined(RE2) || defined(RE3) || defined(RE7)
#define TYPE_INDEX_BITS 18
#define FIELD_BITS 18
#endif
#endif // TYPE_INDEX_BITS

#endif // REFRAMEWORK_UNIVERSAL

namespace sdk {

// Forward declarations — all TDB struct versions exist in all builds.
struct RETypeDefVersion84;
struct RETypeDefVersion83;
struct RETypeDefVersion82;
struct RETypeDefVersion74;
struct RETypeDefVersion71;
struct RETypeDefVersion69;
struct RETypeDefVersion67;
struct RETypeDefVersion66;
struct RETypeDefVersion49;

struct REField;
struct REMethodDefinition;
struct REProperty;
struct RETypeDefinition;

// ============================================================================
// RETypeDefinition_ alias
// ============================================================================
//
// In the monolithic build, we use RETypeDefVersion84 (the largest/newest)
// as the compile-time type. At runtime, accessor functions in
// RETypeDefinition.cpp handle version-specific field offsets.
//
// In legacy per-game builds, the alias points to the exact version.
// ============================================================================

#ifdef REFRAMEWORK_UNIVERSAL
using RETypeDefinition_ = sdk::RETypeDefVersion84;
#else
#if defined(PRAGMATA)
using RETypeDefinition_ = sdk::RETypeDefVersion84;
#elif defined(RE9)
using RETypeDefinition_ = sdk::RETypeDefVersion83;
#elif defined(MHSTORIES3)
using RETypeDefinition_ = sdk::RETypeDefVersion82;
#elif defined(MHWILDS)
using RETypeDefinition_ = sdk::RETypeDefVersion74;
#elif defined(DD2)
using RETypeDefinition_ = sdk::RETypeDefVersion71;
#elif defined(SF6)
using RETypeDefinition_ = sdk::RETypeDefVersion71;
#elif defined(RE4)
using RETypeDefinition_ = sdk::RETypeDefVersion71;
#elif defined(MHRISE)
#ifdef MHRISE_TDB70
using RETypeDefinition_ = sdk::RETypeDefVersion69;
#else
using RETypeDefinition_ = sdk::RETypeDefVersion71;
#endif
#elif defined(RE8)
using RETypeDefinition_ = sdk::RETypeDefVersion69;
#elif defined(DMC5)
using RETypeDefinition_ = sdk::RETypeDefVersion67;
#elif defined(RE2) || defined(RE3)
#ifdef RE2_TDB66
using RETypeDefinition_ = sdk::RETypeDefVersion66;
#elif defined(RE3_TDB67)
using RETypeDefinition_ = sdk::RETypeDefVersion67;
#else
using RETypeDefinition_ = sdk::RETypeDefVersion69;
#endif
#elif RE7
#ifdef RE7_TDB49
using RETypeDefinition_ = sdk::RETypeDefVersion49;
#else
using RETypeDefinition_ = sdk::RETypeDefVersion69;
#endif
#endif
#endif // REFRAMEWORK_UNIVERSAL

} // namespace sdk
