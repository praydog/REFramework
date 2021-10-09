#pragma once
namespace regenny::tdb49 {
#pragma pack(push, 1)
struct MethodDefinition {
    struct AdditionalData {
        int16_t unk; // 0x0
        uint16_t return_typeid; // 0x2
    }; // Size: 0x4

    uint64_t declaring_typeid : 16; // 0x0
    uint64_t invoke_id : 16; // 0x0
    uint64_t num_params : 6; // 0x0
    uint64_t unk : 10; // 0x0
    uint64_t vtable_index : 16; // 0x0
    uint32_t prototype_name_offset; // 0x8
    char pad_c[0x4];
    uint32_t name_offset; // 0x10
    uint16_t flags; // 0x14
    uint16_t impl_flags; // 0x16
    uint32_t unk2; // 0x18
    uint32_t method_data_offset; // 0x1c
}; // Size: 0x20
#pragma pack(pop)
}
