#pragma once
struct RTL_CRITICAL_SECTION_DEBUG;
namespace regenny::via::render {
#pragma pack(push, 1)
struct RTL_CRITICAL_SECTION {
    RTL_CRITICAL_SECTION_DEBUG* DebugInfo; // 0x0
    int32_t LockCount; // 0x8
    int32_t RecursionCount; // 0xc
    void* OwningThread; // 0x10
    void* LockSemaphore; // 0x18
    uint64_t SpinCount; // 0x20
}; // Size: 0x28
#pragma pack(pop)
}
