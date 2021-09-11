#pragma once
namespace regenny::tdb67 {
#pragma pack(push, 1)
struct MethodDefinition {
    uint64_t declaring_typeid : 17; // 0x0
    uint64_t invoke_id : 16; // 0x0
    uint64_t num_params : 6; // 0x0
    uint64_t unk : 8; // 0x0
    uint64_t return_typeid : 17; // 0x0
    char pad_8[0x2];
    int16_t vtable_index; // 0xa
    uint32_t name_offset; // 0xc
    uint16_t flags; // 0x10
    uint16_t impl_flags; // 0x12
    uint32_t params; // 0x14
    void* function; // 0x18
}; // Size: 0x20
#pragma pack(pop)
}
