#pragma once
namespace regenny::tdb49 {
struct TypeDefinition;
}
namespace regenny::via::typeinfo {
struct TypeInfo;
}
namespace regenny::tdb49 {
struct FieldDefinition;
}
namespace regenny::tdb49 {
struct MethodDefinition;
}
namespace regenny {
struct BullShit;
}
namespace regenny::tdb49 {
struct TDB;
}
namespace regenny::via::clr {
#pragma pack(push, 1)
struct VM {
    struct Type {
        regenny::tdb49::TypeDefinition* tdb_type; // 0x0
        uint32_t type_flags; // 0x8
        uint32_t fieldptr_offset; // 0xc
        char pad_10[0x10];
        regenny::via::clr::VM::Type* parent; // 0x20
        regenny::via::clr::VM::Type* next; // 0x28
        char pad_30[0x10];
        struct regenny::via::clr::VM::Method** methods; // 0x40
        struct regenny::via::clr::VM::Field** fields; // 0x48
        void* static_fields; // 0x50
        char pad_58[0x8];
        regenny::via::typeinfo::TypeInfo* reflection_type; // 0x60
        void** vtable; // 0x68
    }; // Size: 0x70

    struct Module {
        void** vtable; // 0x0
        // Metadata: utf8*
        char* name; // 0x8
        char pad_10[0x20];
    }; // Size: 0x30

    struct FullModule {
        char pad_0[0x8];
        uint32_t unk1; // 0x8
        uint32_t unk2; // 0xc
        regenny::via::clr::VM::Module module; // 0x10
    }; // Size: 0x40

    struct ModuleContainer {
        regenny::via::clr::VM::Module* module; // 0x0
        regenny::via::clr::VM::FullModule* full_module; // 0x8
    }; // Size: 0x10

    struct ModuleArray {
        regenny::via::clr::VM::ModuleContainer* modules; // 0x0
        uint32_t num; // 0x8
        uint32_t num_allocated; // 0xc
    }; // Size: 0x10

    struct Method {
        regenny::tdb49::MethodDefinition* tdb_method; // 0x0
        char pad_8[0x8];
        void* function; // 0x10
        char pad_18[0x8];
    }; // Size: 0x20

    struct Field {
        regenny::tdb49::FieldDefinition* tdb_field; // 0x0
        uint32_t unk; // 0x8
        uint32_t offset; // 0xc
        regenny::via::clr::VM::Type* type; // 0x10
    }; // Size: 0x18

    char pad_0[0x110];
    ModuleArray modules; // 0x110
    char pad_120[0x3920];
    Type* types; // 0x3a40
    uint32_t num_types; // 0x3a48
    char pad_3a4c[0x4];
    Method* methods; // 0x3a50
    uint32_t num_methods; // 0x3a58
    char pad_3a5c[0x6c];
    regenny::tdb49::TDB* tdb; // 0x3ac8
    char pad_3ad0[0x28];
    regenny::BullShit** asdf; // 0x3af8
    char pad_3b00[0xc500];
}; // Size: 0x10000
#pragma pack(pop)
}
