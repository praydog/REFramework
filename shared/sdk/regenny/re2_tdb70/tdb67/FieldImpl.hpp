#pragma once
namespace regenny::tdb67 {
#pragma pack(push, 1)
struct FieldImpl {
    uint16_t attributes_id; // 0x0
    uint16_t flags; // 0x2
    uint32_t field_typeid : 18; // 0x4
    uint32_t init_data_lo : 14; // 0x4
    uint32_t name_offset : 30; // 0x8
    uint32_t init_data_hi : 2; // 0x8
}; // Size: 0xc
#pragma pack(pop)
}
