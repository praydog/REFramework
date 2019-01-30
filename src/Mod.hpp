#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "ReClass.hpp"

template <typename T>
struct ModValue {
    using Ptr = std::unique_ptr<ModValue<T>>;

    static ModValue::Ptr create() {
        return std::make_unique<ModValue<T>>();
    }

    operator T() {
        return value;
    }

    T value{};
};

struct ModToggle : public ModValue<bool> {

};

struct ModSlider : public ModValue<float> {
    using Ptr = std::unique_ptr<ModSlider>;

    static ModSlider::Ptr create(float mn = 0.0f, float mx = 1.0f) {
        return std::make_unique<ModSlider>(mn, mx);
    }

    ModSlider(float mn = 0.0f, float mx = 1.0f)
        : minValue{ mn },
        maxValue{ mx }
    {

    }

    float minValue{ 0.0f };
    float maxValue{ 1.0f };
};

class Mod {
public:
    // Functionally equivalent, but onFrame will always get called, onDrawUI can be disabled by REFramework
    virtual void onFrame() {};
    virtual void onDrawUI() {};

    virtual void onUpdateTransform(RETransform* transform) {};
    virtual void onGetMainCamera(RECamera* camera) {};
    virtual void onComponent(REComponent* component) {};

    const auto& getToggles() const {
        return m_toggles;
    }

    const auto& getSliders() const {
        return m_sliders;
    }

protected:
    std::unordered_map<std::string, ModToggle::Ptr> m_toggles;
    std::unordered_map<std::string, ModSlider::Ptr> m_sliders;
};

