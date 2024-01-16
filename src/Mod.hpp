#pragma once

// Game can't use virtual keys unless the menu is open.
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <vector>
#include <unordered_map>
#include <memory>

#include <imgui.h>
#include <sol/sol.hpp>

#include <sdk/ReClass.hpp>
#include <sdk/Renderer.hpp>
#include "utility/Config.hpp"

#include "REFramework.hpp"

class IModValue {
public:
    using Ptr = std::unique_ptr<IModValue>;

    virtual ~IModValue() {};
    virtual bool draw(std::string_view name) = 0;
    virtual void draw_value(std::string_view name) = 0;
    virtual void config_load(const utility::Config& cfg) = 0;
    virtual void config_save(utility::Config& cfg) = 0;
};

// Convenience classes for imgui
template <typename T>
class ModValue : public IModValue {
public:
    using Ptr = std::unique_ptr<ModValue<T>>;

    static auto create(std::string_view config_name, T default_value = T{}) {
        return std::make_unique<ModValue<T>>(config_name, default_value);
    }

    ModValue(std::string_view config_name, T default_value) 
        : m_config_name{ config_name },
        m_value{ default_value }, 
        m_default_value{ default_value }
    {
    }

    virtual ~ModValue() override {};

    virtual void config_load(const utility::Config& cfg) override {
        auto v = cfg.get<T>(m_config_name);

        if (v) {
            m_value = *v;
        }
    };

    virtual void config_save(utility::Config& cfg) override {
        cfg.set<T>(m_config_name, m_value);
    };

    operator T&() {
        return m_value;
    }

    T& value() {
        return m_value;
    }

    const T& default_value() const {
        return m_default_value;
    }

    const auto& get_config_name() const {
        return m_config_name;
    }

protected:
    T m_value{};
    T m_default_value{};
    std::string m_config_name{ "Default_ModValue" };
};

class ModToggle : public ModValue<bool> {
public:
    using Ptr = std::unique_ptr<ModToggle>;

    ModToggle(std::string_view config_name, bool default_value) 
        : ModValue<bool>{ config_name, default_value } 
    { 
    }

    static auto create(std::string_view config_name, bool default_value = false) {
        return std::make_unique<ModToggle>(config_name, default_value);
    }

    bool draw(std::string_view name) override {
        ImGui::PushID(this);
        auto ret = ImGui::Checkbox(name.data(), &m_value);
        ImGui::PopID();

        return ret;
    }

    void draw_value(std::string_view name) override {
        ImGui::Text("%s: %i", name.data(), m_value);
    }

    bool toggle() {
        return m_value = !m_value;
    }
};

class ModFloat : public ModValue<float> {
public:
    using Ptr = std::unique_ptr<ModFloat>;

    ModFloat(std::string_view config_name, float default_value) 
        : ModValue<float>{ config_name, default_value } { }

    static auto create(std::string_view config_name, float default_value = 0.0f) {
        return std::make_unique<ModFloat>(config_name, default_value);
    }

    bool draw(std::string_view name) override {
        ImGui::PushID(this);
        auto ret = ImGui::InputFloat(name.data(), &m_value);
        ImGui::PopID();

        return ret;
    }

    void draw_value(std::string_view name) override {
        ImGui::Text("%s: %f", name.data(), m_value);
    }
};

class ModSlider : public ModFloat {
public:
    using Ptr = std::unique_ptr<ModSlider>;

    static auto create(std::string_view config_name, float mn = 0.0f, float mx = 1.0f, float default_value = 0.0f) {
        return std::make_unique<ModSlider>(config_name, mn, mx, default_value);
    }

    ModSlider(std::string_view config_name, float mn = 0.0f, float mx = 1.0f, float default_value = 0.0f)
        : ModFloat{ config_name, default_value },
        m_range{ mn, mx }
    {
    }

    bool draw(std::string_view name) override {
        ImGui::PushID(this);
        auto ret = ImGui::SliderFloat(name.data(), &m_value, m_range.x, m_range.y);
        ImGui::PopID();

        return ret;
    }

    void draw_value(std::string_view name) override {
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

    static auto create(std::string_view config_name, uint32_t default_value = 0) {
        return std::make_unique<ModInt32>(config_name, default_value);
    }

    ModInt32(std::string_view config_name, uint32_t default_value = 0)
        : ModValue{ config_name, static_cast<int>(default_value) }
    {
    }

    bool draw(std::string_view name) override {
        ImGui::PushID(this);
        auto ret = ImGui::InputInt(name.data(), &m_value);
        ImGui::PopID();

        return ret;
    }

    void draw_value(std::string_view name) override {
        ImGui::Text("%s: %i", name.data(), m_value);
    }
};

class ModCombo : public ModValue<int32_t> {
public:
    using Ptr = std::unique_ptr<ModCombo>;

    static auto create(std::string_view config_name, std::vector<std::string> options, int32_t default_value = 0) {
        return std::make_unique<ModCombo>(config_name, options, default_value);
    }

    ModCombo(std::string_view config_name, const std::vector<std::string>& options, int32_t default_value = 0)
        : ModValue{ config_name, default_value },
        m_options_stdstr{ options }
    {
        for (auto& o : m_options_stdstr) {
            m_options.push_back(o.c_str());
        }
    }

    bool draw(std::string_view name) override {
        // clamp m_value to valid range
        m_value = std::clamp<int32_t>(m_value, 0, static_cast<int32_t>(m_options.size()) - 1);

        ImGui::PushID(this);
        auto ret = ImGui::Combo(name.data(), &m_value, m_options.data(), static_cast<int32_t>(m_options.size()));
        ImGui::PopID();

        return ret;
    }

    void draw_value(std::string_view name) override {
        m_value = std::clamp<int32_t>(m_value, 0, static_cast<int32_t>(m_options.size()) - 1);

        ImGui::Text("%s: %s", name.data(), m_options[m_value]);
    }

    const auto& options() const {
        return m_options;
    }
    
protected:
    std::vector<const char*> m_options{};
    std::vector<std::string> m_options_stdstr{};
};

class ModKey: public ModInt32 {
public:
    using Ptr = std::unique_ptr<ModKey>;

    static auto create(std::string_view config_name, int32_t default_value = UNBOUND_KEY) {
        return std::make_unique<ModKey>(config_name, default_value);
    }

    ModKey(std::string_view config_name, int32_t default_value = UNBOUND_KEY)
        : ModInt32{ config_name, static_cast<uint32_t>(default_value) }
    {
    }

    bool draw(std::string_view name) override {
        if (name.empty()) {
            return false;
        }

        ImGui::PushID(this);
        ImGui::Button(name.data());

        if (ImGui::IsItemHovered() && ImGui::GetIO().MouseDown[0]) {
            m_waiting_for_new_key = true;
        }

        if (m_waiting_for_new_key) {
            const auto &keys = g_framework->get_keyboard_state();
            for (int32_t k{ 0 }; k < keys.size(); ++k) {
                if (k == VK_LBUTTON || k == VK_RBUTTON) {
                    continue;
                }

                if (keys[k]) {
                    m_value = is_erase_key(k) ? UNBOUND_KEY : k;
                    m_waiting_for_new_key = false;
                    break;
                }
            }

            ImGui::SameLine();
            ImGui::Text("Press any key...");
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

    bool is_key_down() const {
        if (m_value < 0 || m_value > 255) {
            return false;
        }

        if (m_value == VK_LBUTTON || m_value == VK_RBUTTON) {
            return false;
        }

        return g_framework->get_keyboard_state()[(uint8_t)m_value] != 0;
    }

    bool is_key_down_once() {
        auto down = is_key_down();

        if (!m_was_key_down && down) {
            m_was_key_down = true;
            return true;
        }

        if (!down) {
            m_was_key_down = false;
        }

        return false;
    }

    bool is_erase_key(uint8_t k) const {
        switch (k) {
        case VK_ESCAPE:
        case VK_BACK:
            return true;

        default:
            return false;
        }
    }

    static constexpr int32_t UNBOUND_KEY{ -1 };

protected:
    bool m_was_key_down{ false };
    bool m_waiting_for_new_key{ false };
};

// Render layer hooks
#define MAKE_LAYER_CALLBACK(T, x) \
    virtual bool on_pre_##x ##_layer_draw(sdk::renderer::layer::##T * layer, void* render_context) { return true; }; \
    virtual void on_##x ##_layer_draw(sdk::renderer::layer::##T * layer, void* render_context) {}; \
    virtual bool on_pre_ ##x ##_layer_update(sdk::renderer::layer::##T * layer, void* render_context) { return true; }; \
    virtual void on_##x ##_layer_update(sdk::renderer::layer::##T * layer, void* render_context) {};

class Mod {
protected:
    using ValueList = std::vector<std::reference_wrapper<IModValue>>;

public:
    virtual ~Mod() {};
    virtual std::string_view get_name() const { return "UnknownMod"; };

    // can be used for ModValues, like Mod_ValueName
    virtual std::string generate_name(std::string_view name) { return std::string{ get_name() } + "_" + name.data(); }

    // Called when REFramework::initialize finishes in the first render frame
    // Returns an error string if it fails
    virtual std::optional<std::string> on_initialize() { return std::nullopt; };
    virtual std::optional<std::string> on_initialize_d3d_thread() { return std::nullopt; };
    virtual void on_lua_state_created(sol::state& lua) {};
    virtual void on_lua_state_destroyed(sol::state& lua) {};

    // This gets called after updating stuff like keyboard/mouse input to imgui
    // can be used to override these inputs e.g. with a custom input system
    // like VR controllers
    virtual void on_pre_imgui_frame() {};
    // Functionally equivalent, but on_frame will always get called, on_draw_ui can be disabled by REFramework
    virtual void on_frame() {}; // BeginRendering, can be used for imgui
    virtual void on_present() {}; // actual present frame, CANNOT be used for imgui
    virtual void on_post_frame() {}; // after imgui rendering is done
    virtual void on_post_present() {}; // actually after present gets called
    virtual void on_draw_ui() {};
    virtual void on_device_reset() {};
    virtual bool on_message(HWND wnd, UINT message, WPARAM w_param, LPARAM l_param) { return true; };

    virtual void on_config_load(const utility::Config& cfg) {};
    virtual void on_config_save(utility::Config& cfg) {};

    // Game-specific callbacks
    virtual void on_pre_update_transform(RETransform* transform) {};
    virtual void on_update_transform(RETransform* transform) {};
    virtual void on_pre_update_camera_controller(RopewayPlayerCameraController* controller) {};
    virtual void on_update_camera_controller(RopewayPlayerCameraController* controller) {};
    virtual void on_pre_update_camera_controller2(RopewayPlayerCameraController* controller) {};
    virtual void on_update_camera_controller2(RopewayPlayerCameraController* controller) {};
    virtual bool on_pre_gui_draw_element(REComponent* gui_element, void* primitive_context) { return true; };
    virtual void on_gui_draw_element(REComponent* gui_element, void* primitive_context) {};
    virtual void on_pre_update_before_lock_scene(void* ctx) {};
    virtual void on_update_before_lock_scene(void* ctx) {};
    virtual void on_pre_lightshaft_draw(void* shaft, void* render_context) {};
    virtual void on_lightshaft_draw(void* shaft, void* render_context) {};
    virtual void on_pre_view_get_size(REManagedObject* scene_view, float* result) {};
    virtual void on_view_get_size(REManagedObject* scene_view, float* result) {};   
    virtual void on_pre_camera_get_projection_matrix(REManagedObject* camera, Matrix4x4f* result) {};
    virtual void on_camera_get_projection_matrix(REManagedObject* camera, Matrix4x4f* result) {};
    virtual void on_pre_camera_get_view_matrix(REManagedObject* camera, Matrix4x4f* result) {};
    virtual void on_camera_get_view_matrix(REManagedObject* camera, Matrix4x4f* result) {};

    // via.application entry hooks
    // For a list of possible entries, see via.ModuleEntry enum
    virtual void on_pre_application_entry(void* entry, const char* name, size_t hash) {};
    virtual void on_application_entry(void* entry, const char* name, size_t hash) {};

    MAKE_LAYER_CALLBACK(Scene, scene);
    MAKE_LAYER_CALLBACK(PostEffect, post_effect);
    MAKE_LAYER_CALLBACK(Overlay, overlay);
};