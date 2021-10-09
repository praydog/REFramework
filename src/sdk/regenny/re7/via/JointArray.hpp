#pragma once
namespace regenny::via {
struct Joint;
}
namespace regenny::via {
#pragma pack(push, 1)
struct JointArray {
    regenny::via::Joint** joints; // 0x0
    int32_t count; // 0x8
    int32_t num_allocated; // 0xc
}; // Size: 0x10
#pragma pack(pop)
}
