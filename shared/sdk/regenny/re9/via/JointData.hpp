#pragma once
#include "vec4.hpp"
namespace regenny::via {
#pragma pack(push, 1)
struct JointData {
    regenny::via::vec4 rotation; // 0x0
    regenny::via::vec4 translation; // 0x10
    regenny::via::vec4 scale; // 0x20
    regenny::via::vec4 blend; // 0x30
}; // Size: 0x40
#pragma pack(pop)
}
