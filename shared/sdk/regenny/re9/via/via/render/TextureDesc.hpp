#pragma once
#include "TextureFormat.hpp"
#include "TextureStreamingType.hpp"
#include "UsageType.hpp"
namespace regenny::via::via::render {
#pragma pack(push, 1)
struct TextureDesc {
    uint32_t width; // 0x0
    uint32_t height; // 0x4
    uint32_t depth; // 0x8
    uint32_t mip; // 0xc
    uint32_t arr; // 0x10
    regenny::via::via::render::TextureFormat format; // 0x14
    uint32_t sample_desc_count; // 0x18
    uint32_t sample_desc_quality; // 0x1c
    regenny::via::via::render::UsageType usage_type; // 0x20
    uint32_t bind_flags; // 0x24
    uint32_t option_flags; // 0x28
    regenny::via::via::render::TextureStreamingType streaming_type; // 0x2c
    uint32_t unk; // 0x30
    private: char pad_34[0x4]; public:
    uint64_t alloc_thing; // 0x38
    uint32_t tile; // 0x40
    private: char pad_44[0x4]; public:
    void* unkptr; // 0x48
}; // Size: 0x50
#pragma pack(pop)
}
