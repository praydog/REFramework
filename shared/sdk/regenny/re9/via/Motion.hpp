#pragma once
#include "JointMap.hpp"
namespace regenny::via {
#pragma pack(push, 1)
struct Motion {
    private: char pad_0[0x200]; public:
    regenny::via::JointMap jointmap; // 0x200
    private: char pad_210[0xdf0]; public:
}; // Size: 0x1000
#pragma pack(pop)
}
