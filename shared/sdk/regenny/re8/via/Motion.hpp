#pragma once
#include ".\JointMap.hpp"
namespace regenny::via {
#pragma pack(push, 1)
struct Motion {
    char pad_0[0x200];
    regenny::via::JointMap jointmap; // 0x200
    char pad_210[0xdf0];
}; // Size: 0x1000
#pragma pack(pop)
}
