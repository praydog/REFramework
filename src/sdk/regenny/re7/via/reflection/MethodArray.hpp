#pragma once
namespace regenny::via::reflection {
struct Method;
}
namespace regenny::via::reflection {
#pragma pack(push, 1)
struct MethodArray {
    regenny::via::reflection::Method** method; // 0x0
    uint32_t num_methods; // 0x8
    uint32_t num_allocated; // 0xc
}; // Size: 0x10
#pragma pack(pop)
}
