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
}; // Size: 0x10
#pragma pack(pop)
}
