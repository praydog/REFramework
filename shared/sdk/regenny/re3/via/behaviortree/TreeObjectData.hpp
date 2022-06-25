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
    regenny::via::behaviortree::ManagedObjectArray actions; // 0x68
    regenny::via::behaviortree::ManagedObjectArray conditions; // 0x78
    regenny::via::behaviortree::ManagedObjectArray transitions; // 0x88
    char pad_98[0x68];
}; // Size: 0x100
#pragma pack(pop)
}
