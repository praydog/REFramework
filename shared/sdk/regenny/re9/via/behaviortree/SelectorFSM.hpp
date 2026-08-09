#pragma once
#include "NodeArray2.hpp"
#include "Selector.hpp"
namespace regenny::via::behaviortree {
struct TreeNode;
}
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct SelectorFSM : public Selector {
    regenny::via::behaviortree::TreeNode* owner_node; // 0x28
    regenny::via::behaviortree::TreeNode* active_node; // 0x30
    regenny::via::behaviortree::NodeArray2 next_nodes; // 0x38
    regenny::via::behaviortree::NodeArray2 node_choices; // 0x48
    bool illegal; // 0x58
    private: char pad_59[0x7]; public:
}; // Size: 0x60
#pragma pack(pop)
}
