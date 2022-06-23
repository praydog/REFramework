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
struct ChainCollisionArray;
}
namespace regenny::via::motion {
#pragma pack(push, 1)
struct ChainCollisions {
    regenny::via::Sphere sphere; // 0x0
    regenny::via::Capsule capsule; // 0x10
    char pad_40[0x50];
    regenny::via::vec4 offset; // 0x90
    regenny::via::vec4 pair_offset; // 0xa0
    char pad_b0[0x40];
    regenny::via::Joint* joint; // 0xf0
    regenny::via::Joint* pair_joint; // 0xf8
    regenny::via::motion::ChainCollisionData* data; // 0x100
    regenny::via::motion::ChainCollisionArray* owner; // 0x108
    float radius; // 0x110
    float radius2; // 0x114
    int32_t d; // 0x118
    int32_t pad1; // 0x11c
    int32_t pad2; // 0x120
    uint32_t flags; // 0x124
    char pad_128[0x18];
}; // Size: 0x140
#pragma pack(pop)
}
