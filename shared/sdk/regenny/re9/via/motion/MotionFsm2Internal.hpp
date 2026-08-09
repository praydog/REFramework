#pragma once
#include "..\behaviortree\BehaviorTreeData.hpp"
namespace regenny::via::motion {
struct MotionFsm2RawData;
}
namespace regenny::via::motion {
#pragma pack(push, 1)
struct MotionFsm2Internal {
    void* vftable; // 0x0
    regenny::via::motion::MotionFsm2RawData* data; // 0x8
    private: char pad_10[0x8]; public:
    regenny::via::behaviortree::BehaviorTreeData bhvt; // 0x18
}; // Size: 0x328
#pragma pack(pop)
}
