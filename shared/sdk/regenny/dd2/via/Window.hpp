#pragma once
#include "Point.hpp"
#include "Size.hpp"
namespace regenny::via {
#pragma pack(push, 1)
struct Window {
    void* vtable; // 0x0
    private: char pad_8[0x58]; public:
    uint32_t width; // 0x60
    uint32_t height; // 0x64
    regenny::via::Point cursor_pos; // 0x68
    bool show_cursor; // 0x70
    private: char pad_71[0x47]; public:
    regenny::via::Size borderless_size; // 0xb8
    private: char pad_c0[0x40]; public:
}; // Size: 0x100
#pragma pack(pop)
}
