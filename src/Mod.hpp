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

class IModValue {
public:
    using Ptr = std::unique_ptr<IModValue>;

    virtual ~IModValue() {};
    virtual bool draw(std::string_view name) = 0;
    virtual void drawValue(std::string_view name) = 0;
    virtual void configLoad(const utility::Config& cfg) = 0;
    virtual void configSave(utility::Config& cfg) = 0;
};

// Convenience classes for imgui
template <typename T>
class ModValue : public IModValue {
public:
    using Ptr = std::unique_ptr<ModValue<T>>;

    static auto create(std::string_view configName, T defaultValue = T{}) {
        return std::make_unique<ModValue<T>>(configName, defaultValue);
    }

    ModValue(std::string_view configName, T defaultValue) 
        : m_configName{ configName },
        m_value{ defaultValue }
    {
    }

    virtual ~ModValue() override {};
    virtual bool draw(std::string_view name) = 0;
    virtual void drawValue(std::string_view name) = 0;

    virtual void configLoad(const utility::Config& cfg) override {
        auto v = cfg.get<T>(m_configName);

        if (v) {
            m_value = *v;
        }
    };

    virtual void configSave(utility::Config& cfg) override {
        cfg.set<T>(m_configName, m_value);
    };

    operator IModValue&() {
        return *(IModValue*)this;
    }

    operator T&() {
        return m_value;
    }

    T& value() {
        return m_value;
    }

    const auto& getConfigName() const {
        return m_configName;
    }

protected:
    T m_value{};
    std::string m_configName{ "Default_ModValue" };
};

class ModToggle : public ModValue<bool> {
public:
    using Ptr = std::unique_ptr<ModToggle>;

    ModToggle(std::string_view configName, bool defaultValue) 
        : ModValue<bool>{ configName, defaultValue } 
    { 
    }

    static auto create(std::string_view configName, bool defaultValue = false) {
        return std::make_unique<ModToggle>(configName, defaultValue);
    }

    bool draw(std::string_view name) override {
        ImGui::PushID(this);
        auto ret = ImGui::Checkbox(name.data(), &m_value);
        ImGui::PopID();

        return ret;
    }

    void drawValue(std::string_view name) override {
        ImGui::Text("%s: %i", name.data(), m_value);
    }

    bool toggle() {
        return m_value = !m_value;
    }
};

class ModFloat : public ModValue<float> {
public:
    using Ptr = std::unique_ptr<ModFloat>;

    ModFloat(std::string_view configName, float defaultValue) 
        : ModValue<float>{ configName, defaultValue } { }

    static auto create(std::string_view configName, float defaultValue = 0.0f) {
        return std::make_unique<ModFloat>(configName, defaultValue);
    }

    bool draw(std::string_view name) override {
        ImGui::PushID(this);
        auto ret = ImGui::InputFloat(name.data(), &m_value);
        ImGui::PopID();

        return ret;
    }

    void drawValue(std::string_view name) override {
        ImGui::Text("%s: %f", name.data(), m_value);
    }
};

class ModSlider : public ModFloat {
public:
    using Ptr = std::unique_ptr<ModSlider>;

    static auto create(std::string_view configName, float mn = 0.0f, float mx = 1.0f, float defaultValue = 0.0f) {
        return std::make_unique<ModSlider>(configName, mn, mx, defaultValue);
    }

    ModSlider(std::string_view configName, float mn = 0.0f, float mx = 1.0f, float defaultValue = 0.0f)
        : ModFloat{ configName, defaultValue },
        m_range{ mn, mx }
    {
    }

    bool draw(std::string_view name) override {
        ImGui::PushID(this);
        auto ret = ImGui::SliderFloat(name.data(), &m_value, m_range.x, m_range.y);
        ImGui::PopID();

        return ret;
    }

    void drawValue(std::string_view name) override {
        ImGui::Text("%s: %f [%f, %f]", name.data(), m_value, m_range.x, m_range.y);
    }

    auto& range() {
        return m_range;
    }

protected:
    Vector2f m_range{ 0.0f, 1.0f };
};

class ModInt32 : public ModValue<int32_t> {
public:
    using Ptr = std::unique_ptr<ModInt32>;

    static auto create(std::string_view configName, uint32_t defaultValue = 0) {
        return std::make_unique<ModInt32>(configName, defaultValue);
    }

    ModInt32(std::string_view configName, uint32_t defaultValue = 0)
        : ModValue{ configName, defaultValue }
    {
    }

    bool draw(std::string_view name) override {
        ImGui::PushID(this);
        auto ret = ImGui::InputInt(name.data(), &m_value);
        ImGui::PopID();

        return ret;
    }

    void drawValue(std::string_view name) override {
        ImGui::Text("%s: %i", name.data(), m_value);
    }
};

class ModKey: public ModInt32 {
public:
    using Ptr = std::unique_ptr<ModKey>;

    static auto create(std::string_view configName, int32_t defaultValue = UNBOUND_KEY) {
        return std::make_unique<ModKey>(configName, defaultValue);
    }

    ModKey(std::string_view configName, int32_t defaultValue = UNBOUND_KEY)
        : ModInt32{ configName, defaultValue }
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
                    m_value = isEraseKey(k) ? UNBOUND_KEY : k;
                    break;
                }
            }

            ImGui::SameLine();
            ImGui::Text("Press any key", m_value);
        }
        else {
            ImGui::SameLine();

            if (m_value >= 0 && m_value <= 255) {
                ImGui::Text("%i", m_value);
            }
            else {
                ImGui::Text("Not bound");
            }
        }

        ImGui::PopID();

        return true;
    }

    bool isKeyDown() const {
        if (m_value < 0 || m_value > 255) {
            return false;
        }

        return g_framework->getKeyboardState()[(uint8_t)m_value] != 0;
    }

    bool isKeyDownOnce() {
        auto down = isKeyDown();

        if (!m_wasKeyDown && down) {
            m_wasKeyDown = true;
            return true;
        }
        else if (!down) {
            m_wasKeyDown = false;
        }

        return false;
    }

    bool isEraseKey(int k) const {
        switch (k) {
        case DIK_ESCAPE:
        case DIK_BACKSPACE:
            return true;

        default:
            return false;
        }
    }

    static constexpr int32_t UNBOUND_KEY = -1;

protected:
    bool m_wasKeyDown{ false };
};

class Mod {
protected:
    using ValueList = std::vector<std::reference_wrapper<IModValue>>;

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

