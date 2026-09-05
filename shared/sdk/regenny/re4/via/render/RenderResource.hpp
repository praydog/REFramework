#pragma once
namespace regenny::via::render {
struct Poop;
}
namespace regenny::via::render {
#pragma pack(push, 1)
struct RenderResource {
    regenny::via::render::Poop* unkpoop; // 0x0
    int32_t ref_count; // 0x8
    uint32_t render_frame; // 0xc
}; // Size: 0x10
#pragma pack(pop)
}
