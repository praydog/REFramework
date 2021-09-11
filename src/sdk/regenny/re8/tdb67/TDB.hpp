#pragma once
namespace regenny {
struct TypeDefinition;
}
namespace regenny::tdb67 {
struct MethodDefinition;
}
namespace regenny::tdb67 {
#pragma pack(push, 1)
struct TDB {
    struct test {
    }; // Size: 0x0

    uint32_t magic; // 0x0
    uint32_t version; // 0x4
    uint32_t initialized; // 0x8
    uint32_t numTypes; // 0xc
    uint32_t numMethods; // 0x10
    uint32_t numFields; // 0x14
    uint32_t numProperties; // 0x18
    uint32_t numEvents; // 0x1c
    uint32_t numUnk; // 0x20
    uint32_t maybeNumParams; // 0x24
    uint32_t maybeNumAttributes; // 0x28
    uint32_t numInitData; // 0x2c
    uint32_t numInternStrings; // 0x30
    uint32_t numModules; // 0x34
    uint32_t devEntry; // 0x38
    uint32_t appEntry; // 0x3c
    uint32_t numStringPool; // 0x40
    uint32_t numBytePool; // 0x44
    void* modules; // 0x48
    regenny::TypeDefinition* types; // 0x50
    regenny::tdb67::MethodDefinition* methods; // 0x58
}; // Size: 0x60
#pragma pack(pop)
}
