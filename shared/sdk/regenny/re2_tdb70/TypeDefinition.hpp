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
    uint64_t index : 18; // 0x0
    uint64_t parent_typeid : 18; // 0x0
    uint64_t declaring_typeid : 18; // 0x0
    uint64_t underlying_typeid : 7; // 0x0
    uint64_t object_type : 3; // 0x0
    uint64_t array_typeid : 18; // 0x8
    uint64_t element_typeid : 18; // 0x8
    uint64_t impl_index : 18; // 0x8
    uint64_t system_typeid : 10; // 0x8
    uint32_t type_flags; // 0x10
    uint32_t size; // 0x14
    uint32_t fqn_hash; // 0x18
    uint32_t type_crc; // 0x1c
    uint32_t default_ctor; // 0x20
    uint32_t vt; // 0x24
    uint32_t member_method; // 0x28
    uint32_t member_field; // 0x2c
    uint32_t num_member_prop : 12; // 0x30
    uint32_t member_prop : 19; // 0x30
    uint32_t pad_bitfield_30_1f : 1;
    uint32_t member_event; // 0x34
    int32_t interfaces; // 0x38
    int32_t generics; // 0x3c
    regenny::via::typeinfo::TypeInfo* type; // 0x40
    regenny::ManagedVtable* managed_vt; // 0x48
    char pad_50[0x30];
}; // Size: 0x80
#pragma pack(pop)
}
