#pragma once
namespace regenny::via::via::render {
struct Poop;
}
namespace regenny::via::via::render {
#pragma pack(push, 1)
struct RenderResource {
    regenny::via::via::render::Poop* unkpoop; // 0x0
    int32_t ref_count; // 0x8
    uint32_t render_frame; // 0xc
    private: char pad_10[0x8]; public:
}; // Size: 0x18
#pragma pack(pop)
}
