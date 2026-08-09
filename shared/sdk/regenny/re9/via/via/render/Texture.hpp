#pragma once
#include "RenderResource.hpp"
#include "TextureDesc.hpp"
#include "UsageType.hpp"
namespace regenny::via::via::render {
#pragma pack(push, 1)
struct Texture : public RenderResource {
    regenny::via::via::render::UsageType usage_type; // 0x18
    private: char pad_1c[0x14]; public:
    regenny::via::via::render::TextureDesc desc; // 0x30
    uint32_t vram; // 0x80
    uint32_t pad_; // 0x84
    private: char pad_88[0x8]; public:
}; // Size: 0x90
#pragma pack(pop)
}
