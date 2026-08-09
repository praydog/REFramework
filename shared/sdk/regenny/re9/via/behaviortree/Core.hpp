#pragma once
namespace regenny::via::behaviortree {
struct TreeObject;
}
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct Core {
    void** vftable; // 0x0
    regenny::via::behaviortree::TreeObject* tree_object; // 0x8
    private: char pad_10[0x1d0]; public:
}; // Size: 0x1e0
#pragma pack(pop)
}
