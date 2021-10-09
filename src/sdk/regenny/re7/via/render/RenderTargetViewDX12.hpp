#pragma once
#include ".\RenderResource.hpp"
namespace regenny::via::render {
struct OutputTargetStateDX12;
}
namespace regenny::via::render {
#pragma pack(push, 1)
struct RenderTargetViewDX12 : public RenderResource {
    char pad_10[0x70];
    regenny::via::render::OutputTargetStateDX12* output_state; // 0x80
    char pad_88[0x78];
}; // Size: 0x100
#pragma pack(pop)
}
