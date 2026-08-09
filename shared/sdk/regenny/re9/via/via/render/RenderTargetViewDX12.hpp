#pragma once
#include "RenderTargetView.hpp"
namespace regenny::via::via::render {
struct TextureDX12;
}
namespace regenny::via::via::render {
#pragma pack(push, 1)
struct RenderTargetViewDX12 : public RenderTargetView {
    private: char pad_a8[0x20]; public:
    regenny::via::via::render::TextureDX12* tex; // 0xc8
    private: char pad_d0[0x30]; public:
}; // Size: 0x100
#pragma pack(pop)
}
