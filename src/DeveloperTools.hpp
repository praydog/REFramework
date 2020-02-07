#pragma once

#include "Mod.hpp"

class DeveloperTools : public Mod {
public:
    DeveloperTools();

    std::string_view get_name() const override { return "DeveloperTools"; };

    // Only one we need right now.
    void on_draw_ui() override;

private:
    std::vector<std::shared_ptr<Mod>> m_tools;
};