#pragma once
#include ".\Point.hpp"
namespace regenny::via {
#pragma pack(push, 1)
struct Window {
    void* vtable; // 0x0
    char pad_8[0x38];
    uint32_t width; // 0x40
    uint32_t height; // 0x44
    regenny::via::Point cursor_pos; // 0x48
    bool show_cursor; // 0x50
    char pad_51[0xfaf];
}; // Size: 0x1000
#pragma pack(pop)
}
