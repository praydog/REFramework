#pragma once
namespace regenny::via::motion {
struct ChainCollisions;
}
namespace regenny::via {
struct Transform;
}
namespace regenny::via::motion {
#pragma pack(push, 1)
struct ChainCollisionTop {
    regenny::via::motion::ChainCollisions* collisions; // 0x0
    uint32_t num_collisions; // 0x8
    uint32_t allocated_collisions; // 0xc
    regenny::via::Transform* owner_transform; // 0x10
    regenny::via::Transform* parent_transform; // 0x18
    float scale; // 0x20
    float parent_scale; // 0x24
    char pad_28[0xd8];
}; // Size: 0x100
#pragma pack(pop)
}
