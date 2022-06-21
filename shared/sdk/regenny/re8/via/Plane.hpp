#pragma once
#include "vec3.hpp"
namespace regenny::via {
#pragma pack(push, 1)
struct Plane {
    regenny::via::vec3 normal; // 0x0
    float dist; // 0xc
}; // Size: 0x10
#pragma pack(pop)
}
