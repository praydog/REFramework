#pragma once
#include ".\TargetDescriptorDX12.hpp"
#include ".\OutputTargetState.hpp"
namespace regenny::via::render {
#pragma pack(push, 1)
struct OutputTargetStateDX12 : public OutputTargetState {
    char pad_10[0x8];
    regenny::via::render::TargetDescriptorDX12 desc; // 0x18
    uint32_t hash; // 0x40
    char pad_44[0xa4];
    void* re_engine_device; // 0xe8
    void* swapchains[2]; // 0xf0
    void* backbuffers[3]; // 0x100
    void* descriptors[3]; // 0x118
    int32_t current_index; // 0x130
    uint32_t frame; // 0x134
    char pad_138[0xc8];
}; // Size: 0x200
#pragma pack(pop)
}
