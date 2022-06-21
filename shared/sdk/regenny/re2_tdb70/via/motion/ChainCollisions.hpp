#pragma once
#include "..\Capsule.hpp"
#include "..\OBB.hpp"
#include "..\Plane.hpp"
#include "..\Sphere.hpp"
#include "..\vec4.hpp"
namespace regenny::via {
struct Joint;
}
namespace regenny::via::motion {
struct ChainCollisionData;
}
namespace regenny::via::motion {
struct ChainCollisionArray;
}
namespace regenny::via::motion {
#pragma pack(push, 1)
struct ChainCollisions {
    uint32_t order; // 0x0
    char pad_4[0xc];
    regenny::via::Sphere sphere; // 0x10
    regenny::via::Capsule capsule; // 0x20
    regenny::via::OBB obb; // 0x50
    regenny::via::Plane plane; // 0x9c
    char pad_ac[0x24];
    regenny::via::vec4 offset; // 0xd0
    regenny::via::vec4 pair_offset; // 0xe0
    char pad_f0[0x40];
    regenny::via::Joint* joint; // 0x130
    regenny::via::Joint* pair_joint; // 0x138
    regenny::via::motion::ChainCollisionData* data; // 0x140
    regenny::via::motion::ChainCollisionArray* owner; // 0x148
    float radius; // 0x150
    float radius2; // 0x154
    int32_t d; // 0x158
    float lerp; // 0x15c
    int32_t pad2; // 0x160
    uint32_t flags; // 0x164
    float scale; // 0x168
    bool valid; // 0x16c
    char pad_16d[0x13];
}; // Size: 0x180
#pragma pack(pop)
}
