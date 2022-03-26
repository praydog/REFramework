#pragma once
namespace regenny::via::gui {
#pragma pack(push, 1)
struct PrimitiveContext {
    char pad_0[0xec];
    int32_t gui_camera; // 0xec
    char pad_f0[0x210];
}; // Size: 0x300
#pragma pack(pop)
}
