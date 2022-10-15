#pragma once
#include "Point.hpp"
#include "Size.hpp"
namespace regenny::via {
#pragma pack(push, 1)
struct Window {
    void* vtable; // 0x0
    char pad_8[0x68];
    uint32_t width; // 0x70
    uint32_t height; // 0x74
    regenny::via::Point cursor_pos; // 0x78
    bool show_cursor; // 0x80
    char pad_81[0x2f];
    regenny::via::Size borderless_size; // 0xb0
    char pad_b8[0x48];
}; // Size: 0x100
#pragma pack(pop)
}
