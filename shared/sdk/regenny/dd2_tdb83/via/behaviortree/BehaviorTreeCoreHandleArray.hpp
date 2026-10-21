#pragma once
namespace regenny::via::behaviortree {
struct CoreHandle;
}
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct BehaviorTreeCoreHandleArray {
    regenny::via::behaviortree::CoreHandle** handles; // 0x0
    uint32_t count; // 0x8
}; // Size: 0xc
#pragma pack(pop)
}
