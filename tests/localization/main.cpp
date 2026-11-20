#include "Localization.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "re2-imgui/font_robotomedium.hpp"

#include <array>
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

static std::string finish_ui_log() {
    // LogFinish clears LogBuffer. Copy what the real widgets rendered first.
    const std::string rendered = ImGui::GetCurrentContext()->LogBuffer.c_str();
    ImGui::LogFinish();
    return rendered;
}

static void test_object_explorer_widgets() {
    struct TreeCase {
        const char* format;
        int count;
        const char* english;
        const char* chinese;
        ImGuiID id{};
    };
    std::array<TreeCase, 5> trees{{
        {"Reflection Methods: %i", 7, "Reflection Methods: 7", "反射方法：7"},
        {"Reflection Properties: %i", 13, "Reflection Properties: 13", "反射属性：13"},
        {"TDB Fields: %i", 23, "TDB Fields: 23", "类型数据库字段：23"},
        {"TDB Methods: %i", 41, "TDB Methods: 41", "类型数据库方法：41"},
        {"%s [Hits: %d [%d instructions]]", 0,
            "field%value [Hits: 11 [17 instructions]]", "field%value [命中：11 次 [17 条指令]]"},
    }};
    ImGuiID input_id{}, drag_id{};
    unsigned int threshold = 123;
    int edited_value = -456;
    // Set an open state once, switch language without forcing it, then repeat closed.
    // A zero log expansion depth prevents logging from silently opening our trees.
    for (int frame = 0; frame < 4; ++frame) {
        const bool chinese = frame == 1 || frame == 2;
        const bool expected_open = frame < 2;
        selected_language = chinese ? loc::Language::SimplifiedChinese : loc::Language::English;
        ImGui::NewFrame();
        ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(950, 600), ImGuiCond_Once);
        ImGui::Begin("ObjectExplorer widget regression");
        ImGui::PushID("object scope");
        ImGui::LogToBuffer(0);
        for (size_t i = 0; i < trees.size(); ++i) {
            auto& tree = trees[i];
            const void* ptr_id = &tree;
            const auto original_pointer_id = ImGui::GetID(ptr_id);
            if (frame == 0) tree.id = original_pointer_id;
            if (frame == 0 || frame == 2) ImGui::SetNextItemOpen(expected_open);
            const bool open = i + 1 == trees.size()
                ? ImGui::TreeNode(ptr_id, loc::text(tree.format), "field%value", 11, 17)
                : ImGui::TreeNode(ptr_id, loc::text(tree.format), tree.count);
            check(ImGui::GetItemID() == original_pointer_id && original_pointer_id == tree.id,
                "Formatted tree lost its original pointer ID: " + std::string(tree.format));
            check(open == expected_open, "Formatted tree open state changed with language: " + std::string(tree.format));
            // TreeNode pushes a scope when open; its storage remains in this window.
            check(ImGui::GetStateStorage()->GetInt(tree.id, -1) == static_cast<int>(expected_open),
                "Formatted tree did not retain its stored open/closed state");
            if (open) ImGui::TreePop();
        }

        ImGui::ProgressBar(0.25f, ImVec2(300, 0), loc::text("Dumping Types..."));
        ImGui::ProgressBar(0.75f, ImVec2(300, 0), loc::text("Generating IDA SDK..."));
        ImGui::InputScalar(loc::label("Max Call Count").c_str(), ImGuiDataType_U32, &threshold);
        if (frame == 0) input_id = ImGui::GetItemID();
        check(ImGui::GetItemID() == input_id, "Max Call Count input ID changed with language");
        ImGui::DragInt(loc::label("Set Value").c_str(), &edited_value);
        if (frame == 0) drag_id = ImGui::GetItemID();
        check(ImGui::GetItemID() == drag_id, "Set Value drag ID changed with language");
        check(threshold == 123 && edited_value == -456, "Language switch changed numeric control values");

        const auto rendered = finish_ui_log();
        for (const auto& tree : trees) {
            check(rendered.find(chinese ? tree.chinese : tree.english) != std::string::npos,
                "Real TreeNode render lost translated text or format arguments: " + std::string(tree.format));
        }
        for (const auto* expected : chinese
            ? std::initializer_list<const char*>{"正在导出类型…", "正在生成 IDA SDK…", "最大调用次数", "设置值", "123", "-456"}
            : std::initializer_list<const char*>{"Dumping Types...", "Generating IDA SDK...", "Max Call Count", "Set Value", "123", "-456"}) {
            check(rendered.find(expected) != std::string::npos, "Real progress/input render missing: " + std::string(expected));
        }
        check(rendered.find("###") == std::string::npos, "Internal ImGui ID leaked into displayed text");
        ImGui::PopID();
        ImGui::End();
        ImGui::Render();
    }
    std::cout << "PASS ObjectExplorer widgets: real ptr-ID TreeNodes retain open/closed state across language switches; "
              << "rendered counts, string arguments, translated progress and numeric controls verified\n";
}

static void test_object_explorer_windows() {
    struct WindowCase {
        const char* title;
        const char* chinese;
        ImGuiWindow* window{};
        ImGuiID id{};
        ImVec2 position{};
    };
    std::array<WindowCase, 3> windows{{
        {"Read Listeners", "读取监听器"},
        {"Pinned objects", "已固定的对象"},
        {"Hooked methods", "已挂钩的方法"},
    }};
    for (int frame = 0; frame < 3; ++frame) {
        const bool chinese = frame == 1;
        selected_language = chinese ? loc::Language::SimplifiedChinese : loc::Language::English;
        ImGui::NewFrame();
        for (size_t i = 0; i < windows.size(); ++i) {
            auto& expected = windows[i];
            if (frame == 0) {
                ImGui::SetNextWindowPos(ImVec2(40.0f + 330.0f * static_cast<float>(i), 80.0f));
                ImGui::SetNextWindowSize(ImVec2(300, 180));
            }
            ImGui::LogToBuffer(0);
            bool open = true;
            ImGui::Begin(loc::label(expected.title).c_str(), &open);
            auto* current = ImGui::GetCurrentWindow();
            if (frame == 0) {
                expected.window = current;
                expected.id = current->ID;
                // Move the actual window, then verify the user position survives two switches.
                ImGui::SetWindowPos(ImVec2(current->Pos.x + 11.0f, current->Pos.y + 19.0f));
                expected.position = current->Pos;
            }
            check(current == expected.window && current->ID == expected.id,
                "Language switch created a different window: " + std::string(expected.title));
            check(current->Pos.x == expected.position.x && current->Pos.y == expected.position.y,
                "Language switch moved a window: " + std::string(expected.title));
            // ImGui may retain the first Name internally; the title bar renders
            // Begin's current name argument. Assert the rendered title instead.
            const auto rendered = finish_ui_log();
            check(rendered.find(chinese ? expected.chinese : expected.title) != std::string::npos,
                "Actual window title rendering missing translated title");
            ImGui::End();
        }
        ImGui::Render();
    }
    std::cout << "PASS ObjectExplorer floating windows: translated titles render and preserve actual window identity and moved positions\n";
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
        for (const auto* module : {"FaultyFileDetector", "FirstPerson", "ManualFlashlight"})
            check(loc::source_catalog.contains(module), "Missing built-in module label: " + std::string(module));
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

        test_object_explorer_widgets();
        test_object_explorer_windows();

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
