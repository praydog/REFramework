#pragma once
#include "..\ManagedObjectArray.hpp"
#include "..\UIntArray.hpp"
#include "NodeArray.hpp"
namespace regenny::via::behaviortree {
struct TreeObjectData;
}
namespace regenny::via::behaviortree {
struct TreeNode;
}
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct TreeObject {
    void* vfptr; // 0x0
    regenny::via::behaviortree::TreeObjectData* data; // 0x8
    regenny::via::behaviortree::NodeArray nodes; // 0x10
    private: char pad_1c[0x4]; public:
    regenny::via::ManagedObjectArray selectors; // 0x20
    private: char pad_30[0x10]; public:
    regenny::via::ManagedObjectArray actions; // 0x40
    regenny::via::ManagedObjectArray conditions; // 0x50
    regenny::via::ManagedObjectArray transitions; // 0x60
    regenny::via::ManagedObjectArray expression_tree_conditions; // 0x70
    regenny::via::UIntArray selector_nodes; // 0x80
    regenny::via::ManagedObjectArray delayed_actions; // 0x90
    regenny::via::ManagedObjectArray delayed_conditions; // 0xa0
    regenny::via::ManagedObjectArray delayed_transitions; // 0xb0
    regenny::via::behaviortree::TreeNode* root_node; // 0xc0
    private: char pad_c8[0x10]; public:
}; // Size: 0xd8
#pragma pack(pop)
}
