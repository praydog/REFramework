#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <optional>

#include <Windows.h>

class Patch {
public:
    using Ptr = std::unique_ptr<Patch>;
    static Ptr create(uintptr_t addr, const std::vector<int16_t>& b);
    static Ptr createNOP(uintptr_t addr, uint32_t length);

    Patch(uintptr_t addr, const std::vector<int16_t>& b, bool shouldEnable = true);

    virtual ~Patch();
    
    bool enable();
    bool disable();
    bool toggle();
    bool toggle(bool state);

    static bool patch(uintptr_t address, const std::vector<int16_t>& bytes);
    // Returns the old protection on success.
    // Note: you don't need to call this if you're using the above patch function.
    static std::optional<DWORD> protect(uintptr_t address, size_t size, DWORD protection);

private:
    uintptr_t m_address{ 0 };
    std::vector<int16_t> m_bytes;
    std::vector<int16_t> m_originalBytes;
    bool m_enabled{ false };
};