#pragma once
#include ".\TypeInfo.hpp"
namespace regenny::via::clr {
struct Type;
}
namespace regenny::via::typeinfo {
struct DeserializeSequence;
}
namespace regenny::via::typeinfo {
#pragma pack(push, 1)
struct TypeInfoCLR : public TypeInfo {
    struct DeserializerArray {
        regenny::via::typeinfo::DeserializeSequence* data; // 0x0
        uint32_t num; // 0x8
        uint32_t num_allocated; // 0xc
    }; // Size: 0x10

    DeserializerArray deserializers; // 0x190
    char pad_1a0[0x8];
    regenny::via::clr::VM::Type* native_type; // 0x1a8
}; // Size: 0x1b0
#pragma pack(pop)
}
