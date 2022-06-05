#pragma once
#include "..\Sphere.hpp"
#include "..\Capsule.hpp"
#include "..\OBB.hpp"
namespace regenny::via::motion {
struct ChainCollisionData;
}
namespace regenny::via {
struct Joint;
}
namespace regenny::via::motion {
struct ChainCollisionArray;
}
namespace regenny::via::motion {
#pragma pack(push, 1)
struct ChainCollisions {
    regenny::via::Sphere sphere; // 0x0
    regenny::via::Capsule capsule; // 0x10
    regenny::via::OBB obb; // 0x40
    char pad_90[0x10];
    regenny::via::Joint* joint; // 0xa0
    regenny::via::Joint* pair_joint; // 0xa8
    regenny::via::motion::ChainCollisionData* data; // 0xb0
    regenny::via::motion::ChainCollisionArray* owner; // 0xb8
    float radius; // 0xc0
    float radius2; // 0xc4
    int32_t d; // 0xc8
    int32_t pad1; // 0xcc
    int32_t pad2; // 0xd0
    uint32_t flags; // 0xd4
    char pad_d8[0x8];
}; // Size: 0xe0
#pragma pack(pop)
}
