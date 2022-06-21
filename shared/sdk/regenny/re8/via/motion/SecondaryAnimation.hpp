#pragma once
#include "..\Component.hpp"
namespace regenny::via::motion {
#pragma pack(push, 1)
struct SecondaryAnimation : public regenny::via::Component {
    char pad_30[0x18];
}; // Size: 0x48
#pragma pack(pop)
}
