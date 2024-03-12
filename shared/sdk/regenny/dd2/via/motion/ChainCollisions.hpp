#pragma once
#include "..\Capsule.hpp"
#include "..\Sphere.hpp"
#include "..\vec4.hpp"
namespace regenny::via {
struct Joint;
}
namespace regenny::via::motion {
struct ChainCollisionData;
}
namespace regenny::via::motion {
struct ChainCollisionTop;
}
namespace regenny::via::motion {
#pragma pack(push, 1)
struct ChainCollisions {
    uint32_t order; // 0x0
    private: char pad_4[0xc]; public:
    regenny::via::Sphere sphere; // 0x10
    regenny::via::Capsule capsule; // 0x20
    private: char pad_50[0x80]; public:
    regenny::via::vec4 offset; // 0xd0
    regenny::via::vec4 pair_offset; // 0xe0
    private: char pad_f0[0x40]; public:
    regenny::via::Joint* joint; // 0x130
    regenny::via::Joint* pair_joint; // 0x138
    regenny::via::motion::ChainCollisionData* data; // 0x140
    regenny::via::motion::ChainCollisionTop* owner; // 0x148
    float radius; // 0x150
    float radius2; // 0x154
    int32_t d; // 0x158
    float lerp; // 0x15c
    int32_t pad2; // 0x160
    uint32_t flags; // 0x164
    float scale; // 0x168
    bool valid; // 0x16c
    private: char pad_16d[0x13]; public:
}; // Size: 0x180
#pragma pack(pop)
}
