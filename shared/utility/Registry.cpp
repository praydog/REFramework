#include <spdlog/spdlog.h>

#include "Registry.hpp"

namespace utility {
std::optional<uint32_t> get_registry_dword(HKEY key, std::string_view subkey, std::string_view value) {
    HKEY subkey_handle{};

    if (auto res = RegOpenKeyExA(key, subkey.data(), 0, KEY_QUERY_VALUE, &subkey_handle); res != ERROR_SUCCESS) {
        spdlog::error("({}) Failed to open registry key {}", res, subkey);
        return std::nullopt;
    }

    DWORD type{};
    DWORD size{};

    if (auto res = RegQueryValueExA(subkey_handle, value.data(), nullptr, &type, nullptr, &size); res != ERROR_SUCCESS) {
        spdlog::error("({}) Failed to query registry value {}", res, value);
        RegCloseKey(subkey_handle);
        return std::nullopt;
    }

    if (type != REG_DWORD) {
        spdlog::error("Registry value is not of type REG_DWORD: {}", value);
        RegCloseKey(subkey_handle);
        return std::nullopt;
    }

    DWORD result{};

    if (auto res = RegQueryValueExA(subkey_handle, value.data(), nullptr, nullptr, (LPBYTE)&result, &size); res != ERROR_SUCCESS) {
        spdlog::error("({}) Failed to query registry value 2 {}", res, value);
        RegCloseKey(subkey_handle);
        return std::nullopt;
    }

    RegCloseKey(subkey_handle);
    return result;
}
}