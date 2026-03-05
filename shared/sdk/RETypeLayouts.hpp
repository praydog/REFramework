#pragma once

// Runtime-dispatch layouts for REType and related structs.
// Under REFRAMEWORK_UNIVERSAL, the global REType is the RE8 layout (0x60).
// MHWILDS/RE9 have a larger REType (0x68) with fields shifted after offset 0x28.
// This header provides the alternate layouts in namespaces for runtime casting.

#include <cstdint>

#ifdef REFRAMEWORK_UNIVERSAL
#include "GameIdentity.hpp"

// MHWILDS/RE9 REType layout (0x68) — super at 0x40 instead of 0x38
namespace reclass_mhwilds {

class REFieldList;
class REClassInfo;

#pragma pack(push, 1)
class REType {
public:
    void*       vfptr;              // 0x0000
    uint32_t    classIndex;         // 0x0008
    int16_t     flags;              // 0x000C
    char        pad_000E[2];        // 0x000E
    uint64_t    fastClassIndex;     // 0x0010
    uint32_t    typeIndexProbably;  // 0x0018
    char        pad_001C[4];        // 0x001C
    char*       name;               // 0x0020
    uint32_t    parentTypeId;       // 0x0028
    uint32_t    typeCRC;            // 0x002C
    uint32_t    size;               // 0x0030
    uint32_t    miscFlags;          // 0x0034
    char        pad_0038[8];        // 0x0038
    ::REType*   super;              // 0x0040
    ::REType*   childType;          // 0x0048
    ::REType*   chainType;          // 0x0050
    REFieldList* fields;            // 0x0058
    ::REClassInfo* classInfo;       // 0x0060
}; // 0x0068
static_assert(sizeof(REType) == 0x68);
#pragma pack(pop)

} // namespace reclass_mhwilds

// Returns true if the running game uses the 0x68 REType layout.
inline bool retype_is_large_layout() {
    const auto& gi = sdk::GameIdentity::get();
    return gi.is_mhwilds() || gi.is_re9();
}

// Runtime accessors for REType fields that differ between layouts.
// Fields at offsets 0x00-0x0020 (through `name`) are identical in all layouts.
// Fields from 0x0028 onward shift by 8 bytes in MHWILDS/RE9.
namespace utility::re_type_accessor {

inline uint32_t get_size(const ::REType* t) {
    if (retype_is_large_layout()) {
        return reinterpret_cast<const reclass_mhwilds::REType*>(t)->size;
    }
    return t->size;
}

inline uint32_t get_typeCRC(const ::REType* t) {
    if (retype_is_large_layout()) {
        return reinterpret_cast<const reclass_mhwilds::REType*>(t)->typeCRC;
    }
    return t->typeCRC;
}

inline ::REType* get_super(const ::REType* t) {
    if (retype_is_large_layout()) {
        return reinterpret_cast<const reclass_mhwilds::REType*>(t)->super;
    }
    return t->super;
}

inline ::REType* get_childType(const ::REType* t) {
    if (retype_is_large_layout()) {
        return reinterpret_cast<const reclass_mhwilds::REType*>(t)->childType;
    }
    return t->childType;
}

inline ::REType* get_chainType(const ::REType* t) {
    if (retype_is_large_layout()) {
        return reinterpret_cast<const reclass_mhwilds::REType*>(t)->chainType;
    }
    return t->chainType;
}

inline ::REFieldList* get_fields(const ::REType* t) {
    if (retype_is_large_layout()) {
        return reinterpret_cast<::REFieldList*>(
            reinterpret_cast<const reclass_mhwilds::REType*>(t)->fields);
    }
    return t->fields;
}

inline ::REClassInfo* get_classInfo(const ::REType* t) {
    if (retype_is_large_layout()) {
        return reinterpret_cast<::REClassInfo*>(
            reinterpret_cast<const reclass_mhwilds::REType*>(t)->classInfo);
    }
    return t->classInfo;
}

} // namespace utility::re_type_accessor

#else
// Non-universal builds: direct access is always correct.
namespace utility::re_type_accessor {
inline uint32_t get_size(const ::REType* t) { return t->size; }
inline uint32_t get_typeCRC(const ::REType* t) { return t->typeCRC; }
inline ::REType* get_super(const ::REType* t) { return t->super; }
inline ::REType* get_childType(const ::REType* t) { return t->childType; }
inline ::REType* get_chainType(const ::REType* t) { return t->chainType; }
inline ::REFieldList* get_fields(const ::REType* t) { return t->fields; }
inline ::REClassInfo* get_classInfo(const ::REType* t) { return t->classInfo; }
} // namespace utility::re_type_accessor

#endif // REFRAMEWORK_UNIVERSAL
