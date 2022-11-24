#pragma once

#include <string_view>
#include <locale>

#include <utility/String.hpp>
#include <sdk/Memory.hpp>
#include "ReClass.hpp"

namespace utility::re_string {
    static void set_string(::REString& str, std::wstring_view value) {
        const auto current_length = str.length;

        if (current_length >= 12 && *(wchar_t**)&str != nullptr) {
            sdk::memory::deallocate(*(wchar_t**)&str);
            *(wchar_t**)&str = nullptr;
        }

        str.length = 0;
        str.maxLength = 0;
        const auto new_length = value.length();

        if (new_length == 0) {
            return;
        }

        if (new_length >= 12) {     
            const auto new_buffer = (wchar_t*)sdk::memory::allocate((new_length + 1) * sizeof(wchar_t));
            *(wchar_t**)&str = new_buffer;
            
            std::memcpy(new_buffer, value.data(), new_length * sizeof(wchar_t));
            new_buffer[new_length] = 0;
            str.length = new_length;
            str.maxLength = new_length;
        } else {
            // copy the string to the buffer
            std::memcpy((wchar_t*)&str, value.data(), new_length * sizeof(wchar_t));
            ((wchar_t*)&str)[new_length] = 0;
            str.length = new_length;
            str.maxLength = new_length;
        }
    }

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