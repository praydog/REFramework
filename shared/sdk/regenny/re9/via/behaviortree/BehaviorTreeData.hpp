#pragma once
#include "TreeObjectData.hpp"
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct BehaviorTreeData {
    void* vftable; // 0x0
    bool unk_bool; // 0x8
    private: char pad_9[0x7]; public:
    regenny::via::behaviortree::TreeObjectData data; // 0x10
}; // Size: 0x310
#pragma pack(pop)
}
