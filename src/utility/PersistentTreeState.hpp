#pragma once

#include <algorithm>
#include <charconv>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include <imgui.h>
#include <imgui_internal.h>

#include <utility/String.hpp>

namespace reframework::ui {
enum class TreeStateSource : uint8_t {
    Native,
    Lua,
    CImGui,
    Count,
};

namespace detail {
struct TreeState {
    std::unordered_set<uint64_t> open_nodes{};
    std::unordered_set<uint64_t> initialized_nodes{};
    std::filesystem::path settings_path{};
    ImGuiContext* context{};
    bool loaded{};
};

inline TreeState g_tree_state{};

inline uint64_t make_key(TreeStateSource source, uint64_t component) {
    return (component & 0x00FFFFFFFFFFFFFFULL) | (static_cast<uint64_t>(source) << 56);
}

inline void read_line(std::string_view line) {
    static constexpr std::string_view PREFIX{"Node="};

    if (!line.starts_with(PREFIX)) {
        return;
    }

    uint64_t key{};
    const auto first = line.data() + PREFIX.size();
    const auto last = line.data() + line.size();
    const auto result = std::from_chars(first, last, key, 16);

    if (result.ec == std::errc{} && result.ptr == last) {
        const auto source_value = static_cast<uint8_t>(key >> 56);

        if (source_value < static_cast<uint8_t>(TreeStateSource::Count)) {
            g_tree_state.open_nodes.insert(key);
        }
    }
}

inline void save_open_nodes() {
    if (g_tree_state.settings_path.empty()) {
        return;
    }

    std::vector<uint64_t> sorted_nodes{g_tree_state.open_nodes.begin(), g_tree_state.open_nodes.end()};
    std::ranges::sort(sorted_nodes);

    std::ofstream output{g_tree_state.settings_path, std::ios::trunc};

    if (!output) {
        return;
    }

    output << "[Open]\nVersion=1\n";

    for (const auto key : sorted_nodes) {
        output << "Node=" << std::uppercase << std::hex << std::setw(16) << std::setfill('0') << key << '\n';
    }
}

inline void update_open_state(uint64_t key, bool is_open) {
    const bool changed = is_open
        ? g_tree_state.open_nodes.insert(key).second
        : g_tree_state.open_nodes.erase(key) > 0;

    if (changed) {
        save_open_nodes();
    }
}
} // namespace detail

inline void initialize_tree_state(const std::filesystem::path& settings_path) {
    if (detail::g_tree_state.loaded) {
        if (detail::g_tree_state.context != ImGui::GetCurrentContext()) {
            detail::g_tree_state.context = ImGui::GetCurrentContext();
            detail::g_tree_state.initialized_nodes.clear();
        }

        return;
    }

    detail::g_tree_state.loaded = true;
    detail::g_tree_state.settings_path = settings_path;
    detail::g_tree_state.context = ImGui::GetCurrentContext();
    detail::g_tree_state.open_nodes.clear();
    detail::g_tree_state.initialized_nodes.clear();

    std::ifstream input{settings_path};
    std::string line{};
    bool recognized_format{};
    bool read_any_line{};

    while (std::getline(input, line)) {
        read_any_line = true;

        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line == "Version=1") {
            recognized_format = true;
        } else if (recognized_format) {
            detail::read_line(line);
        }
    }

    input.close();

    if (read_any_line && !recognized_format) {
        detail::save_open_nodes();
    }
}

template <typename DrawFunction>
bool persistent_tree_item(
    TreeStateSource source,
    std::string_view stable_id,
    DrawFunction&& draw_function,
    bool can_persist = true) {
    const auto component = utility::hash(stable_id);
    const auto key = detail::make_key(source, component);

    if (can_persist && detail::g_tree_state.initialized_nodes.insert(key).second) {
        const auto& next_item_data = ImGui::GetCurrentContext()->NextItemData;
        const bool caller_supplied_open_state = (next_item_data.HasFlags & ImGuiNextItemDataFlags_HasOpen) != 0;

        if (!caller_supplied_open_state) {
            ImGui::SetNextItemOpen(detail::g_tree_state.open_nodes.contains(key), ImGuiCond_Always);
        }
    }

    const bool is_open = draw_function();

    if (can_persist && ImGui::IsItemToggledOpen()) {
        detail::update_open_state(key, is_open);
    }

    return is_open;
}
} // namespace reframework::ui
