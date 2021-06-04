#pragma once

#include <cstdint>

#include "RETypeDefinition.hpp"

class RETypeDB;
class REClassInfo;

namespace sdk {
namespace tdb69 {
using TDB = RETypeDB;
}

namespace tdb67 {
struct REMethodDefinition;
struct REField;
struct REProperty;

struct TDB {
    uint32_t magic;                                    // 0x0000
    uint32_t version;                                  // 0x0004
    uint32_t initialized;                              // 0x0008
    uint32_t numTypes;                                 // 0x000C
    uint32_t numMethods;                               // 0x0010
    uint32_t numFields;                                // 0x0014
    uint32_t numProperties;                            // 0x0018
    uint32_t numEvents;                                // 0x001C
    uint32_t numUnk;                                   // 0x0020
    uint32_t maybeNumParams;                           // 0x0024
    uint32_t maybeNumAttributes;                       // 0x0028
    uint32_t maybeNumInitData;                         // 0x002C
    uint32_t numInternStrings;                         // 0x0030
    uint32_t numModules;                               // 0x0034
    uint32_t devEntry;                                 // 0x0038
    uint32_t appEntry;                                 // 0x003C
    uint32_t numStringPool;                            // 0x0040
    uint32_t numBytePool;                              // 0x0044
    class N00002524 (*modules)[256];                   // 0x0048
    sdk::RETypeDefinition (*types)[81728];             // 0x0050
    sdk::tdb67::REMethodDefinition (*methods)[556344]; // 0x0058
    sdk::tdb67::REField (*fields)[122496];             // 0x0060
    sdk::tdb67::REProperty (*properties)[119791];            // 0x0068
    void* events;                                      // 0x0070
    char pad_0078[8];                                  // 0x0078
    void* N0000243D;                                   // 0x0080
    void* N0000243E;                                   // 0x0088
    void* N0000243F;                                   // 0x0090
    char (*stringPool)[0];                             // 0x0098
    uint8_t (*bytePool)[1];                            // 0x00A0
    uint32_t (*internStrings)[17014];                  // 0x00A8
};

struct REMethodDefinition {
    uint64_t declaring_typeid : TYPE_INDEX_BITS;
    uint64_t invoke_id : 16;
    uint64_t num_params : 6;
    uint64_t unk : 8; // NOT REALLY SURE WHAT THIS IS? IT HAS SOMETHING TO DO WITH RETURN TYPE
    uint64_t return_typeid : TYPE_INDEX_BITS;
    char pad_0008[2];
    int16_t vtable_index;
    uint32_t name_offset;
    uint16_t flags;
    uint16_t impl_flags;
    uint32_t params; // bytepool
    void* function;
};

struct REMethodParamDef {
    uint64_t param_typeid : TYPE_INDEX_BITS;
    uint64_t flags : 16;
    uint64_t name_offset : 31;
};

struct REField {
    uint64_t declaring_typeid : TYPE_INDEX_BITS;
    uint64_t field_typeid : TYPE_INDEX_BITS;
    // TODO: fill in rest of bitfield

    uint32_t name_offset;
    uint16_t flags;
    uint16_t unk;
    uint32_t offset;
    uint32_t unk2;
};

struct REProperty {
    char pad_0000[4];    // 0x0000
    uint32_t name_offset; // 0x0004
    uint32_t getter;     // 0x0008
    uint32_t setter;     // 0x000C
};
} // namespace tdb67
} // namespace sdk

// laziness
/*#ifdef DMC5
class RETypeDB : public sdk::tdb67::TDB {};
using REMethodDefinition = sdk::tdb67::REMethodDefinition;
using REField = sdk::tdb67::REField;
using REProperty = sdk::tdb67::REProperty;
#endif*/