#pragma once
namespace regenny::via::typeinfo {
struct TypeInfo;
}
namespace regenny::via::reflection {
struct Reflection;
}
namespace regenny::via::clr {
struct Type;
}
namespace regenny::via::typeinfo {
#pragma pack(push, 1)
struct TypeInfo {
    void** vfptr; // 0x0
    uint32_t fqn_hash; // 0x8
    uint16_t attributes; // 0xc
    uint16_t type; // 0xe
    uint64_t fast_class_index; // 0x10
    char pad_18[0x8];
    uint32_t flags; // 0x20
    char pad_24[0x4];
    // Metadata: utf8*
    char* name; // 0x28
    uint32_t parent_fqn; // 0x30
    char pad_34[0x4];
    uint32_t size; // 0x38
    char pad_3c[0x24];
    regenny::via::typeinfo::TypeInfo* parent; // 0x60
    regenny::via::typeinfo::TypeInfo* next; // 0x68
    regenny::via::typeinfo::TypeInfo* prev; // 0x70
    char pad_78[0x100];
    regenny::via::reflection::Reflection* reflection; // 0x178
    regenny::via::clr::VM::Type* vm_type; // 0x180
    uint32_t crc; // 0x188
    char pad_18c[0x4];
}; // Size: 0x190
#pragma pack(pop)
}
