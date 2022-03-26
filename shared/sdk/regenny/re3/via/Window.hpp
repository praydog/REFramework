#pragma once
#include ".\Point.hpp"
#include ".\Size.hpp"
namespace regenny::via {
#pragma pack(push, 1)
struct Window {
    void* vtable; // 0x0
    char pad_8[0x40];
    uint32_t width; // 0x48
    uint32_t height; // 0x4c
    regenny::via::Point cursor_pos; // 0x50
    bool show_cursor; // 0x58
    char pad_59[0x2f];
    regenny::via::Size borderless_size; // 0x88
    char pad_90[0x70];
}; // Size: 0x100
#pragma pack(pop)
}
