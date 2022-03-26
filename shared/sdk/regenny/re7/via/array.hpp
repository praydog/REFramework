#pragma once
#include "clr\ManagedObject.hpp"
namespace regenny {
struct TypeDefinition;
}
namespace regenny::via {
#pragma pack(push, 1)
struct array : public clr::ManagedObject {
    regenny::TypeDefinition* contained_type; // 0x20
    int32_t count; // 0x28
    int32_t allocated; // 0x2c
    char pad_30[0xd0];
}; // Size: 0x100
#pragma pack(pop)
}
