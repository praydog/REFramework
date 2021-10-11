#pragma once
namespace regenny::via::reflection {
struct Property;
}
namespace regenny::via::typeinfo {
#pragma pack(push, 1)
struct Deserializer {
    uint32_t code : 8; // 0x0
    uint32_t size : 8; // 0x0
    uint32_t align : 8; // 0x0
    uint32_t depth : 6; // 0x0
    uint32_t is_array : 1; // 0x0
    uint32_t is_static : 1; // 0x0
    uint32_t offset; // 0x4
    regenny::via::reflection::Property* prop; // 0x8
}; // Size: 0x10
#pragma pack(pop)
}
