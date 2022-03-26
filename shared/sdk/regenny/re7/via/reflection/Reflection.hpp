#pragma once
namespace regenny::via::reflection {
struct MethodArray;
}
namespace regenny::via::reflection {
struct PropertyArray;
}
namespace regenny {
struct BullShit;
}
namespace regenny::via::reflection {
#pragma pack(push, 1)
struct Reflection {
    regenny::via::reflection::MethodArray** method_variations; // 0x0
    uint32_t num_methods; // 0x8
    uint32_t num_allocated_methods; // 0xc
    regenny::BullShit* a1; // 0x10
    char pad_18[0x8];
    regenny::via::reflection::PropertyArray* a3; // 0x20
    char pad_28[0x8];
    void* deserializer; // 0x30
    char pad_38[0xc8];
}; // Size: 0x100
#pragma pack(pop)
}
