#pragma once
#include ".\TargetDescriptorDX11.hpp"
#include ".\OutputTargetState.hpp"
namespace regenny::via::render {
#pragma pack(push, 1)
struct OutputTargetStateDX11 : public OutputTargetState {
    char pad_10[0x8];
    regenny::via::render::TargetDescriptorDX11 desc; // 0x18
    uint32_t hash; // 0x40
    char pad_44[0x84];
    void* swapchain; // 0xc8
    void* device; // 0xd0
}; // Size: 0xd8
#pragma pack(pop)
}
