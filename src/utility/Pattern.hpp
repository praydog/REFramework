#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace utility {
    class Pattern {
    public:
        Pattern() = delete;
        Pattern(const Pattern& other) = default;
        Pattern(Pattern&& other) = default;
        Pattern(const std::string& pattern);
        ~Pattern() = default;

        std::optional<uintptr_t> find(uintptr_t start, size_t length);

        Pattern& operator=(const Pattern& other) = default;
        Pattern& operator=(Pattern&& other) = default;

        auto pattern_len() const noexcept { return m_pattern.size(); }

    private:
        std::vector<int16_t> m_pattern;
    };

    // Converts a string pattern (eg. "90 90 ? EB ? ? ?" to a vector of int's where
    // wildcards are -1.
    std::vector<int16_t> buildPattern(std::string patternStr);
}
