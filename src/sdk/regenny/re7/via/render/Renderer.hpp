#pragma once
#include "..\JobSet.hpp"
#include "..\Object.hpp"
namespace regenny::via::render {
#pragma pack(push, 1)
struct Renderer : public regenny::via::Object {
    struct FrameContext {
        uint32_t frame_count; // 0x0
        char pad_4[0xfc];
    }; // Size: 0x100

    char pad_8[0xf78];
    regenny::via::JobSet before_rendering_task; // 0xf80
    char pad_f88[0x78];
    regenny::via::JobSet end_task; // 0x1000
    char pad_1008[0x3ff8];
}; // Size: 0x5000
#pragma pack(pop)
}
