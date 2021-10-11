#pragma once

#include "../clr/VM.hpp"

namespace regenny::via::reflection {
#pragma pack(push, 1)
struct Property {
    struct PropertyFlags {
        uint32_t type_kind : 5; // 0x0
        uint32_t type_qual : 2; // 0x0
        uint32_t type_attr : 3; // 0x0
        uint32_t size : 20; // 0x0
        uint32_t managed_str : 1; // 0x0
        uint32_t reserved : 1; // 0x0
    }; // Size: 0x4

    // Metadata: utf8*
    char* name; // 0x0
    char pad_8[0x8];
    uint32_t type_kind; // 0x10
    uint32_t type_qual; // 0x14
    uint32_t type_attr; // 0x18
    bool managed_str; // 0x1c
    char pad_1d[0x3];
    // Metadata: utf8*
    char* base_type; // 0x20
    // Metadata: utf8*
    char* pass_type; // 0x28
    // Metadata: utf8*
    char* type_name; // 0x30
    // Metadata: utf8*
    char* full_type_name; // 0x38
    uint32_t size; // 0x40
    char pad_44[0x1c];
    void* getter; // 0x60
    void* getter_array; // 0x68
    char pad_70[0x60];
    void* getter_array2; // 0xd0
    char pad_d8[0x50];
    regenny::via::clr::VM::Field* vm_field; // 0x128
    uint32_t clr_member_type; // 0x130
    char pad_134[0x4];
}; // Size: 0x138
#pragma pack(pop)
}
