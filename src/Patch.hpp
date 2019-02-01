#pragma once

#include <cstdint>
#include <vector>
#include <optional>

#include <Windows.h>

struct Patch {
    uintptr_t address{ 0 };
    std::vector<int16_t> bytes;
    std::vector<int16_t> originalBytes;
};

bool patch(uintptr_t address, const std::vector<int>& bytes);

bool patch(Patch& p);
bool undoPatch(const Patch& p);

// Returns the old protection on success.
// Note: you don't need to call this if you're using the above patch function.
std::optional<DWORD> protect(uintptr_t address, size_t size, DWORD protection);