#pragma once
#include "Core.hpp"
#include "..\clr\ManagedObject.hpp"
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct CoreHandle : public regenny::via::clr::ManagedObject {
    private: char pad_10[0x8]; public:
    regenny::via::behaviortree::Core core; // 0x18
    private: char pad_1f8[0x28]; public:
}; // Size: 0x220
#pragma pack(pop)
}
