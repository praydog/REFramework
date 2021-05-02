#pragma once

#include <string_view>
#include <locale>

#include "utility/String.hpp"
#include "ReClass.hpp"

namespace utility::re_string {
    static std::wstring_view get_view(const ::REString& str) {
        auto length = str.length;

        if (length <= 0) {
            return L"";
        }

        std::wstring_view raw_name;

        if (length >= 12) {
            auto name_ptr = *(wchar_t**)&str;

            if (name_ptr == nullptr) {
                return L"";
            }

            raw_name = name_ptr;
        } else {
            if (length <= 0) {
                return L"";
            }

            raw_name = (wchar_t*)&str;
        }

        return raw_name;
    }

    static std::wstring_view get_view(const ::SystemString& str) {
        return std::wstring_view{ str.data };
    }

    static std::wstring_view get_view(SystemString* str) {
        return get_view(*str);
    }

    static std::string get_string(const ::REString& str) {
        return utility::narrow(get_view(str));
    }

    static std::string get_string(const ::SystemString& str) {
        return utility::narrow(str.size > 0 ? str.data : L"");
    }

    static std::string get_string(SystemString* str) {
        return get_string(*str);
    }

    static bool equals(const ::REString& str, std::wstring_view view) {
        return get_view(str) == view;
    }

    static bool equals(const ::SystemString& str, std::wstring_view view) {
        return get_view(str) == view;
    }

    static bool equals(const ::SystemString* str, std::wstring_view view) {
        return equals(*str, view);
    }
    }