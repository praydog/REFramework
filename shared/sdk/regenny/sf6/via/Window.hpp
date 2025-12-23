#pragma once
#include "Point.hpp"
#include "Size.hpp"
namespace regenny::via {
#pragma pack(push, 1)
struct Window {
    void* vtable; // 0x0
    private: char pad_8[0x50]; public:
    uint32_t width; // 0x58
    uint32_t height; // 0x5c
    regenny::via::Point cursor_pos; // 0x60
    bool show_cursor; // 0x68
    private: char pad_69[0x2f]; public:
    regenny::via::Size borderless_size; // 0x98
    private: char pad_a0[0x60]; public:
}; // Size: 0x100
#pragma pack(pop)
}
