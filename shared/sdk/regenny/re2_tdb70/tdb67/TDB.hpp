#pragma once
namespace regenny::tdb67 {
struct PropertyImpl;
}
namespace regenny::tdb67 {
struct AttributeDef;
}
namespace regenny::tdb67 {
struct TypeDefinition;
}
namespace regenny::tdb67 {
struct MethodDefinition;
}
namespace regenny::tdb67 {
struct TypeImpl;
}
namespace regenny::tdb67 {
struct Field;
}
namespace regenny::tdb67 {
struct MethodImpl;
}
namespace regenny::tdb67 {
struct FieldImpl;
}
namespace regenny::tdb67 {
struct Property;
}
namespace regenny::tdb67 {
struct ParameterDef;
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
    uint32_t numTypeImpl; // 0x18
    uint32_t numFieldImpl; // 0x1c
    uint32_t numMethodImpl; // 0x20
    uint32_t numPropertyImpl; // 0x24
    uint32_t numProperties; // 0x28
    uint32_t numEvents; // 0x2c
    uint32_t numParams; // 0x30
    uint32_t numAttributes; // 0x34
    int32_t numInitData; // 0x38
    uint32_t numAttributes2; // 0x3c
    uint32_t numInternStrings; // 0x40
    uint32_t numModules; // 0x44
    int32_t devEntry; // 0x48
    int32_t appEntry; // 0x4c
    uint32_t numStringPool; // 0x50
    uint32_t numBytePool; // 0x54
    void* modules; // 0x58
    regenny::tdb67::TypeDefinition* types; // 0x60
    regenny::tdb67::TypeImpl* typesImpl; // 0x68
    regenny::tdb67::MethodDefinition* methods; // 0x70
    regenny::tdb67::MethodImpl* methodsImpl; // 0x78
    regenny::tdb67::Field* fields; // 0x80
    regenny::tdb67::FieldImpl* fieldsImpl; // 0x88
    regenny::tdb67::Property* properties; // 0x90
    regenny::tdb67::PropertyImpl* propertiesImpl; // 0x98
    void* events; // 0xa0
    regenny::tdb67::ParameterDef* params; // 0xa8
    regenny::tdb67::AttributeDef* attributes; // 0xb0
    int32_t* initData; // 0xb8
    void* unk; // 0xc0
    int32_t* attributes2; // 0xc8
    char* stringPool; // 0xd0
    uint8_t* bytePool; // 0xd8
    int32_t* internStrings; // 0xe0
}; // Size: 0xe8
#pragma pack(pop)
}
