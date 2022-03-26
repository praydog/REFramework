#include <Windows.h>

#include "Patch.hpp"

using namespace std;

std::unique_ptr<Patch> Patch::create(uintptr_t addr, const std::vector<int16_t>& b, bool shouldEnable) {
    return std::make_unique<Patch>(addr, b, shouldEnable);
}

std::unique_ptr<Patch> Patch::create_nop(uintptr_t addr, uint32_t length, bool shouldEnable) {
    std::vector<decltype(m_bytes)::value_type> bytes; bytes.resize(length);
    std::fill(bytes.begin(), bytes.end(), 0x90);

    return std::make_unique<Patch>(addr, bytes, shouldEnable);
}

bool Patch::patch(uintptr_t address, const vector<int16_t>& bytes) {
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

optional<DWORD> Patch::protect(uintptr_t address, size_t size, DWORD protection) {
    DWORD oldProtection{ 0 };

    if (VirtualProtect((LPVOID)address, size, protection, &oldProtection) != FALSE) {
        return oldProtection;
    }

    return {};
}

Patch::Patch(uintptr_t addr, const std::vector<int16_t>& b, bool shouldEnable /*= true*/) 
    : m_address{ addr },
    m_bytes{ b }
{
    if (shouldEnable) {
        enable();
    }
}

Patch::~Patch() {
    disable();
}

bool Patch::enable() {
    // Backup the original bytes.
    if (m_original_bytes.empty()) {
        m_original_bytes.resize(m_bytes.size());

        unsigned int count = 0;

        for (auto& byte : m_original_bytes) {
            byte = *(uint8_t*)(m_address + count++);
        }
    }

    return m_enabled = patch(m_address, m_bytes);
}

bool Patch::disable() {
    return !(m_enabled = !patch(m_address, m_original_bytes));
}

bool Patch::toggle() {
    if (!m_enabled) {
        return enable();
    }

    return !disable();
}


bool Patch::toggle(bool state) {
    return state ? enable() : disable();
}
