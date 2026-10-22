#pragma once
#include "Texture.hpp"
namespace regenny::via::render {
struct DXResource;
}
namespace regenny::via::render {
#pragma pack(push, 1)
struct TextureDX12 : public Texture {
    char pad_70[0x2];
    uint16_t index; // 0x72
    char pad_74[0xc];
    regenny::via::render::DXResource* unk_resource; // 0x80
    uint32_t last_frame; // 0x88
    uint32_t locked_thing; // 0x8c
    uint32_t scale; // 0x90
    bool b1; // 0x94
    bool b2; // 0x95
    char pad_96[0xa];
    regenny::via::render::DXResource* resource; // 0xa0
    char pad_a8[0xf58];
}; // Size: 0x1000
#pragma pack(pop)
}
