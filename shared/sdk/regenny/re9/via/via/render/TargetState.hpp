#pragma once
#include "RenderResource.hpp"
namespace regenny::via::via::render {
struct RenderTargetViewDX12;
}
namespace regenny::via::via::render {
struct DepthStencilViewDX12;
}
namespace regenny::via::via::render {
#pragma pack(push, 1)
struct TargetState : public RenderResource {
    regenny::via::via::render::RenderTargetViewDX12** rtvs; // 0x18
    regenny::via::via::render::DepthStencilViewDX12* ds; // 0x20
    uint32_t num_rtv; // 0x28
    float left; // 0x2c
    float right; // 0x30
    float top; // 0x34
    float bottom; // 0x38
    uint32_t flag; // 0x3c
    private: char pad_40[0xfc0]; public:
}; // Size: 0x1000
#pragma pack(pop)
}
