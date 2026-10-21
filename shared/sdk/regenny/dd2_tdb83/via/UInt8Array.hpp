#pragma once
namespace regenny::via {
#pragma pack(push, 1)
struct UInt8Array {
    uint8_t* data; // 0x0
    uint64_t count; // 0x8
}; // Size: 0x10
#pragma pack(pop)
}
