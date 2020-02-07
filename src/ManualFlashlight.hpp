#pragma once

#include "utility/Patch.hpp"

#include "Mod.hpp"

// Original founder: SkacikPL (https://github.com/SkacikPL)
// Recreated in REFramework
class ManualFlashlight : public Mod {
public:
    std::string_view get_name() const override { return "ManualFlashlight"; }

    void on_frame() override;
    void on_draw_ui() override;

    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    void on_update_transform(RETransform* transform) override;

private:
    RopewayIlluminationManager* m_illumination_manager{ nullptr };

    bool m_should_pull_out{ false };

    // keys need to be directinput unfortunately. window messages for windows virtual keys get blocked
    const ModKey::Ptr m_key{ ModKey::create(generate_name("Key"), DIK_F) };
    const ModToggle::Ptr m_enabled{ ModToggle::create(generate_name("Enabled"), false) };

    ValueList m_options{
        *m_key,
        *m_enabled
    };
};