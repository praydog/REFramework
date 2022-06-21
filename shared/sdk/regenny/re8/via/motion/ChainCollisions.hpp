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
    char pad_ac[0x4];
    regenny::via::vec4 offset; // 0xB0
    regenny::via::vec4 pair_offset; // 0xbc
    char pad_cc[0x40];
    regenny::via::Joint* joint; // 0x110
    regenny::via::Joint* pair_joint; // 0x118
    regenny::via::motion::ChainCollisionData* data; // 0x120
    regenny::via::motion::ChainCollisionArray* owner; // 0x128
    float radius; // 0x130
    float radius2; // 0x134
    int32_t d; // 0x138
    float lerp; // 0x13c
    int32_t pad2; // 0x140
    uint32_t flags; // 0x144
    char pad_148[0x18];
}; // Size: 0x160
#pragma pack(pop)
}
