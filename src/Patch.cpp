#include <Windows.h>

#include "Patch.hpp"

using namespace std;

std::unique_ptr<Patch> Patch::create(uintptr_t addr, const std::vector<int16_t>& b) {
    return std::make_unique<Patch>(addr, b);
}


std::unique_ptr<Patch> Patch::createNOP(uintptr_t addr, uint32_t length) {
    std::vector<decltype(m_bytes)::value_type> bytes; bytes.resize(length);
    std::fill(bytes.begin(), bytes.end(), 0x90);

    return std::make_unique<Patch>(addr, bytes);
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
    if (m_originalBytes.empty()) {
        m_originalBytes.resize(m_bytes.size());

        unsigned int count = 0;

        for (auto& byte : m_originalBytes) {
            byte = *(uint8_t*)(m_address + count++);
        }
    }

    return m_enabled = patch(m_address, m_bytes);
}

bool Patch::disable() {
    return !(m_enabled = !patch(m_address, m_originalBytes));
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
