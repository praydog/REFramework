#pragma once
namespace regenny {
struct ManagedVtable;
}
namespace regenny::via::clr {
#pragma pack(push, 1)
struct ManagedObject {
    regenny::ManagedVtable* info; // 0x0
    uint32_t reference_count; // 0x8
    uint32_t unk; // 0xc
}; // Size: 0x10
#pragma pack(pop)
}
