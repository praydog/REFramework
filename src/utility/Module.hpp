#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include <Windows.h>

namespace utility {
    //
    // Module utilities.
    //
    std::optional<size_t> getModuleSize(const std::string& module);
    std::optional<size_t> getModuleSize(HMODULE module);

    std::optional<std::string> getModuleDirectory(HMODULE module);

    // Note: This function doesn't validate the dll's headers so make sure you've
    // done so before calling it.
    std::optional<uintptr_t> ptrFromRVA(uint8_t* dll, uintptr_t rva);
}
