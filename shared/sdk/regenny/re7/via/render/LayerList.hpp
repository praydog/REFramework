#pragma once
namespace regenny::via::render {
struct RenderLayer;
}
namespace regenny::via::render {
#pragma pack(push, 1)
struct LayerList {
    regenny::via::render::RenderLayer** layers; // 0x0
    uint32_t num_layers; // 0x8
    uint32_t allocated_layers; // 0xc
}; // Size: 0x10
#pragma pack(pop)
}
