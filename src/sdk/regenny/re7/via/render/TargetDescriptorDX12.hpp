#pragma once
#include "..\Size.hpp"
namespace regenny::via::render {
struct RenderTargetViewDX12;
}
namespace regenny::via::render {
#pragma pack(push, 1)
struct TargetDescriptorDX12 {
    regenny::via::render::RenderTargetViewDX12** render_targets; // 0x0
    void* depth_stencil; // 0x8
    uint32_t num_render_targets; // 0x10
    char pad_14[0x8];
    regenny::via::Size viewport; // 0x1c
    uint32_t flag; // 0x24
}; // Size: 0x28
#pragma pack(pop)
}
