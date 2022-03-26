#pragma once
namespace regenny::tdb49 {
#pragma pack(push, 1)
struct PropertyDefinition {
    uint16_t declaring_typeid; // 0x0
    char pad_2[0x6];
    uint32_t name_offset; // 0x8
    uint32_t getter; // 0xc
    uint32_t setter; // 0x10
}; // Size: 0x14
#pragma pack(pop)
}
