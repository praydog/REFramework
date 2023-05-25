#pragma once
#include "..\RenderLayer.hpp"
namespace regenny::via::render {
struct TargetState;
}
namespace regenny::via::render::layer {
#pragma pack(push, 1)
struct PrepareOutput : public regenny::via::render::RenderLayer {
    char pad_88[0x80];
    regenny::via::render::TargetState* output_state; // 0x108
    char pad_110[0xef0];
}; // Size: 0x1000
#pragma pack(pop)
}
