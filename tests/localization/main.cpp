#include "Localization.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "re2-imgui/font_robotomedium.hpp"

#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <sstream>
#include <stdexcept>

namespace loc = reframework::localization;
static loc::Language selected_language = loc::Language::SimplifiedChinese;
static int checks = 0;
static loc::Language language_provider() { return selected_language; }
static void check(bool condition, const std::string& message) {
    ++checks;
    if (!condition) throw std::runtime_error(message);
}
static std::vector<std::string> format_tokens(const std::string& text) {
    static const std::regex pattern(R"(%(?:%|[-+ #0']*(?:\d+|\*)?(?:\.(?:\d+|\*))?(?:hh|ll|I64|I32|[hljztL])?[diuoxXfFeEgGaAcspn]))");
    std::vector<std::string> tokens;
    for (auto i = std::sregex_iterator(text.begin(), text.end(), pattern); i != std::sregex_iterator(); ++i)
        tokens.push_back(i->str());
    return tokens;
}
static std::vector<std::string> brace_tokens(const std::string& text) {
    static const std::regex pattern(R"(\{[^{}]*\})");
    std::vector<std::string> tokens;
    for (auto i = std::sregex_iterator(text.begin(), text.end(), pattern); i != std::sregex_iterator(); ++i)
        tokens.push_back(i->str());
    return tokens;
}
static std::string unicode_name(unsigned int cp) {
    std::ostringstream out;
    out << "U+" << std::uppercase << std::hex << std::setfill('0') << std::setw(cp <= 0xffff ? 4 : 6) << cp;
    return out.str();
}
static std::map<unsigned int, std::set<std::string>> glyph_usage;
static void collect_codepoints(const char* text, const std::string& usage) {
    while (*text != 0) {
        unsigned int cp = 0;
        const int length = ImTextCharFromUtf8(&cp, text, nullptr);
        check(length > 0 && cp != IM_UNICODE_CODEPOINT_INVALID, "Invalid UTF-8: " + usage);
        if (cp >= 0x80) glyph_usage[cp].insert(usage);
        text += length;
    }
}

int main() {
    try {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        auto& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        io.LogFilename = nullptr;
        io.DisplaySize = ImVec2(1280.0f, 720.0f);
        io.DeltaTime = 1.0f / 60.0f;
        io.Fonts->AddFontDefault();
        check(io.Fonts->Build(), "Default atlas build");
        ImGui::NewFrame();
        ImGui::Begin("Localization regression");
        ImGui::PushID("parent scope");

        check(loc::current_language() == loc::Language::English, "Default language must be English");
        check(static_cast<int>(loc::Language::English) == 0, "Persisted English index changed");
        check(static_cast<int>(loc::Language::SimplifiedChinese) == 1, "Persisted Simplified Chinese index changed");
        check(static_cast<int>(loc::Language::TraditionalChinese) == 2, "Traditional Chinese index mismatch");
        check(static_cast<int>(loc::Language::Japanese) == 3, "Japanese index mismatch");
        check(static_cast<int>(loc::Language::Korean) == 4, "Korean index mismatch");
        check(loc::languages.size() == 5, "Expected five selectable languages");
        check(loc::source_catalog.size() == 298, "Expected 298 source keys");
        check(loc::languages[0].catalog == nullptr, "English must retain original text");
        const auto names = loc::language_names();
        const std::vector<std::string> expected_names{"English", "简体中文", "繁體中文", "日本語", "한국어"};
        check(names == expected_names, "Language self names or saved option order changed");
        loc::set_language_provider(language_provider);

        size_t catalog_count = 0;
        for (size_t language_index = 0; language_index < loc::languages.size(); ++language_index) {
            const auto& language = loc::languages[language_index];
            selected_language = static_cast<loc::Language>(language_index);
            check(loc::current_language() == selected_language, "Language provider not used");
            check(loc::current_catalog() == language.catalog, "Wrong selected catalog");
            check(names[language_index] == language.name, "Language names diverged from registry");
            collect_codepoints(language.name, std::string(language.tag) + " language name");
            if (language.catalog != nullptr) {
                ++catalog_count;
                check(language.catalog->size() == loc::source_catalog.size(), "Catalog size differs: " + std::string(language.tag));
                for (const auto& [key, ignored] : loc::source_catalog)
                    check(language.catalog->contains(key), "Missing key: " + std::string(language.tag) + ": " + std::string(key));
            }

            size_t translated_labels = 0, formatted_entries = 0;
            for (const auto& [original, ignored] : loc::source_catalog) {
                const std::string source{original};
                const std::string context = std::string(language.tag) + ": " + source;
                const char* translated = language.catalog == nullptr ? source.c_str() : language.catalog->at(original);
                const auto tokens = format_tokens(source);
                check(tokens == format_tokens(translated), "Printf contract mismatch: " + context);
                check(brace_tokens(source) == brace_tokens(translated), "Brace format contract mismatch: " + context);
                if (!tokens.empty()) ++formatted_entries;
                selected_language = loc::Language::English;
                const auto english = loc::label(source);
                const auto english_id = ImGui::GetID(english.c_str());
                selected_language = static_cast<loc::Language>(language_index);
                const auto localized = loc::label(source);
                check(std::strcmp(loc::text(source.c_str()), translated) == 0, "Text lookup mismatch: " + context);
                check(english_id == ImGui::GetID(localized.c_str()), "ImGui::GetID changed: " + context);
                check(ImHashStr(english.c_str()) == ImHashStr(localized.c_str()), "ImHashStr changed: " + context);
                check(format_tokens(localized) == tokens, "Label added printf tokens: " + context);
                if (source.find('%') == std::string::npos) {
                    check(localized.substr(0, localized.find("##")) == translated, "Wrong localized label: " + context);
                    ++translated_labels;
                } else {
                    check(localized == source, "Formatted label must remain exact: " + context);
                }
                collect_codepoints(translated, context);
            }

            for (const char* original : {"Camera###camera-persistent", "Configuration###config-persistent"}) {
                const auto localized = loc::label(original);
                const auto explicit_id = std::string(original).substr(std::string(original).find("###"));
                check(localized.ends_with(explicit_id), "Explicit ### suffix lost");
                check(ImGui::GetID(original) == ImGui::GetID(localized.c_str()), "Explicit ### identity changed");
            }
            for (const char* hidden : {"##Camera", "###Camera", "##search", ""})
                check(loc::label(hidden) == hidden, "Pure hidden label was altered");
            for (const char* original : {"Camera##left", "Camera##right"}) {
                selected_language = loc::Language::English;
                const auto english = loc::label(original);
                selected_language = static_cast<loc::Language>(language_index);
                const auto localized = loc::label(original);
                check(localized.ends_with(std::string("###") + original), "Hidden identity suffix lost");
                check(ImGui::GetID(english.c_str()) == ImGui::GetID(localized.c_str()), "Hidden suffix ID unstable");
            }
            check(ImGui::GetID(loc::label("Camera##left").c_str()) != ImGui::GetID(loc::label("Camera##right").c_str()), "Distinct ## IDs collided");
            for (const char* unknown : {"Third-party untranslated entry", "", "##Camera"}) {
                check(loc::text(unknown) == unknown, "Unknown or empty text pointer changed");
                check(loc::label(unknown) == unknown, "Unknown or empty label changed");
            }
            check(loc::text(nullptr) == nullptr, "Null text changed");

            const std::vector<const char*> original_options{"Camera", "Configuration", "Untranslated option", "Camera"};
            const auto original_pointers = original_options;
            std::vector<std::string> original_values;
            for (const auto* option : original_options) original_values.emplace_back(option);
            const auto displayed_options = loc::options(original_options);
            check(displayed_options.size() == original_options.size(), "Options count changed");
            for (size_t i = 0; i < original_options.size(); ++i) {
                check(original_options[i] == original_pointers[i], "Stored option pointer mutated");
                check(original_values[i] == original_options[i], "Stored option value mutated");
                check(std::strcmp(displayed_options[i], loc::text(original_options[i])) == 0, "Option index changed");
                // Integer-backed and string-backed settings keep their original stored value.
                const auto saved_index = static_cast<int>(i);
                const auto saved_value = std::string(original_options[saved_index]);
                check(saved_value == original_values[i], "Saved option value changed after translation");
            }
            check(loc::options({}).empty(), "Empty option list changed");
            if (language.catalog == nullptr) {
                check(displayed_options == original_pointers, "English option pointers changed");
                check(loc::text(original_options[0]) == original_options[0], "English text pointer changed");
            }
            std::cout << "PASS " << language.tag << ": " << loc::source_catalog.size() << " entries, " << translated_labels
                      << " static labels, " << formatted_entries << " printf entries; real ImHashStr/GetID stable; "
                      << "format contracts, hidden/explicit IDs, unknown/null text and stored option values preserved\n";
        }
        check(catalog_count == 4, "Expected four translated catalogs");
        std::cout << "PASS registry: five language self names, preserved option indices, English default\n";

        ImGui::PopID();
        ImGui::End();
        ImGui::Render();

        // Real official compressed font, rasterized offline: no game, GPU or GUI required.
        // Accumulate every missing glyph before failing, without replacing text to hide font gaps.
        ImFontAtlas font_atlas;
        ImFontGlyphRangesBuilder ranges_builder;
        for (const auto& [cp, usages] : glyph_usage) {
            check(cp <= 0xFFFF, "Test needs 32-bit ImWchar for " + unicode_name(cp));
            ranges_builder.AddChar(static_cast<ImWchar>(cp));
        }
        ImVector<ImWchar> ranges;
        ranges_builder.BuildRanges(&ranges);
        ImFontConfig config;
        config.GlyphRanges = ranges.Data;
        auto* font = font_atlas.AddFontFromMemoryCompressedTTF(RobotoCJKSC_Medium_compressed_data,
            RobotoCJKSC_Medium_compressed_size, 18.0f, &config);
        check(font != nullptr && font_atlas.Build(), "Official CJK font failed to load/build");
        auto* baked = font->GetFontBaked(18.0f);
        size_t missing_count = 0;
        for (const auto& [cp, usages] : glyph_usage) {
            const bool in_font = font->IsGlyphInFont(static_cast<ImWchar>(cp));
            const auto* glyph = baked->FindGlyphNoFallback(static_cast<ImWchar>(cp));
            checks += 2;
            if (!in_font || glyph == nullptr) {
                ++missing_count;
                std::cerr << "MISSING GLYPH " << unicode_name(cp) << ": font=" << in_font << ", rasterized=" << (glyph != nullptr) << '\n';
                for (const auto& usage : usages) std::cerr << "  " << usage << '\n';
            }
        }
        check(missing_count == 0, "Official RobotoCJKSC-Medium missing " + std::to_string(missing_count) + " of " + std::to_string(glyph_usage.size()) + " distinct non-ASCII characters; see complete list above");
        std::cout << "PASS official RobotoCJKSC-Medium: all " << glyph_usage.size()
                  << " distinct non-ASCII characters from four catalogs and all language self names present and rasterized\n";
        loc::set_language_provider(nullptr);
        check(loc::current_language() == loc::Language::English, "Default language changed after removing provider");
        check(loc::current_catalog() == nullptr, "Default English catalog must be null");
        ImGui::DestroyContext();
        std::cout << "ALL PASS: " << checks << " checks\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL after " << checks << " checks: " << error.what() << '\n';
        return 1;
    }
}
