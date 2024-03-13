#pragma once
namespace regenny::via::typeinfo {
struct TypeInfo;
}
namespace regenny::tdb71 {
#pragma pack(push, 1)
struct TypeDefinition {
    uint64_t index : 19; // 0x0
    uint64_t parent_typeid : 19; // 0x0
    uint64_t declaring_typeid : 19; // 0x0
    uint64_t underlying_typeid : 7; // 0x0
    uint64_t array_typeid : 19; // 0x8
    uint64_t element_typeid : 19; // 0x8
    uint64_t impl_index : 18; // 0x8
    uint64_t system_typeid : 7; // 0x8
    private: uint64_t pad_bitfield_8_3f : 1; public:
    uint32_t type_flags; // 0x10
    uint32_t size; // 0x14
    uint32_t fqn_hash; // 0x18
    uint32_t type_crc; // 0x1c
    uint64_t default_ctor : 22; // 0x20
    uint64_t member_method : 22; // 0x20
    uint64_t member_field : 19; // 0x20
    private: uint64_t pad_bitfield_20_3f : 1; public:
    uint32_t num_member_prop : 12; // 0x28
    uint32_t member_prop : 19; // 0x28
    private: uint32_t pad_bitfield_28_1f : 1; public:
    uint32_t unk_data : 26; // 0x2c
    uint32_t object_type : 3; // 0x2c
    private: uint32_t pad_bitfield_2c_1d : 3; public:
    int64_t unk_data_before_generics : 26; // 0x30
    int64_t generics : 26; // 0x30
    int64_t interfaces : 12; // 0x30
    regenny::via::typeinfo::TypeInfo* type; // 0x38
    void** managed_vt; // 0x40
}; // Size: 0x48
#pragma pack(pop)
}
