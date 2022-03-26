#pragma once
namespace regenny::via::reflection {
struct Property;
}
namespace regenny::via::typeinfo {
#pragma pack(push, 1)
struct DeserializeSequence {
    uint16_t code; // 0x0
    uint16_t depth; // 0x2
    uint32_t offset; // 0x4
    regenny::via::reflection::Property* prop; // 0x8
}; // Size: 0x10
#pragma pack(pop)
}
