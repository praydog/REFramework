#pragma once
namespace regenny::via::gui {
#pragma pack(push, 1)
struct PrimitiveContext {
    private: char pad_0[0xec]; public:
    int32_t gui_camera; // 0xec
    private: char pad_f0[0x210]; public:
}; // Size: 0x300
#pragma pack(pop)
}
