#pragma once
namespace regenny::tdb67 {
#pragma pack(push, 1)
struct Field {
    uint64_t declaring_typeid : 19; // 0x0
    uint64_t impl_id : 18; // 0x0
    uint64_t offset : 17; // 0x0
    uint64_t pad_bitfield_0_36 : 10;
}; // Size: 0x8
#pragma pack(pop)
}
