#pragma once
#include "..\Object.hpp"
namespace regenny::via::render {
#pragma pack(push, 1)
struct RenderResource : public regenny::via::Object {
    int32_t ref_count; // 0x8
    uint32_t render_frame; // 0xc
}; // Size: 0x10
#pragma pack(pop)
}
