#pragma once
#include ".\Object.hpp"
namespace regenny::via {
struct Entry;
}
namespace regenny::via {
#pragma pack(push, 1)
struct Application : public Object {
    struct RangeInt {
        int32_t l; // 0x0
        int32_t h; // 0x4
    }; // Size: 0x8

    struct Ticket {
        uint32_t priority; // 0x0
        char pad_4[0x4];
        void* jobset; // 0x8
    }; // Size: 0x10

    struct Function {
        regenny::via::Entry* entry; // 0x0
        void* func; // 0x8
        void* unk; // 0x10
        // Metadata: utf8*
        char* description; // 0x18
        uint16_t priority; // 0x20
        uint16_t type; // 0x22
        char pad_24[0xac];
    }; // Size: 0xd0

    char pad_8[0x338];
    uint8_t bool_thing; // 0x340
    char pad_341[0xb7];
    void* hwnd; // 0x3f8
    char pad_400[0x38];
    regenny::via::Entry* entries[128]; // 0x438
    Function functions[1024]; // 0x838
    RangeInt test[6]; // 0x34838
    Ticket tickets[1024]; // 0x34868
    uint32_t ticket_thing; // 0x38868
    char pad_3886c[0x6a4];
}; // Size: 0x38f10
#pragma pack(pop)
}
