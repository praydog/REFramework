#pragma once
#include "..\clr\ManagedObject.hpp"
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct Selector : public regenny::via::clr::ManagedObject {
    bool late_select; // 0x10
    bool b2; // 0x11
    bool b3; // 0x12
    char pad_13[0x5];
    uint32_t* buf; // 0x18
    uint32_t bufsize; // 0x20
    char pad_24[0x4];
}; // Size: 0x28
#pragma pack(pop)
}
