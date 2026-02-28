#pragma once
#include "vec3.hpp"
namespace regenny::via {
#pragma pack(push, 1)
struct Sphere {
    regenny::via::vec3 pos; // 0x0
    float r; // 0xc
}; // Size: 0x10
#pragma pack(pop)
}
