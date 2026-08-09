#pragma once
#include "vec4.hpp"
namespace regenny::via {
#pragma pack(push, 1)
struct Capsule {
    regenny::via::vec4 p0; // 0x0
    regenny::via::vec4 p1; // 0x10
    float r; // 0x20
    private: char pad_24[0xc]; public:
}; // Size: 0x30
#pragma pack(pop)
}
