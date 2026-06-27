#pragma once
#include <cstdint>

class REFieldList;
class REClassInfo;

#pragma pack(push, 1)
class REType {
public:
    const char* get_type_name() const { return name; }
    int16_t get_flags() const { return flags; }
    uint32_t get_classIndex() const { return classIndex; }

    // Dispatching accessors — implementations in RETypeLayouts.hpp.
    uint32_t get_size() const;
    uint32_t get_typeCRC() const;
    REType* get_super() const;
    REType* get_childType() const;
    REType* get_chainType() const;
    REFieldList* get_fields() const;
    REClassInfo* get_classInfo() const;

    // Runtime sizeof(REType). 0x60 on most games, 0x68 on MHWILDS/RE9.
    static size_t runtime_size();
private:
    void*       vtable;             // 0x00
    uint32_t    classIndex;         // 0x08
    int16_t     flags;              // 0x0C — NoneType(<0), abstract(1), concrete(2), interface(256), root(16384)
    char        _pad_000E[2];       // 0x0E
    uint64_t    fastClassIndex;     // 0x10
    uint32_t    typeIndexProbably;  // 0x18
    char        _pad_001C[4];       // 0x1C
    char*       name;               // 0x20
    char        _pad_0028[4];       // 0x28
    uint32_t    size;               // 0x2C — swapped with typeCRC on TDB>=81
    uint32_t    typeCRC;            // 0x30 — swapped with size on TDB>=81
    uint32_t    miscFlags;          // 0x34
    REType*     super;              // 0x38 — shifted to 0x40 on MHWILDS/RE9
    REType*     childType;          // 0x40 — shifted to 0x48 on MHWILDS/RE9
    REType*     chainType;          // 0x48 — shifted to 0x50 on MHWILDS/RE9
    REFieldList* fields;            // 0x50 — shifted to 0x58 on MHWILDS/RE9
    REClassInfo* classInfo;         // 0x58 — shifted to 0x60 on MHWILDS/RE9; non-null = managed type
};
static_assert(sizeof(REType) == 0x60);
#pragma pack(pop)
