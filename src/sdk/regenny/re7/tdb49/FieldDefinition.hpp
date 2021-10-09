#pragma once
namespace regenny::tdb49 {
#pragma pack(push, 1)
struct FieldDefinition {
    uint64_t declaring_typeid : 16; // 0x0
    uint64_t field_typeid : 16; // 0x0
    uint32_t name_offset; // 0x8
    uint16_t flags; // 0xc
    char pad_e[0x2];
    uint16_t init_data_index; // 0x10
    char pad_12[0x2];
    uint32_t offset; // 0x14
    char pad_18[0x4];
}; // Size: 0x1c
#pragma pack(pop)
}
