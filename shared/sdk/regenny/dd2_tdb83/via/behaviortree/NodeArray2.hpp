#pragma once
namespace regenny::via::behaviortree {
struct TreeNode;
}
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct NodeArray2 {
    regenny::via::behaviortree::TreeNode** nodes; // 0x0
    uint32_t unk; // 0x8
    uint32_t count; // 0xc
}; // Size: 0x10
#pragma pack(pop)
}
