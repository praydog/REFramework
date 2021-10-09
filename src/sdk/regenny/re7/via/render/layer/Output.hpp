#pragma once
#include "..\RenderLayer.hpp"
namespace regenny::via::render {
struct OutputTargetStateDX12;
}
namespace regenny::via::render::layer {
#pragma pack(push, 1)
struct Output : public regenny::via::render::RenderLayer {
    regenny::via::render::OutputTargetStateDX12* state; // 0x78
    char pad_80[0xf80];
}; // Size: 0x1000
#pragma pack(pop)
}
