#pragma once
namespace regenny::tdb49 {
#pragma pack(push, 1)
struct TypeDefinition {
    uint32_t unk_data : 16; // 0x0
    uint32_t unk_data2 : 16; // 0x0
    uint16_t parent_typeid; // 0x4
    uint16_t declaring_typeid; // 0x6
    char pad_8[0xa];
    uint16_t something; // 0x12
    uint32_t name_offset; // 0x14
    uint32_t namespace_offset; // 0x18
    uint32_t type_flags; // 0x1c
    uint16_t system_type; // 0x20
    uint8_t object_type; // 0x22
    char pad_23[0x5];
    uint16_t num_member_method; // 0x28
    uint16_t num_member_field; // 0x2a
    uint16_t num_member_prop; // 0x2c
    uint16_t num_virtual_method; // 0x2e
    uint16_t num_virtual_field; // 0x30
    char pad_32[0x2];
    uint32_t element_size; // 0x34
    uint32_t virtual_field_start; // 0x38
    char pad_3c[0x4];
    uint32_t member_method; // 0x40
    uint32_t member_field; // 0x44
    uint32_t member_prop; // 0x48
    uint32_t virtual_method_start; // 0x4c
    char pad_50[0x10];
}; // Size: 0x60
#pragma pack(pop)
}
