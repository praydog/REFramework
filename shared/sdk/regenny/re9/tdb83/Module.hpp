#pragma once
namespace regenny::tdb83 {
#pragma pack(push, 1)
struct Module {
    uint32_t guid[4]; // 0x0
    int32_t unk1; // 0x10
    uint32_t unk2; // 0x14
    uint16_t major; // 0x18
    uint16_t minor; // 0x1a
    uint16_t build; // 0x1c
    uint16_t revision; // 0x1e
    uint32_t flags; // 0x20
    int32_t assembly_name_offset; // 0x24
    int32_t location_offset; // 0x28
    uint32_t unk3[2]; // 0x2c
    uint32_t module_name_offset; // 0x34
    int32_t types_count; // 0x38
    int32_t types_start; // 0x3c
    int32_t methods_count; // 0x40
    int32_t methods_start; // 0x44
    int32_t method_instantiations_count; // 0x48
    int32_t method_instantiations_start; // 0x4c
    int32_t member_references_count; // 0x50
    int32_t member_references_start; // 0x54
}; // Size: 0x58
#pragma pack(pop)
}
