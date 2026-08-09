#pragma once
#include "..\ManagedObjectArray.hpp"
#include "..\UInt8Array.hpp"
namespace regenny::via::behaviortree {
struct TreeNodeData;
}
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct TreeObjectData {
    void* vftable; // 0x0
    regenny::via::behaviortree::TreeNodeData* nodes; // 0x8
    uint32_t node_count; // 0x10
    private: char pad_14[0x4]; public:
    regenny::via::ManagedObjectArray static_selector_callers; // 0x18
    regenny::via::ManagedObjectArray static_actions; // 0x28
    regenny::via::ManagedObjectArray static_conditions; // 0x38
    regenny::via::ManagedObjectArray static_transitions; // 0x48
    regenny::via::ManagedObjectArray expression_tree_conditions; // 0x58
    regenny::via::ManagedObjectArray actions; // 0x68
    regenny::via::ManagedObjectArray conditions; // 0x78
    regenny::via::ManagedObjectArray transitions; // 0x88
    private: char pad_98[0x60]; public:
    regenny::via::UInt8Array action_methods; // 0xf8
    regenny::via::UInt8Array static_action_methods; // 0x108
    private: char pad_118[0x1e8]; public:
}; // Size: 0x300
#pragma pack(pop)
}
