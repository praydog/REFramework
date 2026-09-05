#pragma once
#include "RenderTargetView.hpp"
namespace regenny::via::render {
struct TextureDX12;
}
namespace regenny::via::render {
#pragma pack(push, 1)
struct RenderTargetViewDX12 : public RenderTargetView {
    void* output_target; // 0x90
    regenny::via::render::TextureDX12* tex; // 0x98
    char pad_a0[0x60];
}; // Size: 0x100
#pragma pack(pop)
}
