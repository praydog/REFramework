#pragma once
#include "ChainCollisionArray.hpp"
namespace regenny::via::motion {
#pragma pack(push, 1)
struct Chain {
    char pad_0[0x60];
    regenny::via::motion::ChainCollisionArray CollisionData; // 0x60
    char pad_74[0x78];
    float BlendRate; // 0xec
    float FreezeRate; // 0xf0
    char pad_f4[0x3c];
    float StepFrame; // 0x130
    char pad_134[0xd4];
    uint32_t CalculateMode; // 0x208
    char pad_20c[0x4];
    uint32_t GravityCoord; // 0x210
}; // Size: 0x214
#pragma pack(pop)
}
