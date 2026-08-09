#pragma once
#include "vec3.hpp"
namespace regenny::via {
#pragma pack(push, 1)
struct vec4 : public vec3 {
    float w; // 0xc
}; // Size: 0x10
#pragma pack(pop)
}
