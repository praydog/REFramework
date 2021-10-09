#pragma once
namespace regenny::via::reflection {
#pragma pack(push, 1)
struct Property {
    // Metadata: utf8*
    char* name; // 0x0
    char pad_8[0x18];
    // Metadata: utf8*
    char* base_type; // 0x20
    // Metadata: utf8*
    char* pass_type; // 0x28
    // Metadata: utf8*
    char* type_name; // 0x30
    // Metadata: utf8*
    char* full_type_name; // 0x38
    char pad_40[0x20];
    void* getter; // 0x60
}; // Size: 0x68
#pragma pack(pop)
}
