#pragma once
namespace regenny::via::typeinfo {
struct TypeInfo;
}
namespace regenny {
#pragma pack(push, 1)
struct TypeArray {
    regenny::via::typeinfo::TypeInfo** data; // 0x0
    uint32_t num; // 0x8
    uint32_t num_allocated; // 0xc
}; // Size: 0x10
#pragma pack(pop)
}
