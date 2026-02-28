#pragma once
#include "ChainCollisionArray.hpp"
#include "SecondaryAnimation.hpp"
namespace regenny::via::motion {
#pragma pack(push, 1)
struct Chain : public SecondaryAnimation {
    private: char pad_50[0xf0]; public:
    regenny::via::motion::ChainCollisionArray CollisionData; // 0x140
    private: char pad_154[0xd8]; public:
    float BlendRate; // 0x22c
    float FreezeRate; // 0x230
    private: char pad_234[0x4c]; public:
    float StepFrame; // 0x280
    private: char pad_284[0xf4]; public:
    uint32_t CalculateMode; // 0x378
    private: char pad_37c[0x4]; public:
    uint32_t GravityCoord; // 0x380
    private: char pad_384[0xc]; public:
}; // Size: 0x390
#pragma pack(pop)
}
