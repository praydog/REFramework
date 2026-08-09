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
    uint8_t pad[3]; // 0x71
    private: char pad_74[0x34]; public:
    regenny::via::Size borderless_size; // 0xa8
    private: char pad_b0[0x50]; public:
}; // Size: 0x100
#pragma pack(pop)
}
