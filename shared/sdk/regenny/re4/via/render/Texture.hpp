#pragma once
#include "RenderResource.hpp"
#include "TextureDesc.hpp"
#include "UsageType.hpp"
namespace regenny::via::render {
#pragma pack(push, 1)
struct Texture : public RenderResource {
    regenny::via::render::UsageType usage_type; // 0x10
    char pad_14[0x4];
    regenny::via::render::TextureDesc desc; // 0x18
    uint32_t vram; // 0x68
    uint32_t pad_; // 0x6c
}; // Size: 0x70
#pragma pack(pop)
}
