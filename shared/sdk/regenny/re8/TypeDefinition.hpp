#pragma once
namespace regenny::via::typeinfo {
struct TypeInfo;
}
namespace regenny {
struct ManagedVtable;
}
namespace regenny {
#pragma pack(push, 1)
struct TypeDefinition {
    uint64_t index : 17; // 0x0
    uint64_t unk : 13; // 0x0
    uint64_t parent_typeid : 17; // 0x0
    uint64_t declaring_typeid : 17; // 0x0
    uint32_t fqn_hash; // 0x8
    uint32_t crc; // 0xc
    char pad_10[0x8];
    uint32_t name_offset; // 0x18
    uint32_t namespace_offset; // 0x1c
    uint32_t type_flags; // 0x20
    uint8_t system_type; // 0x24
    char pad_25[0x1];
    uint8_t object_type; // 0x26
    char pad_27[0x1];
    uint32_t default_ctor; // 0x28
    uint32_t element_size; // 0x2c
    uint32_t size; // 0x30
    char pad_34[0x4];
    uint32_t member_method; // 0x38
    uint32_t num_member_method; // 0x3c
    uint32_t member_field; // 0x40
    uint32_t num_member_field; // 0x44
    uint32_t num_member_prop : 12; // 0x48
    uint32_t member_prop : 19; // 0x48
    char pad_4c[0x4];
    uint32_t interfaces; // 0x50
    char pad_54[0x4];
    uint32_t generics; // 0x58
    uint32_t vt; // 0x5c
    char pad_60[0x10];
    regenny::via::typeinfo::TypeInfo* type; // 0x70
    regenny::ManagedVtable* managed_vt; // 0x78
}; // Size: 0x80
#pragma pack(pop)
}
