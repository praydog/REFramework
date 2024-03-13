#pragma once
namespace regenny::tdb71 {
#pragma pack(push, 1)
struct ParamList {
    uint16_t numParams; // 0x0
    int16_t invokeID; // 0x2
    uint32_t returnType; // 0x4
    uint32_t params[256]; // 0x8
}; // Size: 0x408
#pragma pack(pop)
}
