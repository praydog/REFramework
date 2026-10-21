#pragma once
namespace regenny::via {
#pragma pack(push, 1)
struct Application {
    struct Function {
        void* entry; // 0x0
        void* func; // 0x8
        void* unk; // 0x10
        // Metadata: utf8*
        char* description; // 0x18
        uint16_t priority; // 0x20
        uint16_t type; // 0x22
        uint8_t pad[172]; // 0x24
    }; // Size: 0xd0

    private: char pad_0[0x808]; public:
    Function functions[100]; // 0x808
}; // Size: 0x5948
#pragma pack(pop)
}
