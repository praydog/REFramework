#pragma once
namespace regenny::tdb71 {
#pragma pack(push, 1)
struct FieldImpl {
    uint16_t attributes; // 0x0
    uint16_t unk : 1; // 0x2
    uint16_t flags : 15; // 0x2
    uint32_t offset : 26; // 0x4
    uint32_t init_data_lo : 6; // 0x4
    uint32_t name_offset : 22; // 0x8
    uint32_t init_data_mid : 4; // 0x8
    private: uint32_t pad_bitfield_8_1a : 6; public:
}; // Size: 0xc
#pragma pack(pop)
}
