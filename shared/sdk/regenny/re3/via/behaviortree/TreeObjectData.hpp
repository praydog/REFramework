#pragma once
#include "ManagedObjectArray.hpp"
namespace regenny::via::behaviortree {
struct TreeNodeData;
}
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct TreeObjectData {
    void* vftable; // 0x0
    regenny::via::behaviortree::TreeNodeData* nodes; // 0x8
    uint32_t node_count; // 0x10
    char pad_14[0x4];
    regenny::via::behaviortree::ManagedObjectArray static_selector_callers; // 0x18
    regenny::via::behaviortree::ManagedObjectArray static_actions; // 0x28
    regenny::via::behaviortree::ManagedObjectArray static_conditions; // 0x38
    regenny::via::behaviortree::ManagedObjectArray static_transitions; // 0x48
    regenny::via::behaviortree::ManagedObjectArray expression_tree_conditions; // 0x58
    char pad_68[0x298];
}; // Size: 0x300
#pragma pack(pop)
}
