#pragma once
namespace regenny::via::render {
#pragma pack(push, 1)
struct LightRenderer {
    char pad_0[0x2610];
    uint32_t frame1; // 0x2610
    char pad_2614[0xec];
    uint32_t FlushElapsedFrameCount; // 0x2700
    char pad_2704[0xc4];
    uint32_t frame2; // 0x27c8
    char pad_27cc[0x1c];
    uint32_t frame3; // 0x27e8
    char pad_27ec[0x5344];
}; // Size: 0x7b30
#pragma pack(pop)
}
