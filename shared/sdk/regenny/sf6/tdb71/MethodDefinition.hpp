#pragma once
namespace regenny::tdb71 {
#pragma pack(push, 1)
struct MethodDefinition {
    uint32_t declaring_typeid : 19; // 0x0
    uint32_t pad : 13; // 0x0
    uint32_t impl_id : 19; // 0x4
    uint32_t params : 13; // 0x4
    int32_t offset; // 0x8
}; // Size: 0xc
#pragma pack(pop)
}
