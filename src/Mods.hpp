#pragma once

#include "Mod.hpp"

class Mods {
public:
    Mods();
    virtual ~Mods() {}

    std::optional<std::string> on_initialize() const;
    std::optional<std::string> on_initialize_d3d_thread() const;

    void on_pre_imgui_frame() const;
    void on_frame() const;
    void on_present() const;
    void on_post_frame() const;
    void on_draw_ui() const;
    void on_device_reset() const;

    const auto& get_mods() const {
        return m_mods;
    }

private:
    std::vector<std::shared_ptr<Mod>> m_mods;
};