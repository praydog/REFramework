#pragma once
#include "Point.hpp"
#include "Size.hpp"
namespace regenny::via {
#pragma pack(push, 1)
struct Window {
    void* vtable; // 0x0
    char pad_8[0x70];
    uint32_t width; // 0x78
    uint32_t height; // 0x7c
    regenny::via::Point cursor_pos; // 0x80
    bool show_cursor; // 0x88
    char pad_89[0x2f];
    regenny::via::Size borderless_size; // 0xb8
    char pad_c0[0x40];
}; // Size: 0x100
#pragma pack(pop)
}
