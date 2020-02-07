#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include <Windows.h>

namespace utility {
    //
    // Module utilities.
    //
    std::optional<size_t> get_module_size(const std::string& module);
    std::optional<size_t> get_module_size(HMODULE module);

    std::optional<std::string> get_module_directory(HMODULE module);

    // Note: This function doesn't validate the dll's headers so make sure you've
    // done so before calling it.
    std::optional<uintptr_t> ptr_from_rva(uint8_t* dll, uintptr_t rva);
}
