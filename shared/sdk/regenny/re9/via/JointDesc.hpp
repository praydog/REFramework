#pragma once
#include "vec4.hpp"
namespace regenny::via {
#pragma pack(push, 1)
struct JointDesc {
    // Metadata: utf16*
    wchar_t* name; // 0x0
    uint32_t name_hash; // 0x8
    int16_t parent_index; // 0xc
    int16_t symmetry_index; // 0xe
    regenny::via::vec4 position; // 0x10
    regenny::via::vec4 rotation; // 0x20
    regenny::via::vec4 scale; // 0x30
}; // Size: 0x40
#pragma pack(pop)
}
