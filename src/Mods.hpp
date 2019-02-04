#pragma once

#include "Mod.hpp"
#include "ComponentTraverser.hpp"

class Mods {
public:
    Mods();
    virtual ~Mods() {}

    void onInitialize() const;

    void onFrame() const;
    void onDrawUI() const;

    const auto& getMods() const {
        return m_mods;
    }

    // One of the necessary exposures to refresh entity/component list
    auto getComponentTraverser() const {
        return m_componentTraverser;
    }

private:
    std::vector<std::shared_ptr<Mod>> m_mods;
    std::shared_ptr<ComponentTraverser> m_componentTraverser;
};