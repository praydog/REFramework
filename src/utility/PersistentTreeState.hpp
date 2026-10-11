#pragma once

#include <charconv>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_set>

#include <imgui.h>
#include <imgui_internal.h>

#include <utility/Config.hpp>

namespace reframework::ui {
enum class TreeStateSource : uint8_t {
    Native,
    Lua,
    CImGui,
    Count,
};

namespace detail {
static inline constexpr int32_t CONFIG_VERSION{1};
static inline constexpr std::string_view CONFIG_VERSION_KEY{"Version"};
static inline constexpr std::string_view OPEN_NODE_PREFIX{"OpenNode_"};

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

inline void save_open_nodes() {
    if (g_tree_state.settings_path.empty()) {
        return;
    }

    utility::Config config{};
    config.set<int32_t>(CONFIG_VERSION_KEY.data(), CONFIG_VERSION);

    for (const auto key : g_tree_state.open_nodes) {
        config.set<bool>(std::string{OPEN_NODE_PREFIX} + std::to_string(key), true);
    }

    config.save(g_tree_state.settings_path.string());
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

    const utility::Config config{settings_path.string()};

    if (config.get<int32_t>(detail::CONFIG_VERSION_KEY.data()).value_or(0) != detail::CONFIG_VERSION) {
        return;
    }

    for (const auto& [name, value] : config.get_key_values()) {
        if (!name.starts_with(detail::OPEN_NODE_PREFIX) || value != "true") {
            continue;
        }

        uint64_t key{};
        const auto first = name.data() + detail::OPEN_NODE_PREFIX.size();
        const auto last = name.data() + name.size();
        const auto result = std::from_chars(first, last, key);

        if (result.ec == std::errc{} && result.ptr == last) {
            const auto source_value = static_cast<uint8_t>(key >> 56);

            if (source_value < static_cast<uint8_t>(TreeStateSource::Count)) {
                detail::g_tree_state.open_nodes.insert(key);
            }
        }
    }
}

template <typename DrawFunction>
bool persistent_tree_item(
    TreeStateSource source,
    std::string_view stable_id,
    DrawFunction&& draw_function,
    bool can_persist = true) {
    auto* window = ImGui::GetCurrentWindow();
    const auto component = static_cast<uint64_t>(
        window->GetID(stable_id.data(), stable_id.data() + stable_id.size()));
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
