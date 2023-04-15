#pragma once
#include "ChainCollisionArray.hpp"
#include "SecondaryAnimation.hpp"
namespace regenny::via::motion {
#pragma pack(push, 1)
struct Chain : public SecondaryAnimation {
    char pad_50[0xf0];
    regenny::via::motion::ChainCollisionArray CollisionData; // 0x140
    char pad_154[0xd8];
    float BlendRate; // 0x22c
    float FreezeRate; // 0x230
    char pad_234[0x4c];
    float StepFrame; // 0x280
    char pad_284[0xf4];
    uint32_t CalculateMode; // 0x378
    char pad_37c[0x4];
    uint32_t GravityCoord; // 0x380
    char pad_384[0xc];
}; // Size: 0x390
#pragma pack(pop)
}
