#pragma once
#include "..\vec3.hpp"
namespace regenny::via::motion {
#pragma pack(push, 1)
struct ChainCollisionData {
    void* sub_data; // 0x0
    regenny::via::vec3 pos; // 0x8
    regenny::via::vec3 pair_pos; // 0x14
    uint32_t joint_hash; // 0x20
    uint32_t pair_joint_hash; // 0x24
    float radius; // 0x28
    float lerp; // 0x2c
    uint8_t shape_type; // 0x30
    uint8_t div; // 0x31
    uint8_t num_sub_data; // 0x32
    private: char pad_33[0x1]; public:
    uint32_t flags; // 0x34
    uint32_t unk; // 0x38
}; // Size: 0x3c
#pragma pack(pop)
}
