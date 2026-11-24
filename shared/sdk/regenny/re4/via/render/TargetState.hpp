#pragma once
#include "RenderResource.hpp"
namespace regenny::via::render {
struct RenderTargetViewDX12;
}
namespace regenny::via::render {
struct DepthStencilViewDX12;
}
namespace regenny::via::render {
#pragma pack(push, 1)
struct TargetState : public RenderResource {
    regenny::via::render::RenderTargetViewDX12** rtvs; // 0x10
    regenny::via::render::DepthStencilViewDX12* ds; // 0x18
    uint32_t num_rtv; // 0x20
    float left; // 0x24
    float right; // 0x28
    float top; // 0x2c
    float bottom; // 0x30
    uint32_t flag; // 0x34
    char pad_38[0xfc8];
}; // Size: 0x1000
#pragma pack(pop)
}
