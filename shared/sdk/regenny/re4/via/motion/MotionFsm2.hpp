#pragma once
#include "..\behaviortree\BehaviorTree.hpp"
namespace regenny::via::motion {
#pragma pack(push, 1)
struct MotionFsm2 : public regenny::via::behaviortree::BehaviorTree {
    char pad_a4[0x5c];
}; // Size: 0x100
#pragma pack(pop)
}
