#pragma once
namespace regenny::tdb49 {
#pragma pack(push, 1)
struct MethodParam {
    uint16_t param_typeid; // 0x0
    uint64_t flag : 32; // 0x2
    uint64_t name_offset : 32; // 0x2
}; // Size: 0xa
#pragma pack(pop)
}
