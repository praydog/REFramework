#pragma once

#include <windows.h>
#include <cstdint>

namespace utility {
// True when addr is native ARM64EC code (Windows on ARM, Wine/Proton ARM64EC builds).
// An x86-64 inline hook written there is never emulated and crashes with STATUS_ILLEGAL_INSTRUCTION.
inline bool is_arm64ec_code(uintptr_t addr) {
    using RtlIsEcCodeFn = BOOLEAN(NTAPI*)(ULONG64);

    static const auto is_ec_code = []() -> RtlIsEcCodeFn {
        const auto ntdll = GetModuleHandleW(L"ntdll.dll");
        return ntdll != nullptr ? (RtlIsEcCodeFn)GetProcAddress(ntdll, "RtlIsEcCode") : nullptr;
    }();

    return addr != 0 && is_ec_code != nullptr && is_ec_code((ULONG64)addr) != FALSE;
}
}
