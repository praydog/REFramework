#pragma once
namespace regenny {
struct StdMapNode;
}
namespace regenny {
#pragma pack(push, 1)
struct StdMap {
    regenny::StdMapNode* _MyHead; // 0x0
    int32_t _MySize; // 0x8
    uint8_t pad[4]; // 0xc
}; // Size: 0x10
#pragma pack(pop)
}
