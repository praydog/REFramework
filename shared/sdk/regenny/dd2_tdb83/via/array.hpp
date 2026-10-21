#pragma once
#include "clr\ManagedObject.hpp"
namespace regenny {
struct TypeDefinition;
}
namespace regenny::via {
#pragma pack(push, 1)
struct array : public clr::ManagedObject {
    regenny::TypeDefinition* contained_type; // 0x10
    int32_t count; // 0x18
    int32_t allocated; // 0x1c
    private: char pad_20[0xe0]; public:
}; // Size: 0x100
#pragma pack(pop)
}
