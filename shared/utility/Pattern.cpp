#include <algorithm>

#include <Windows.h>

#include "Memory.hpp"
#include "Pattern.hpp"

using namespace std;

namespace utility {
    static uint8_t toByte(char digit) {
        if (digit >= '0' && digit <= '9') {
            return (digit - '0');
        }

        if (digit >= 'a' && digit <= 'f') {
            return (digit - 'a' + 10);
        }

        if (digit >= 'A' && digit <= 'F') {
            return (digit - 'A' + 10);
        }

        return 0;
    }

    Pattern::Pattern(const string& pattern)
        : m_pattern{}
    {
        m_pattern = move(buildPattern(pattern));
    }

    optional<uintptr_t> Pattern::find(uintptr_t start, size_t length) {
        auto patternLength = m_pattern.size();
        auto end = start + length - patternLength;

        for (auto i = start; i <= end; ++i) {
            auto j = i;
            auto failedToMatch = false;

            // Make sure the address is readable.
            if (IsBadReadPtr((const void*)i, patternLength) != FALSE) {
                i += patternLength - 1;
                continue;
            }

            for (auto& k : m_pattern) {
                if (k != -1 && k != *(uint8_t*)j) {
                    failedToMatch = true;
                    break;
                }

                ++j;
            }

            if (!failedToMatch) {
                return i;
            }
        }

        return {};
    }

    vector<int16_t> buildPattern(string patternStr) {
        // Remove spaces from the pattern string.
        patternStr.erase(remove_if(begin(patternStr), end(patternStr), isspace), end(patternStr));

        auto length = patternStr.length();
        vector<int16_t> pattern{};

        for (size_t i = 0; i < length;) {
            auto p1 = patternStr[i];

            if (p1 != '?') {
                // Bytes require 2 hex characters to encode, make sure we don't read
                // past the end of the pattern string attempting to read the next char.
                if (i + 1 >= length) {
                    break;
                }

                auto p2 = patternStr[i + 1];
                auto value = toByte(p1) << 4 | toByte(p2);

                pattern.emplace_back(value);

                i += 2;
            }
            else {
                // Wildcard's (?'s) get encoded as a -1.
                pattern.emplace_back(-1);
                i += 1;
            }
        }

        return pattern;
    }
}
