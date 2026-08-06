#pragma once

#include "Mod.hpp"

class REFrameworkConfig : public Mod {
public:
    static inline constexpr std::string_view REFRAMEWORK_CONFIG_NAME{ "re2_fw_config.txt" };
    static inline constexpr std::string_view UI_MONITOR_WIDTH_CONFIG_NAME{ "REFrameworkConfig_UIMonitorWidth" };
    static inline constexpr std::string_view UI_MONITOR_HEIGHT_CONFIG_NAME{ "REFrameworkConfig_UIMonitorHeight" };
    static inline constexpr std::string_view UI_FONT_SIZE_CONFIG_NAME{ "REFrameworkConfig_UIFontSize" };
    static std::shared_ptr<REFrameworkConfig>& get();

public:
    std::string_view get_name() const override {
        return "REFrameworkConfig";
    }

    std::optional<std::string> on_initialize() override;
    void on_draw_ui() override;
    void on_frame() override;
    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    auto& get_menu_key() {
        return m_menu_key;
    }

    auto& get_menu_open() {
        return m_menu_open;
    }

    bool is_always_show_cursor() const {
        return m_always_show_cursor->value();
    }

    void set_ui_layout_state(int32_t width, int32_t height, float font_size) {
        m_ui_monitor_width->value() = width;
        m_ui_monitor_height->value() = height;
        m_ui_font_size->value() = font_size;
        m_font_size->value() = static_cast<int32_t>(font_size + 0.5f);
    }

private:
    ModKey::Ptr m_menu_key{ ModKey::create(generate_name("MenuKey_V2"), VK_INSERT) };
    ModToggle::Ptr m_menu_open{ ModToggle::create(generate_name("MenuOpen"), true) };
    ModToggle::Ptr m_remember_menu_state{ ModToggle::create(generate_name("RememberMenuState"), false) };
#if defined(RE8) && !defined(REFRAMEWORK_UNIVERSAL)
    ModToggle::Ptr m_always_show_cursor{ ModToggle::create(generate_name("DrawCursorWithMenuOpen"), true) };
#else
    ModToggle::Ptr m_always_show_cursor{ ModToggle::create(generate_name("DrawCursorWithMenuOpen"), false) };
#endif
    ModKey::Ptr m_show_cursor_key{ ModKey::create(generate_name("ShowCursorKey")) };
    ModInt32::Ptr m_font_size{ModInt32::create(generate_name("FontSize"), 16)};
    ModInt32::Ptr m_ui_monitor_width{ModInt32::create(UI_MONITOR_WIDTH_CONFIG_NAME, 0)};
    ModInt32::Ptr m_ui_monitor_height{ModInt32::create(UI_MONITOR_HEIGHT_CONFIG_NAME, 0)};
    ModFloat::Ptr m_ui_font_size{ModFloat::create(UI_FONT_SIZE_CONFIG_NAME, 0.0f)};
    std::vector<std::string> fonts{};
    ModComboString::Ptr m_font_file{};

    ValueList m_options {
        *m_menu_key,
        *m_menu_open,
        *m_remember_menu_state,
        *m_always_show_cursor,
        *m_show_cursor_key,
        *m_font_size,
        *m_ui_monitor_width,
        *m_ui_monitor_height,
        *m_ui_font_size,
    };
};
