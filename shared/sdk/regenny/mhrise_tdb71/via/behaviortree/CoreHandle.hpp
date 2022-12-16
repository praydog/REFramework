#pragma once
#include "Core.hpp"
#include "..\clr\ManagedObject.hpp"
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct CoreHandle : public regenny::via::clr::ManagedObject {
    char pad_10[0x8];
    regenny::via::behaviortree::Core core; // 0x18
    char pad_1f8[0x28];
}; // Size: 0x220
#pragma pack(pop)
}
