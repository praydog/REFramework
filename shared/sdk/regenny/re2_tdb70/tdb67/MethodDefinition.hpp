#pragma once
namespace regenny::tdb67 {
#pragma pack(push, 1)
struct MethodDefinition {
    uint64_t declaring_typeid : 18; // 0x0
    uint64_t impl_id : 20; // 0x0
    uint64_t params : 26; // 0x0
    void* function; // 0x8
}; // Size: 0x10
#pragma pack(pop)
}
