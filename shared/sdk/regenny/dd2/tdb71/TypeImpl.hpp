#pragma once
namespace regenny::tdb71 {
#pragma pack(push, 1)
struct TypeImpl {
    int32_t name_offset; // 0x0
    int32_t namespace_offset; // 0x4
    int32_t field_size; // 0x8
    int32_t static_field_size; // 0xc
    uint64_t unk_pad : 33; // 0x10
    uint64_t num_member_fields : 24; // 0x10
    uint64_t unk_pad_2 : 7; // 0x10
    uint16_t num_member_methods; // 0x18
    int16_t num_native_vtable; // 0x1a
    int16_t interface_id; // 0x1c
    private: char pad_1e[0x12]; public:
}; // Size: 0x30
#pragma pack(pop)
}
