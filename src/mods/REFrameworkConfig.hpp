#pragma once

#include "Mod.hpp"

class REFrameworkConfig : public Mod {
public:
    static std::shared_ptr<REFrameworkConfig>& get();

public:
    std::string_view get_name() const {
        return "REFrameworkConfig";
    }

    std::optional<std::string> on_initialize() override;
    void on_draw_ui() override;
    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    auto& get_menu_key() {
        return m_menu_key;
    }

    auto& get_menu_open() {
        return m_menu_open;
    }

private:
    ModKey::Ptr m_menu_key{ ModKey::create(generate_name("MenuKey"), DIK_INSERT) };
    ModToggle::Ptr m_menu_open{ ModToggle::create(generate_name("MenuOpen"), true) };
    ModToggle::Ptr m_remember_menu_state{ ModToggle::create(generate_name("RememberMenuState"), false) };

    ValueList m_options {
        *m_menu_key,
        *m_menu_open,
        *m_remember_menu_state
    };
};