#pragma once
#include "RenderResource.hpp"
namespace regenny::via::via::render {
#pragma pack(push, 1)
struct RenderTargetView : public RenderResource {
    uint32_t format; // 0x18
    uint32_t dimension; // 0x1c
    private: char pad_20[0x88]; public:
}; // Size: 0xa8
#pragma pack(pop)
}
