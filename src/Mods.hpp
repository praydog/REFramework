#pragma once

#include "Mod.hpp"

class Mods {
public:
    Mods();
    virtual ~Mods() {}

    std::optional<std::string> on_initialize() const;

    void on_frame() const;
    void on_draw_ui() const;

    const auto& get_mods() const {
        return m_mods;
    }

private:
    std::vector<std::shared_ptr<Mod>> m_mods;
};