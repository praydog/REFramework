#pragma once
#include ".\RenderResource.hpp"
namespace regenny::via::render {
#pragma pack(push, 1)
struct RenderTargetViewDX11 : public RenderResource {
    char pad_10[0x78];
    void* d3d11_resource; // 0x88
    char pad_90[0x70];
}; // Size: 0x100
#pragma pack(pop)
}
