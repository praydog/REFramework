#pragma once
namespace regenny::via::motion {
struct ChainCollisionTop;
}
namespace regenny::via::motion {
#pragma pack(push, 1)
struct ChainCollisionArray {
    regenny::via::motion::ChainCollisionTop* collisions; // 0x0
    int32_t num; // 0x8
    int32_t num_allocated; // 0xc
    int32_t preset_id; // 0x10
}; // Size: 0x14
#pragma pack(pop)
}
