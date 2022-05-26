#pragma once

#include <cstdint>
#include <string_view>
#include <optional>
#include <windows.h>

namespace utility {
std::optional<uint32_t> get_registry_dword(HKEY key, std::string_view subkey, std::string_view value);
}