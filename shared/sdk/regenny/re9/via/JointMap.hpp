#pragma once
namespace regenny::via {
struct JointData;
}
namespace regenny::via {
#pragma pack(push, 1)
struct JointMap {
    regenny::via::JointData* joints; // 0x0
    private: char pad_8[0x4]; public:
    int32_t num_joints; // 0xc
}; // Size: 0x10
#pragma pack(pop)
}
