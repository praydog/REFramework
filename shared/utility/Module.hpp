#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <Windows.h>

#include "Address.hpp"

namespace utility {
    //
    // Module utilities.
    //
    std::optional<size_t> get_module_size(const std::string& module);
    std::optional<size_t> get_module_size(HMODULE module);
    std::optional<HMODULE> get_module_within(Address address);
    std::optional<uintptr_t> get_dll_imagebase(Address dll);
    std::optional<uintptr_t> get_imagebase_va_from_ptr(Address dll, Address base, void* ptr);

    std::optional<std::string> get_module_path(HMODULE module);
    std::optional<std::string> get_module_directory(HMODULE module);

    std::vector<uint8_t> read_module_from_disk(HMODULE module);

    // Returns the original bytes of the module at the given address.
    // useful for un-patching something.
    std::optional<std::vector<uint8_t>> get_original_bytes(Address address);
    std::optional<std::vector<uint8_t>> get_original_bytes(HMODULE module, Address address);

    // Note: This function doesn't validate the dll's headers so make sure you've
    // done so before calling it.
    std::optional<uintptr_t> ptr_from_rva(uint8_t* dll, uintptr_t rva);

    HMODULE get_executable();
    HMODULE unlink(HMODULE module);
}
