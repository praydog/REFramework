#pragma once

#include "Mod.hpp"

// Calls Mod::onComponent for each entity/component
class ComponentTraverser : public Mod {
public:
    void onDrawUI() override;
    void onGetMainCamera(RECamera* camera) override;
    void refresh();

    static void refreshComponents();

private:
    bool m_issueRefresh{ false };
    uint32_t m_timesRefreshed{ 0 };
};