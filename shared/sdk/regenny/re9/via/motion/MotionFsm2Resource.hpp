#pragma once
#include "MotionFsm2Internal.hpp"
namespace regenny::via::motion {
#pragma pack(push, 1)
struct MotionFsm2Resource {
    private: char pad_0[0x50]; public:
    regenny::via::motion::MotionFsm2Internal fsm2data; // 0x50
}; // Size: 0x378
#pragma pack(pop)
}
