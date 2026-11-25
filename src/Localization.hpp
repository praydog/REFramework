#pragma once

#include <atomic>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "localization/catalogs.hpp"

// Presentation only: callers retain their original configuration and storage keys.
namespace reframework::localization {
using LanguageProvider = Language (*)();
inline std::atomic<LanguageProvider> language_provider{nullptr};

inline void set_language_provider(LanguageProvider provider) {
    language_provider.store(provider, std::memory_order_release);
}

inline Language current_language() {
    const auto provider = language_provider.load(std::memory_order_acquire);
    return provider == nullptr ? default_language : provider();
}

inline const Catalog* current_catalog() {
    const auto index = static_cast<int>(current_language());
    return index >= 0 && index < static_cast<int>(languages.size()) ? languages[index].catalog : nullptr;
}

inline std::vector<std::string> language_names() {
    std::vector<std::string> names;
    names.reserve(languages.size());
    for (const auto& language : languages) {
        names.emplace_back(language.name);
    }
    return names;
}

inline const char* text(const char* original) {
    const auto* catalog = current_catalog();
    if (original == nullptr || catalog == nullptr) {
        return original;
    }

    const auto found = catalog->find(original);
    return found == catalog->end() ? original : found->second;
}

// Both languages use the same explicit ImGui ID. Existing ### IDs are preserved.
// Hidden labels are never translated, and dictionary misses keep their original ID.
inline std::string label(std::string_view original) {
    // Formatted TreeNode labels need formatting before assigning an explicit ID.
    // Do not duplicate printf placeholders into an appended ID.
    if (original.find('%') != std::string_view::npos) {
        return std::string{original};
    }
    const auto hidden = original.find("##");
    const auto visible = original.substr(0, hidden);
    if (visible.empty() || !source_catalog.contains(visible)) {
        return std::string{original};
    }

    std::string result{visible};
    if (const auto* catalog = current_catalog(); catalog != nullptr) {
        if (const auto found = catalog->find(visible); found != catalog->end()) {
            result = found->second;
        }
    }
    const auto explicit_id = original.find("###");
    result += explicit_id == std::string_view::npos
        ? "###" + std::string{original}
        : std::string{original.substr(explicit_id)};
    return result;
}

// Translate display options without changing their indices or stored values.
inline std::vector<const char*> options(const std::vector<const char*>& original) {
    std::vector<const char*> translated;
    translated.reserve(original.size());
    for (const auto* value : original) {
        translated.push_back(text(value));
    }
    return translated;
}
} // namespace reframework::localization
