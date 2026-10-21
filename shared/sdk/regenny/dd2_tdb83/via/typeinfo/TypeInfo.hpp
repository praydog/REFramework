#pragma once
namespace regenny {
struct TypeDefinition;
}
namespace regenny::via::typeinfo {
#pragma pack(push, 1)
struct TypeInfo {
    void* vfptr; // 0x0
    uint32_t fqn_hash; // 0x8
    uint16_t attributes; // 0xc
    uint16_t type; // 0xe
    uint64_t fast_class_index; // 0x10
    uint64_t bsr_fast_class_index; // 0x18
    // Metadata: utf8*
    char* name; // 0x20
    uint32_t parent_typeid; // 0x28
    uint32_t size; // 0x2c
    uint32_t crc; // 0x30
    uint32_t mics_flags; // 0x34
    regenny::via::typeinfo::TypeInfo* parent; // 0x38
    regenny::via::typeinfo::TypeInfo* child; // 0x40
    regenny::via::typeinfo::TypeInfo* next; // 0x48
    void* reflection; // 0x50
    regenny::TypeDefinition* type_definition; // 0x58
}; // Size: 0x60
#pragma pack(pop)
}
