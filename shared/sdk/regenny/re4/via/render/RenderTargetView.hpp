#pragma once
#include "RenderResource.hpp"
namespace regenny::via::render {
#pragma pack(push, 1)
struct RenderTargetView : public RenderResource {
    uint32_t format; // 0x10
    uint32_t dimension; // 0x14
    char pad_18[0x78];
}; // Size: 0x90
#pragma pack(pop)
}
