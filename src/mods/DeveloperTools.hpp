#pragma once

#include "Mod.hpp"
#include "Tool.hpp"

class DeveloperTools : public Mod {
public:
    DeveloperTools();

    std::string_view get_name() const override { return "DeveloperTools"; };

    // Only one we need right now.
    void on_draw_ui() override;

    const std::vector<std::shared_ptr<Tool>>& get_tools() const {
        return m_tools;
    }

private:
    std::vector<std::shared_ptr<Tool>> m_tools;
};