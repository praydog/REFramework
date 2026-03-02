#pragma once
namespace regenny::via::behaviortree {
struct TreeNode;
}
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct NodeArray {
    regenny::via::behaviortree::TreeNode* nodes; // 0x0
    uint32_t count; // 0x8
}; // Size: 0xc
#pragma pack(pop)
}
