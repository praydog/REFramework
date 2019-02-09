#pragma once

#include "utility/Patch.hpp"

#include "Mod.hpp"

// Original founder: SkacikPL (https://github.com/SkacikPL)
// Recreated in REFramework
class ManualFlashlight : public Mod {
public:
    std::string_view getName() const override { return "ManualFlashlight"; }

    void onFrame() override;
    void onDrawUI() override;

    void onConfigLoad(const utility::Config& cfg) override;
    void onConfigSave(utility::Config& cfg) override;

    void onUpdateTransform(RETransform* transform) override;

private:
    RopewayIlluminationManager* m_illuminationManager{ nullptr };

    bool m_lastButton{ false };
    bool m_shouldPullOut{ false };

    // keys need to be directinput unfortunately. window messages for windows virtual keys get blocked
    ModKey::Ptr m_key{ ModKey::create(DIK_F) };
    ModToggle::Ptr m_enabled{ ModToggle::create(false) };
};