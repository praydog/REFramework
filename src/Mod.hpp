#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include <imgui/imgui.h>

#include "ReClass.hpp"

// Convenience classes for imgui
template <typename T>
struct ModValue {
    using Ptr = std::unique_ptr<ModValue<T>>;

    static ModValue::Ptr create(T defaultValue = T{}) {
        return std::make_unique<ModValue<T>>(defaultValue);
    }

    ModValue(T defaultValue) : value{ defaultValue } {}
    virtual ~ModValue() {};
    virtual bool draw(std::string_view name) = 0;
    virtual void drawValue(std::string_view name) = 0;

    operator T() {
        return this->value;
    }

    T value{};
};

struct ModToggle : public ModValue<bool> {
    ModToggle(bool defaultValue) : ModValue<bool>{ defaultValue } { }

    static ModToggle::Ptr create(bool defaultValue = false) {
        return std::make_unique<ModToggle>(defaultValue);
    }

    bool draw(std::string_view name) override {
        return ImGui::Checkbox(name.data(), &this->value);
    }

    void drawValue(std::string_view name) override {
        ImGui::Text("%s: %i", name.data(), this->value);
    }
};

struct ModFloat : public ModValue<float> {
    ModFloat(float defaultValue) : ModValue<float>{ defaultValue } { }

    static ModFloat::Ptr create(float defaultValue = 0.0f) {
        return std::make_unique<ModFloat>(defaultValue);
    }

    bool draw(std::string_view name) override {
        return ImGui::InputFloat(name.data(), &this->value);
    }

    void drawValue(std::string_view name) override {
        ImGui::Text("%s: %f", name.data(), this->value);
    }
};

struct ModSlider : public ModFloat {
    using Ptr = std::unique_ptr<ModSlider>;

    static ModSlider::Ptr create(float mn = 0.0f, float mx = 1.0f, float defaultValue = 0.0f) {
        return std::make_unique<ModSlider>(mn, mx, defaultValue);
    }

    ModSlider(float mn = 0.0f, float mx = 1.0f, float defaultValue = 0.0f)
        : ModFloat{ defaultValue },
        minValue{ mn },
        maxValue{ mx }
    {
    }

    bool draw(std::string_view name) override {
        return ImGui::SliderFloat(name.data(), &this->value, this->minValue, this->maxValue);
    }

    void drawValue(std::string_view name) override {
        ImGui::Text("%s: %f [%f, %f]", name.data(), this->value, this->minValue, this->maxValue);
    }

    float minValue{ 0.0f };
    float maxValue{ 1.0f };
};

class Mod {
public:
    virtual std::string_view getName() const { return "UnknownMod"; };

    // Called when REFramework::initialize finishes in the first render frame
    virtual void onInitialize() {};

    // Functionally equivalent, but onFrame will always get called, onDrawUI can be disabled by REFramework
    virtual void onFrame() {};
    virtual void onDrawUI() {};

    // Game-specific callbacks
    virtual void onPreUpdateTransform(RETransform* transform) {};
    virtual void onUpdateTransform(RETransform* transform) {};
    virtual void onPreUpdateCameraController(RopewayPlayerCameraController* controller) {};
    virtual void onUpdateCameraController(RopewayPlayerCameraController* controller) {};
    virtual void onPreUpdateCameraController2(RopewayPlayerCameraController* controller) {};
    virtual void onUpdateCameraController2(RopewayPlayerCameraController* controller) {};

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

