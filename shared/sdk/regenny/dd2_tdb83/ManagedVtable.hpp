#pragma once
namespace regenny::tdb73 {
struct TypeDefinition;
}
namespace regenny {
#pragma pack(push, 1)
struct ManagedVtable {
    regenny::tdb73::TypeDefinition* t; // 0x0
    void* funcs[32]; // 0x8
}; // Size: 0x108
#pragma pack(pop)
}
