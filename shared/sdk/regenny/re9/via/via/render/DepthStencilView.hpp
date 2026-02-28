#pragma once
#include "RenderResource.hpp"
namespace regenny::via::via::render {
#pragma pack(push, 1)
struct DepthStencilView : public RenderResource {
    uint32_t format; // 0x18
}; // Size: 0x1c
#pragma pack(pop)
}
