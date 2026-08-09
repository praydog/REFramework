#pragma once
#include "DepthStencilView.hpp"
namespace regenny::via::via::render {
struct TextureDX12;
}
namespace regenny::via::via::render {
#pragma pack(push, 1)
struct DepthStencilViewDX12 : public DepthStencilView {
    uint32_t format2; // 0x1c
    private: char pad_20[0x10]; public:
    regenny::via::via::render::TextureDX12* tex; // 0x30
    private: char pad_38[0xc8]; public:
}; // Size: 0x100
#pragma pack(pop)
}
