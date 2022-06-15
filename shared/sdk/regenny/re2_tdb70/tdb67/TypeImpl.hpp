#pragma once
namespace regenny::tdb67 {
#pragma pack(push, 1)
struct TypeImpl {
    int32_t name_offset; // 0x0
    int32_t namespace_offset; // 0x4
    int32_t field_size; // 0x8
    int32_t static_field_size; // 0xc
    uint8_t module_id; // 0x10
    uint8_t array_rank; // 0x11
    uint16_t num_member_methods; // 0x12
    int32_t num_member_fields; // 0x14
    int16_t interface_id; // 0x18
    uint16_t num_native_vtable; // 0x1a
    uint16_t attributes_id; // 0x1c
    uint16_t num_vtable; // 0x1e
    uint64_t mark; // 0x20
    uint64_t cycle; // 0x28
}; // Size: 0x30
#pragma pack(pop)
}
