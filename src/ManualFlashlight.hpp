#pragma once

#include "utility/Patch.hpp"

#include "Mod.hpp"

// Original founder: SkacikPL (https://github.com/SkacikPL)
// Recreated in REFramework
class ManualFlashlight : public Mod {
public:
    ManualFlashlight();

    std::string_view getName() const override { return "ManualFlashlight"; }

    void onFrame() override;
    void onDrawUI() override;

    void onUpdateTransform(RETransform* transform) override;

private:
    bool m_enabled{ false };
    bool m_lastButton{ false };
    bool m_shouldPullOut{ false };
    RopewayIlluminationManager* m_illuminationManager{ nullptr };

    std::string m_keyButtonName{ "Change Key" };

    // keys need to be directinput unfortunately. window messages for windows virtual keys get blocked
    uint32_t m_key{ DIK_F };
};