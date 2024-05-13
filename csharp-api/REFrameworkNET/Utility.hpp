#pragma once

#include <string_view>
#include <cstdint>

namespace REFrameworkNET {
    ref class TypeDefinition;

    static constexpr auto hash(std::string_view data) {
        size_t result = 0xcbf29ce484222325;

        for (char c : data) {
            result ^= c;
            result *= (size_t)1099511628211;
        }

        return result;
    }

    consteval auto operator "" _fnv(const char* s, size_t) {
        return hash(s);
    }

    ref class Utility {
    public:
        static System::Object^ BoxData(uintptr_t* ptr, TypeDefinition^ t, bool fromInvoke);
        static System::Object^ TranslateBoxedData(System::Type^ targetReturnType, System::Object^ boxedData);
    };
}