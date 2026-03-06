#pragma once
namespace regenny::via::render {
struct RenderLayer;
}
namespace regenny::via::render {
#pragma pack(push, 1)
struct LayerArray {
    regenny::via::render::RenderLayer** elements; // 0x0
    uint32_t num; // 0x8
    uint32_t num_allocated; // 0xc
}; // Size: 0x10
#pragma pack(pop)
}
