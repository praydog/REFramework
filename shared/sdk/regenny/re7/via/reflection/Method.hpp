#pragma once
namespace regenny::via::reflection {
#pragma pack(push, 1)
struct Method {
    // Metadata: utf8*
    char* name; // 0x0
    char pad_8[0x10];
    void* function; // 0x18
    char pad_20[0x18];
    // Metadata: utf8*
    char* base_type; // 0x38
    // Metadata: utf8*
    char* pass_type; // 0x40
    // Metadata: utf8*
    char* type_name; // 0x48
    // Metadata: utf8*
    char* full_type_name; // 0x50
    char pad_58[0xa8];
}; // Size: 0x100
#pragma pack(pop)
}
