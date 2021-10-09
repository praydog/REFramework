#pragma once
namespace regenny::via::reflection {
struct Property;
}
namespace regenny::via::reflection {
#pragma pack(push, 1)
struct PropertyArray {
    regenny::via::reflection::Property** properties; // 0x0
    uint32_t num_properties; // 0x8
    uint32_t num_allocated; // 0xc
    char pad_10[0xf0];
}; // Size: 0x100
#pragma pack(pop)
}
