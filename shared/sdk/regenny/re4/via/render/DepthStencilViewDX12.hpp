#pragma once
#include "DepthStencilView.hpp"
namespace regenny::via::render {
struct TextureDX12;
}
namespace regenny::via::render {
#pragma pack(push, 1)
struct DepthStencilViewDX12 : public DepthStencilView {
    uint32_t format2; // 0x18
    char pad_1c[0x14];
    regenny::via::render::TextureDX12* tex; // 0x30
    char pad_38[0xc8];
}; // Size: 0x100
#pragma pack(pop)
}
