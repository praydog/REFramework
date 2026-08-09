#pragma once
#include "..\RenderLayer.hpp"
namespace regenny {
struct BullShit;
}
namespace regenny::via::via::render {
struct TargetState;
}
namespace regenny::via::via::render::layer {
#pragma pack(push, 1)
struct PrepareOutput : public regenny::via::via::render::RenderLayer {
    private: char pad_88[0x18]; public:
    regenny::BullShit* asdf; // 0xa0
    private: char pad_a8[0x80]; public:
    regenny::via::via::render::TargetState* output_state; // 0x128
    private: char pad_130[0xed0]; public:
}; // Size: 0x1000
#pragma pack(pop)
}
