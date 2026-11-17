#pragma once
#include "RenderResource.hpp"
namespace regenny::via::render {
#pragma pack(push, 1)
struct DepthStencilView : public RenderResource {
    uint32_t format; // 0x10
    char pad_14[0x4];
}; // Size: 0x18
#pragma pack(pop)
}
