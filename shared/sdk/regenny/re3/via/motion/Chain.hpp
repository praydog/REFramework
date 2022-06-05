#pragma once
#include ".\ChainCollisionArray.hpp"
#include ".\SecondaryAnimation.hpp"
namespace regenny::via::motion {
#pragma pack(push, 1)
struct Chain : public SecondaryAnimation {
    char pad_48[0x10];
    regenny::via::motion::ChainCollisionArray CollisionData; // 0x58
    char pad_6c[0x80];
    float BlendRate; // 0xec
    float FreezeRate; // 0xf0
    char pad_f4[0x3c];
    float StepFrame; // 0x130
    char pad_134[0xd4];
    uint32_t CalculateMode; // 0x208
    char pad_20c[0x4];
    uint32_t GravityCoord; // 0x210
    char pad_214[0xc];
}; // Size: 0x220
#pragma pack(pop)
}
