#pragma once
#include "NodeArray.hpp"
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct TreeObject {
    char pad_0[0x10];
    regenny::via::behaviortree::NodeArray nodes; // 0x10
    char pad_1c[0xbc];
}; // Size: 0xd8
#pragma pack(pop)
}
