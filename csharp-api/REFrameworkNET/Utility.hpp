#pragma once

#include <string_view>
#include <cstdint>

namespace REFrameworkNET {
    ref class TypeDefinition;
    ref class Field;

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
        // field can be null, just used for more information (particularly for static strings that can be literal)
        static System::Object^ BoxData(uintptr_t* ptr, TypeDefinition^ t, bool fromInvoke) {
            return BoxData(ptr, t, fromInvoke, nullptr);
        }
        
        static System::Object^ BoxData(uintptr_t* ptr, TypeDefinition^ t, bool fromInvoke, Field^ field);
        static System::Object^ TranslateBoxedData(System::Type^ targetReturnType, System::Object^ boxedData);
    };
}