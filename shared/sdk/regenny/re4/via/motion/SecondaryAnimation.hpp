#pragma once
#include "..\Component.hpp"
namespace regenny::via::motion {
#pragma pack(push, 1)
struct SecondaryAnimation : public regenny::via::Component {
    void* motion; // 0x30
    char pad_38[0x18];
}; // Size: 0x50
#pragma pack(pop)
}
