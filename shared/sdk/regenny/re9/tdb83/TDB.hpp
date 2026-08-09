#pragma once
namespace regenny::tdb83 {
struct Module;
}
namespace regenny::tdb83 {
struct TypeImpl;
}
namespace regenny::tdb83 {
struct TypeDefinition;
}
namespace regenny::tdb83 {
struct MethodDefinition;
}
namespace regenny::tdb83 {
struct MethodImpl;
}
namespace regenny::tdb83 {
struct FieldImpl;
}
namespace regenny::tdb83 {
struct Field;
}
namespace regenny::tdb83 {
struct Property;
}
namespace regenny::tdb83 {
struct PropertyImpl;
}
namespace regenny::tdb83 {
struct ParameterDef;
}
namespace regenny::tdb83 {
struct AttributeDef;
}
namespace regenny::tdb83 {
#pragma pack(push, 1)
struct TDB {
    struct test {
    }; // Size: 0x0

    uint32_t magic; // 0x0
    uint32_t version; // 0x4
    uint32_t numTypes; // 0x8
    uint32_t typesStartOfGenericsProbably; // 0xc
    uint32_t unknown_new_tdb_54; // 0x10
    uint32_t numMethods; // 0x14
    uint32_t numFields; // 0x18
    uint32_t numTypeImpl; // 0x1c
    uint32_t numFieldImpl; // 0x20
    uint32_t numMethodImpl; // 0x24
    uint32_t numPropertyImpl; // 0x28
    uint32_t numProperties; // 0x2c
    uint32_t numEvents; // 0x30
    uint32_t numParams; // 0x34
    uint32_t numAttributes; // 0x38
    int32_t numInitData; // 0x3c
    uint32_t numAttributes2; // 0x40
    uint32_t numInternStrings; // 0x44
    uint32_t numModules; // 0x48
    int32_t devEntry; // 0x4c
    int32_t appEntry; // 0x50
    uint32_t unkNum; // 0x54
    uint32_t numStringPool; // 0x58
    uint32_t numBytePool; // 0x5c
    regenny::tdb83::Module* modules; // 0x60
    regenny::tdb83::TypeDefinition* types; // 0x68
    regenny::tdb83::TypeImpl* typesImpl; // 0x70
    regenny::tdb83::MethodDefinition* methods; // 0x78
    regenny::tdb83::MethodImpl* methodsImpl; // 0x80
    regenny::tdb83::Field* fields; // 0x88
    regenny::tdb83::FieldImpl* fieldsImpl; // 0x90
    regenny::tdb83::Property* properties; // 0x98
    regenny::tdb83::PropertyImpl* propertiesImpl; // 0xa0
    void* events; // 0xa8
    regenny::tdb83::ParameterDef* params; // 0xb0
    regenny::tdb83::AttributeDef* attributes; // 0xb8
    int32_t* initData; // 0xc0
    void* unk; // 0xc8
    int32_t* attributes2; // 0xd0
    // Metadata: utf8*
    char* stringPool; // 0xd8
    uint8_t* bytePool; // 0xe0
    int32_t* internStrings; // 0xe8
}; // Size: 0xf0
#pragma pack(pop)
}
