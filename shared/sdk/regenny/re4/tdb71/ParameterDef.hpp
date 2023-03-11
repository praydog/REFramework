#pragma once
namespace regenny::tdb71 {
#pragma pack(push, 1)
struct ParameterDef {
    uint16_t attributes_id; // 0x0
    uint16_t init_data_index; // 0x2
    uint32_t name_offset : 30; // 0x4
    uint32_t modifier : 2; // 0x4
    uint32_t type_id : 18; // 0x8
    uint32_t flags : 14; // 0x8
}; // Size: 0xc
#pragma pack(pop)
}
