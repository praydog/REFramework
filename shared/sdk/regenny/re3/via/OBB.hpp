#pragma once
#include ".\mat4.hpp"
#include ".\vec3.hpp"
namespace regenny::via {
#pragma pack(push, 1)
struct OBB {
    regenny::via::mat4 coord; // 0x0
    regenny::via::vec3 extent; // 0x40
    char pad_4c[0x4];
}; // Size: 0x50
#pragma pack(pop)
}
