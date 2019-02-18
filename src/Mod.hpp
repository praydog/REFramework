#pragma once

// Game can't use virtual keys unless the menu is open.
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <vector>
#include <unordered_map>
#include <memory>

#include <imgui/imgui.h>

#include "sdk/ReClass.hpp"
#include "utility/Config.hpp"

#include "REFramework.hpp"

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

    virtual void configLoad(const utility::Config& cfg, std::string_view name) {
        auto v = cfg.get<T>(name.data());

        if (v) {
            this->value = *v;
        }
    };

    virtual void configSave(utility::Config& cfg, std::string_view name) {
        cfg.set<T>(name.data(), this->value);
    };

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
        ImGui::PushID(this);
        auto ret = ImGui::Checkbox(name.data(), &this->value);
        ImGui::PopID();

        return ret;
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
        ImGui::PushID(this);
        auto ret = ImGui::InputFloat(name.data(), &this->value);
        ImGui::PopID();

        return ret;
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
        ImGui::PushID(this);
        auto ret = ImGui::SliderFloat(name.data(), &this->value, this->minValue, this->maxValue);
        ImGui::PopID();

        return ret;
    }

    void drawValue(std::string_view name) override {
        ImGui::Text("%s: %f [%f, %f]", name.data(), this->value, this->minValue, this->maxValue);
    }

    float minValue{ 0.0f };
    float maxValue{ 1.0f };
};

struct ModInt32 : public ModValue<int32_t> {
    using Ptr = std::unique_ptr<ModInt32>;

    static ModInt32::Ptr create(uint32_t defaultValue = 0) {
        return std::make_unique<ModInt32>(defaultValue);
    }

    ModInt32(uint32_t defaultValue = 0)
        : ModValue{ defaultValue }
    {
    }

    bool draw(std::string_view name) override {
        ImGui::PushID(this);
        auto ret = ImGui::InputInt(name.data(), &this->value);
        ImGui::PopID();

        return ret;
    }

    void drawValue(std::string_view name) override {
        ImGui::Text("%s: %i", name.data(), this->value);
    }
};

struct ModKey : public ModInt32 {
    using Ptr = std::unique_ptr<ModKey>;

    static ModKey::Ptr create(uint8_t defaultValue = 0) {
        return std::make_unique<ModKey>(defaultValue);
    }

    ModKey(uint8_t defaultValue = 0)
        : ModInt32{ defaultValue }
    {
    }

    bool draw(std::string_view name) override {
        if (name.empty()) {
            return false;
        }

        ImGui::PushID(this);
        ImGui::Button(name.data());

        if (ImGui::IsItemHovered()) {
            auto& keys = g_framework->getKeyboardState();

            for (auto k = 0; k < keys.size(); ++k) {
                if (keys[k]) {
                    this->value = k;
                    break;
                }
            }

            ImGui::SameLine();
            ImGui::Text("Press any key", this->value);
        }
        else {
            ImGui::SameLine();
            ImGui::Text("%i", this->value);
        }

        ImGui::PopID();

        return true;
    }

    bool isKeyDown() {
        return g_framework->getKeyboardState()[(uint8_t)this->value] != 0;
    }
};

class Mod {
public:
    virtual std::string_view getName() const { return "UnknownMod"; };

    // can be used for ModValues, like Mod_ValueName
    virtual std::string generateName(std::string_view name) { return std::string{ getName() } + "_" + name.data(); }

    // Called when REFramework::initialize finishes in the first render frame
    // Returns an error string if it fails
    virtual std::optional<std::string> onInitialize() { return std::nullopt; };

    // Functionally equivalent, but onFrame will always get called, onDrawUI can be disabled by REFramework
    virtual void onFrame() {};
    virtual void onDrawUI() {};

    virtual void onConfigLoad(const utility::Config& cfg) {};
    virtual void onConfigSave(utility::Config& cfg) {};

    // Game-specific callbacks
    virtual void onPreUpdateTransform(RETransform* transform) {};
    virtual void onUpdateTransform(RETransform* transform) {};
    virtual void onPreUpdateCameraController(RopewayPlayerCameraController* controller) {};
    virtual void onUpdateCameraController(RopewayPlayerCameraController* controller) {};
    virtual void onPreUpdateCameraController2(RopewayPlayerCameraController* controller) {};
    virtual void onUpdateCameraController2(RopewayPlayerCameraController* controller) {};
};

