#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include <string_view>
#include <functional>

#include <Windows.h>
#include <winternl.h>

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
    std::optional<std::wstring> get_module_directoryw(HMODULE module);
    HMODULE load_module_from_current_directory(const std::wstring& module);

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
    HMODULE safe_unlink(HMODULE module);

    void foreach_module(std::function<void(LIST_ENTRY*, _LDR_DATA_TABLE_ENTRY*)> callback);
    size_t get_module_count(std::wstring_view name);
    void unlink_duplicate_modules();
    void spoof_module_paths_in_exe_dir();
}
