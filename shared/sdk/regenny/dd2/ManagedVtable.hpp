#pragma once
namespace regenny::tdb71 {
struct TypeDefinition;
}
namespace regenny {
#pragma pack(push, 1)
struct ManagedVtable {
    regenny::tdb71::TypeDefinition* t; // 0x0
    void* funcs[32]; // 0x8
}; // Size: 0x108
#pragma pack(pop)
}
