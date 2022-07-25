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
    std::optional<uintptr_t> scan_reverse(uintptr_t start, size_t length, const std::string& pattern);
    
    std::optional<uintptr_t> scan_data(HMODULE, const uint8_t* data, size_t size);
    std::optional<uintptr_t> scan_ptr(HMODULE module, uintptr_t ptr);
    std::optional<uintptr_t> scan_string(HMODULE module, const std::string& str);
    std::optional<uintptr_t> scan_string(HMODULE module, const std::wstring& str);

    std::optional<uintptr_t> scan_reference(HMODULE module, uintptr_t ptr, bool relative = true);
    std::optional<uintptr_t> scan_relative_reference_strict(HMODULE module, uintptr_t ptr, const std::string& preceded_by);

    std::optional<uintptr_t> scan_opcode(uintptr_t ip, size_t num_instructions, uint8_t opcode);
    std::optional<uintptr_t> scan_disasm(uintptr_t ip, size_t num_instructions, const std::string& pattern);

    uintptr_t calculate_absolute(uintptr_t address, uint8_t custom_offset = 4);
}
