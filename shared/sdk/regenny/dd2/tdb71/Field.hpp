#pragma once
namespace regenny::tdb71 {
#pragma pack(push, 1)
struct Field {
    uint64_t declaring_typeid : 19; // 0x0
    uint64_t impl_id : 19; // 0x0
    uint64_t field_typeid : 19; // 0x0
    uint64_t init_data_hi : 6; // 0x0
    private: uint64_t pad_bitfield_0_3f : 1; public:
}; // Size: 0x8
#pragma pack(pop)
}
