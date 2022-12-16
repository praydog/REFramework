#pragma once
#include "Point.hpp"
#include "Size.hpp"
namespace regenny::via {
#pragma pack(push, 1)
struct Window {
    void* vtable; // 0x0
    char pad_8[0x50];
    uint32_t width; // 0x58
    uint32_t height; // 0x5c
    regenny::via::Point cursor_pos; // 0x60
    bool show_cursor; // 0x68
    char pad_69[0x2f];
    regenny::via::Size borderless_size; // 0x98
    char pad_a0[0x60];
}; // Size: 0x100
#pragma pack(pop)
}
