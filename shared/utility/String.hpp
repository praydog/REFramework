#pragma once

#include <string>
#include <string_view>

namespace utility {
    //
    // String utilities.
    //

    // Conversion functions for UTF8<->UTF16.
    std::string narrow(std::wstring_view std);
    std::wstring widen(std::string_view std);

    std::string format_string(const char* format, va_list args);
    
    // FNV-1a
    static constexpr auto hash(std::string_view data) {
        size_t result = 0xcbf29ce484222325;

        for (char c : data) {
            result ^= c;
            result *= (size_t)1099511628211;
        }

        return result;
    }
}

constexpr auto operator "" _fnv(const char* s, size_t) {
    return utility::hash(s);
}