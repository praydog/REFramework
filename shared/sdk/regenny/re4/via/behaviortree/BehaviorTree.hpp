#pragma once
#include "..\Component.hpp"
#include "BehaviorTreeCoreHandleArray.hpp"
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct BehaviorTree : public regenny::via::Component {
    char pad_30[0x68];
    regenny::via::behaviortree::BehaviorTreeCoreHandleArray trees; // 0x98
}; // Size: 0xa4
#pragma pack(pop)
}
