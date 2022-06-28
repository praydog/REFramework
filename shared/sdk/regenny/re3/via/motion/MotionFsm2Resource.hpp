#pragma once
#include "MotionFsm2Internal.hpp"
namespace regenny::via::motion {
#pragma pack(push, 1)
struct MotionFsm2Resource {
    char pad_0[0x220];
    regenny::via::motion::MotionFsm2Internal fsm2data; // 0x220
}; // Size: 0x548
#pragma pack(pop)
}
