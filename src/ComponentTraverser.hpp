#pragma once

#include <chrono>

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

    // Limit the amount of times this is called because there's a lot of entities
    // SO, TODO: find a way to do OnEntityCreate/Remove
    std::chrono::system_clock::time_point m_nextTime{};
};