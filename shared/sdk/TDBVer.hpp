#pragma once

#ifdef DMC5
#define TDB_VER 67
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

namespace sdk {
struct RETypeDefVersion71;
struct RETypeDefVersion69;
struct RETypeDefVersion67;
struct RETypeDefVersion66;
struct RETypeDefVersion49;

struct REField;
struct REMethodDefinition;
struct REProperty;
struct RETypeDefinition;

#if defined(DD2)
#define TYPE_INDEX_BITS 19
using RETypeDefinition_ = sdk::RETypeDefVersion71; // same thing for now
#elif defined(SF6)
#define TYPE_INDEX_BITS 19
using RETypeDefinition_ = sdk::RETypeDefVersion71;
#elif defined(RE4)
#define TYPE_INDEX_BITS 19
using RETypeDefinition_ = sdk::RETypeDefVersion71;
#elif defined(MHRISE)
#ifdef MHRISE_TDB70
// it's version 70 but looks the same for now i guess
#define TYPE_INDEX_BITS 18
using RETypeDefinition_ = sdk::RETypeDefVersion69;
#else
#define TYPE_INDEX_BITS 19
using RETypeDefinition_ = sdk::RETypeDefVersion71;
#endif
#elif defined(RE8)
#define TYPE_INDEX_BITS 18
using RETypeDefinition_ = sdk::RETypeDefVersion69;
#elif defined(DMC5)
#define TYPE_INDEX_BITS 17
using RETypeDefinition_ = sdk::RETypeDefVersion67;
#elif defined(RE2) || defined(RE3)
#ifdef RE2_TDB66
#define TYPE_INDEX_BITS 16
using RETypeDefinition_ = sdk::RETypeDefVersion66;
#elif defined(RE3_TDB67)
#define TYPE_INDEX_BITS 17
using RETypeDefinition_ = sdk::RETypeDefVersion67;
#else
#define TYPE_INDEX_BITS 18
using RETypeDefinition_ = sdk::RETypeDefVersion69;
#endif
#elif RE7
#ifdef RE7_TDB49
#define TYPE_INDEX_BITS 16
using RETypeDefinition_ = sdk::RETypeDefVersion49;
#else
#define TYPE_INDEX_BITS 18
using RETypeDefinition_ = sdk::RETypeDefVersion69;
#endif
#endif
} // namespace sdk
