#include <Windows.h>

#include "Patch.hpp"

using namespace std;

bool patch(uintptr_t address, const vector<int16_t>& bytes) {
    auto oldProtection = protect(address, bytes.size(), PAGE_EXECUTE_READWRITE);

    if (!oldProtection) {
        return false;
    }

    unsigned int count = 0;

    for (auto byte : bytes) {
        if (byte >= 0 && byte <= 0xFF) {
            *(uint8_t*)(address + count) = (uint8_t)byte;
        }

        ++count;
    }

    FlushInstructionCache(GetCurrentProcess(), (LPCVOID)address, bytes.size());
    protect(address, bytes.size(), *oldProtection);

    return true;
}

bool patch(Patch& p) {
    if (p.address == 0 || p.bytes.empty()) {
        return false;
    }

    // Backup the original bytes.
    if (p.originalBytes.empty()) {
        p.originalBytes.resize(p.bytes.size());

        unsigned int count = 0;

        for (auto& byte : p.originalBytes) {
            byte = *(uint8_t*)(p.address + count++);
        }
    }

    // Apply the patch.
    return patch(p.address, p.bytes);
}

bool undoPatch(const Patch& p) {
    if (p.address == 0 || p.originalBytes.empty()) {
        return false;
    }

    // Patch in the original bytes.
    return patch(p.address, p.originalBytes);
}

optional<DWORD> protect(uintptr_t address, size_t size, DWORD protection) {
    DWORD oldProtection{ 0 };

    if (VirtualProtect((LPVOID)address, size, protection, &oldProtection) != FALSE) {
        return oldProtection;
    }

    return {};
}
