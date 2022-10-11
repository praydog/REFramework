#pragma once

#include <string_view>
#include <locale>

#include "utility/String.hpp"
#include "ReClass.hpp"

namespace utility::re_string {
    static std::wstring_view get_view(const ::REString& str) {
        if (&str == nullptr) {
            return L"";
        }

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
        if (&str == nullptr) {
            return L"";
        }

        return std::wstring_view{ str.data };
    }

    static std::wstring_view get_view(SystemString* str) {
        if (str == nullptr) {
            return L"";
        }

        return get_view(*str);
    }

    static std::string get_string(const ::REString& str) {
        if (&str == nullptr) {
            return "";
        }

        return utility::narrow(get_view(str));
    }

    static std::string get_string(const ::SystemString& str) {
        if (&str == nullptr) {
            return "";
        }

        return utility::narrow(str.size > 0 ? str.data : L"");
    }

    static std::string get_string(SystemString* str) {
        if (str == nullptr) {
            return "";
        }

        return get_string(*str);
    }

    static bool equals(const ::REString& str, std::wstring_view view) {
        if (&str == nullptr) {
            return false;
        }

        return get_view(str) == view;
    }

    static bool equals(const ::SystemString& str, std::wstring_view view) {
        if (&str == nullptr) {
            return false;
        }

        return get_view(str) == view;
    }

    static bool equals(const ::SystemString* str, std::wstring_view view) {
        if (str == nullptr) {
            return false;
        }

        return equals(*str, view);
    }
    }