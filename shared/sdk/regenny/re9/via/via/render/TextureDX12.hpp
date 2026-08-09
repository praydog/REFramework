#pragma once
#include "Texture.hpp"
namespace regenny::via::via::render {
struct DXResource;
}
namespace regenny::via::via::render {
#pragma pack(push, 1)
struct TextureDX12 : public Texture {
    private: char pad_90[0x10]; public:
    uint32_t last_frame; // 0xa0
    uint32_t locked_thing; // 0xa4
    uint32_t scale; // 0xa8
    bool b1; // 0xac
    bool b2; // 0xad
    private: char pad_ae[0x32]; public:
    regenny::via::via::render::DXResource* resource; // 0xe0
    private: char pad_e8[0xf18]; public:
}; // Size: 0x1000
#pragma pack(pop)
}
