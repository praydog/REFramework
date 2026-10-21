#pragma once
#include "NodeStatus.hpp"
namespace regenny::via::behaviortree {
struct TreeNodeData;
}
namespace regenny::via::behaviortree {
struct TreeObject;
}
namespace regenny::via::behaviortree {
struct SelectorFSM;
}
namespace regenny::via::behaviortree {
struct Condition;
}
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct TreeNode {
    uint64_t id; // 0x0
    regenny::via::behaviortree::TreeNodeData* data; // 0x8
    regenny::via::behaviortree::TreeObject* owner; // 0x10
    uint16_t attr; // 0x18
    uint16_t unk_flags; // 0x1a
    uint16_t pad[2]; // 0x1c
    regenny::via::behaviortree::SelectorFSM* selector; // 0x20
    regenny::via::behaviortree::Condition* selector_condition; // 0x28
    int32_t selector_condition_index; // 0x30
    int32_t priority; // 0x34
    regenny::via::behaviortree::TreeNode* node_parent; // 0x38
    uint32_t num_children; // 0x40
    private: char pad_44[0xc]; public:
    uint32_t num_actions; // 0x50
    uint32_t num_selector_callers; // 0x54
    regenny::via::behaviortree::Condition* parent_condition; // 0x58
    int32_t parent_condition_index; // 0x60
    regenny::via::behaviortree::NodeStatus status1; // 0x64
    regenny::via::behaviortree::NodeStatus status2; // 0x68
    private: char pad_6c[0x4]; public:
    // Metadata: utf16*
    wchar_t name[12]; // 0x70
    uint32_t name_len; // 0x88
    uint32_t name_capacity; // 0x8c
    regenny::via::behaviortree::TreeNode* node_end; // 0x90
    regenny::via::behaviortree::TreeNode* node_restart; // 0x98
    regenny::via::behaviortree::TreeNode* node_end_notify; // 0xa0
    regenny::via::behaviortree::TreeNode* node_end_selector; // 0xa8
    regenny::via::behaviortree::TreeNode* node_active_child; // 0xb0
    private: char pad_b8[0x18]; public:
}; // Size: 0xd0
#pragma pack(pop)
}
