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
    char pad_40[0x20];
    regenny::via::vec4 offset; // 0x60
    regenny::via::vec4 pair_offset; // 0x70
    char pad_80[0x20];
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
