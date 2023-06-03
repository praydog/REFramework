#pragma once
#include "via\Component.hpp"
namespace regenny {
#pragma pack(push, 1)
struct RenderEntity : public via::Component {
    char pad_30[0x468];
}; // Size: 0x498
#pragma pack(pop)
}
