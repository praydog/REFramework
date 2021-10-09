#pragma once
namespace regenny::tdb49 {
#pragma pack(push, 1)
struct Module {
    int32_t name; // 0x0
    char pad_4[0x4];
    int32_t bytepool1; // 0x8
    char pad_c[0x4];
    int32_t bytepool2; // 0x10
    char pad_14[0x4];
    int32_t bytepool3; // 0x18
}; // Size: 0x1c
#pragma pack(pop)
}
