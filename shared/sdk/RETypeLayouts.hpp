#pragma once

// Runtime-dispatch layouts for REType and related structs.
// Under REFRAMEWORK_UNIVERSAL, the global REType is the RE8 layout (0x60).
// MHWILDS/RE9 have a larger REType (0x68) with fields shifted after offset 0x28.
// This header provides the alternate layouts in namespaces for runtime casting.

#include <cstdint>

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

// Returns true if the running game uses the 0x68 REType layout (super at 0x40, +8 shift).
// Only MHWILDS and RE9 have this; MHSTORIES3/PRAGMATA are 0x60 despite tdb >= 82.
inline bool retype_has_shifted_pointers() {
    const auto& gi = sdk::GameIdentity::get();
    return gi.is_mhwilds() || gi.is_re9();
}

// Returns true if typeCRC/size fields are reordered relative to RE8.
// All tdb >= 81 games swap these two scalar fields (typeCRC@0x2C, size@0x30
// instead of RE8's size@0x2C, typeCRC@0x30). This is independent of the
// pointer shift — MHSTORIES3/PRAGMATA have the scalar swap but NOT the shift.
inline bool retype_has_field_reorder() {
    return sdk::GameIdentity::get().tdb_ver() >= 81;
}

// Runtime accessors for REType fields that differ between layouts.
// Fields at offsets 0x00-0x0020 (through `name`) are identical in all layouts.
// Scalar fields (size, typeCRC) swap positions in tdb >= 81.
// Pointer fields (super, childType, chainType, fields, classInfo) shift +8 only in MHWILDS/RE9.
// Member method implementations for REType dispatching accessors.
// These are declared in ReClass_Internal_RE8.hpp and defined here
// because they need GameIdentity.hpp which isn't available in the
// ReClass header.

inline uint32_t REType::get_size() const {
    if (retype_has_field_reorder()) {
        return reinterpret_cast<const reclass_mhwilds::REType*>(this)->size;
    }
    return size;
}

inline uint32_t REType::get_typeCRC() const {
    if (retype_has_field_reorder()) {
        return reinterpret_cast<const reclass_mhwilds::REType*>(this)->typeCRC;
    }
    return typeCRC;
}

inline ::REType* REType::get_super() const {
    if (retype_has_shifted_pointers()) {
        return reinterpret_cast<const reclass_mhwilds::REType*>(this)->super;
    }
    return super;
}

inline ::REType* REType::get_childType() const {
    if (retype_has_shifted_pointers()) {
        return reinterpret_cast<const reclass_mhwilds::REType*>(this)->childType;
    }
    return childType;
}

inline ::REType* REType::get_chainType() const {
    if (retype_has_shifted_pointers()) {
        return reinterpret_cast<const reclass_mhwilds::REType*>(this)->chainType;
    }
    return chainType;
}

inline ::REFieldList* REType::get_fields() const {
    if (retype_has_shifted_pointers()) {
        return reinterpret_cast<::REFieldList*>(
            reinterpret_cast<const reclass_mhwilds::REType*>(this)->fields);
    }
    return fields;
}

inline ::REClassInfo* REType::get_classInfo() const {
    if (retype_has_shifted_pointers()) {
        return reinterpret_cast<::REClassInfo*>(
            reinterpret_cast<const reclass_mhwilds::REType*>(this)->classInfo);
    }
    return classInfo;
}

// Backward-compatible free-function aliases in the old namespace.
// New code should use the member methods directly (t->get_size() etc.).
namespace utility::re_type_accessor {
inline uint32_t get_size(const ::REType* t) { return t->get_size(); }
inline uint32_t get_typeCRC(const ::REType* t) { return t->get_typeCRC(); }
inline ::REType* get_super(const ::REType* t) { return t->get_super(); }
inline ::REType* get_childType(const ::REType* t) { return t->get_childType(); }
inline ::REType* get_chainType(const ::REType* t) { return t->get_chainType(); }
inline ::REFieldList* get_fields(const ::REType* t) { return t->get_fields(); }
inline ::REClassInfo* get_classInfo(const ::REType* t) { return t->get_classInfo(); }
} // namespace utility::re_type_accessor

