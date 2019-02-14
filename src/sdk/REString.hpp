#pragma once

#include <string_view>
#include <locale>

#include "utility/String.hpp"
#include "ReClass.hpp"

namespace utility::REString {
    static std::wstring_view getView(const ::REString& str) {
        auto length = str.length;

        if (length <= 0) {
            return L"";
        }

        std::wstring_view rawName;

        if (length >= 12) {
            auto namePtr = *(wchar_t**)&str;

            if (namePtr == nullptr) {
                return L"";
            }

            rawName = namePtr;
        }
        else {
            if (length <= 0) {
                return L"";
            }

            rawName = (wchar_t*)&str;
        }

        return rawName;
    }

    static std::string getString(const ::REString& str) {
        return utility::narrow(getView(str));
    }

    static bool equals(const ::REString& str, std::wstring_view view) {
        return getView(str) == view;
    }
}