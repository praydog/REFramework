#pragma once
namespace regenny::tdb71 {
#pragma pack(push, 1)
struct MethodImpl {
    uint16_t attributes_id; // 0x0
    int16_t vtable_index; // 0x2
    uint16_t flags; // 0x4
    uint16_t impl_flags; // 0x6
    uint32_t name_offset; // 0x8
}; // Size: 0xc
#pragma pack(pop)
}
