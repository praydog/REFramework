#pragma once
namespace regenny::via {
struct UIntArray;
}
namespace regenny::via {
#pragma pack(push, 1)
struct UIntArrayArray {
    regenny::via::UIntArray* data; // 0x0
    uint64_t count; // 0x8
}; // Size: 0x10
#pragma pack(pop)
}
