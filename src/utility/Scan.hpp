#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include <Windows.h>

namespace utility {
    std::optional<uintptr_t> scan(const std::string& module, const std::string& pattern);
    std::optional<uintptr_t> scan(const std::string& module, uintptr_t start, const std::string& pattern);
    std::optional<uintptr_t> scan(HMODULE module, const std::string& pattern);
    std::optional<uintptr_t> scan(uintptr_t start, size_t length, const std::string& pattern);

    uintptr_t calculate_absolute(uintptr_t address, uint8_t custom_offset = 4);
}
