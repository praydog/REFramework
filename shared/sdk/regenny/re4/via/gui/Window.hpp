#pragma once
#include "Control.hpp"
namespace regenny::via::gui {
#pragma pack(push, 1)
struct Window : public Control {
    bool Open; // 0x1b0
    char pad_1b1[0xf];
}; // Size: 0x1c0
#pragma pack(pop)
}
