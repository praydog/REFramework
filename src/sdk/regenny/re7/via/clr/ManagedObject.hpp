#pragma once
#include "VM.hpp"

namespace regenny::via::clr {
#pragma pack(push, 1)
struct ManagedObject {
    void** vtable; // 0x0
    uint32_t reference_count; // 0x8
    uint32_t MO_unk; // 0xc
    uint32_t MO_unk2; // 0x10
    uint32_t MO_unk3; // 0x14
    regenny::via::clr::VM::Type* vm_type; // 0x18
}; // Size: 0x20
#pragma pack(pop)
}
