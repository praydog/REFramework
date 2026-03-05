#pragma once
namespace regenny::via {
#pragma pack(push, 1)
struct Application {
    struct Function {
        void* entry; // 0x0
        void* func; // 0x8
        // Metadata: utf8*
        char* description; // 0x10
        uint16_t priority; // 0x18
        uint16_t type; // 0x1a
        uint8_t pad[160]; // 0x1c
        private: char pad_bc[0xc]; public:
    }; // Size: 0xc8

    private: char pad_0[0x820]; public:
    Function functions[1024]; // 0x820
}; // Size: 0x32820
#pragma pack(pop)
}
