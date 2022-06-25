#pragma once
#include "..\ManagedObjectArray.hpp"
#include "NodeArray.hpp"
namespace regenny::via::behaviortree {
struct TreeObjectData;
}
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct TreeObject {
    void* vfptr; // 0x0
    regenny::via::behaviortree::TreeObjectData* data; // 0x8
    regenny::via::behaviortree::NodeArray nodes; // 0x10
    char pad_1c[0x4];
    regenny::via::ManagedObjectArray selectors; // 0x20
    char pad_30[0x10];
    regenny::via::ManagedObjectArray actions; // 0x40
    char pad_50[0x20];
    regenny::via::ManagedObjectArray conditions; // 0x70
    char pad_80[0x10];
    regenny::via::ManagedObjectArray delayed_actions; // 0x90
    char pad_a0[0x38];
}; // Size: 0xd8
#pragma pack(pop)
}
