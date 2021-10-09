#pragma once
namespace regenny::tdb49 {
struct MethodDefinition;
}
namespace regenny::tdb49 {
struct PropertyDefinition;
}
namespace regenny::tdb49 {
struct TypeDefinition;
}
namespace regenny::tdb49 {
struct Module;
}
namespace regenny::tdb49 {
struct FieldDefinition;
}
namespace regenny::tdb49 {
#pragma pack(push, 1)
struct TDB {
    uint32_t magic; // 0x0
    uint32_t version; // 0x4
    uint32_t initialized; // 0x8
    uint32_t numTypes; // 0xc
    uint32_t numMethods; // 0x10
    uint32_t numFields; // 0x14
    uint32_t numProperties; // 0x18
    uint32_t numEvents; // 0x1c
    uint32_t numInitData; // 0x20
    uint32_t numModules; // 0x24
    uint32_t devEntry; // 0x28
    uint32_t appEntry; // 0x2c
    uint32_t numStringPool; // 0x30
    uint32_t numBytePool; // 0x34
    regenny::tdb49::Module* modules; // 0x38
    regenny::tdb49::TypeDefinition* types; // 0x40
    regenny::tdb49::MethodDefinition* methods; // 0x48
    regenny::tdb49::FieldDefinition* fields; // 0x50
    regenny::tdb49::PropertyDefinition* properties; // 0x58
    void* events; // 0x60
    void* strings; // 0x68
    uint8_t* bytes; // 0x70
    char pad_78[0x88];
}; // Size: 0x100
#pragma pack(pop)
}
