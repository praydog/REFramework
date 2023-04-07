#pragma once

#include "Mod.hpp"

class RenderToneMappingInternal {
public:
	char pad_0000[336]; //0x0000
	bool update_vignette; //0x0150
	char pad_0151[135]; //0x0151
	float vignetting_brightness; //0x01D8
	char pad_01DC[8]; //0x01DC
	int32_t vignetting_mode; //0x01E4
};

class RenderToneMapping : public REComponent {
public:
    RenderToneMappingInternal* toneMappingInternal;
};

class AppPlayerCameraParameter {
public:
    char pad_0000[56]; // 0x0000
    float DefaultFOV;  // 0x0038
    float AimmingFOV;  // 0x003C
};

class AppPlayerCameraConfigurationBase {
public:
    char pad_0000[80];                               // 0x0000
    AppPlayerCameraParameter* playerCameraParameter; // 0x0050
};

class AppPlayerCameraConfiguration {
public:
    char pad_0000[32];                                               // 0x0000
    AppPlayerCameraConfigurationBase* playerCameraConfigurationBase; // 0x0020
};

class AppCameraConfiguration {
public:
    char pad_0000[24];                                       // 0x0000
    AppPlayerCameraConfiguration* playerCameraConfiguration; // 0x0018
};

class AppPlayerConfiguration {
public:
    char pad_0000[232];                          // 0x0000
    AppCameraConfiguration* cameraConfiguration; // 0x00E8
};

class AppPlayerConfigure {
public:
    char pad_0000[80];                           // 0x0000
    AppPlayerConfiguration* playerConfiguration; // 0x0050
};

class Camera : public Mod {
public:
    static std::shared_ptr<Camera>& get() {
        static std::shared_ptr<Camera> instance = std::make_shared<Camera>();
        return instance;
    }

    std::string_view get_name() const override { return "Camera"; };

    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    void on_draw_ui() override;

    void on_update_transform(RETransform* transform);
    void on_pre_application_entry(void* entry, const char* name, size_t hash) override;
    void on_application_entry(void* entry, const char* name, size_t hash) override;

    bool is_disable_vignette() const {
        return m_enabled->value() && m_disable_vignette->value();
    }

private:
    const ModToggle::Ptr m_enabled{ ModToggle::create(generate_name("Enabled"), false) };
    const ModToggle::Ptr m_disable_vignette{ ModToggle::create(generate_name("DisableVignette"), true) };
    const ModSlider::Ptr m_vignette_brightness{ ModSlider::create(generate_name("VignetteBrightness"), -1.0, 1.0, 0.0f ) };

    const ModToggle::Ptr m_use_custom_global_fov{ ModToggle::create(generate_name("GlobalFOVEnabled"), false) };
    const ModSlider::Ptr m_global_fov{ ModSlider::create(generate_name("GlobalFOV"), 0.0f, 180.0f, 81.0f) };

#ifdef RE8
    const ModSlider::Ptr m_fov{ ModSlider::create(generate_name("FOV"), 0.0f, 180.0f, 81.0f) };
    const ModSlider::Ptr m_fov_aiming{ ModSlider::create(generate_name("FOVAiming"), 0.0f, 180.0f, 70.0f) };
#endif

    ValueList m_options{
        *m_enabled,
        *m_disable_vignette,
        *m_vignette_brightness,
        *m_use_custom_global_fov,
        *m_global_fov,

#ifdef RE8
        *m_fov,
        *m_fov_aiming
#endif
    };

#ifdef RE8
    AppPropsManager* m_props_manager{ nullptr };
#endif
    RECamera* m_camera{ nullptr };
    REGameObject* m_player{ nullptr };
    RenderToneMapping* m_tone_map{ nullptr };
    RenderToneMappingInternal* m_tone_map_internal{ nullptr };
    AppPlayerConfigure* m_player_configure{ nullptr };
    AppPlayerCameraParameter* m_player_camera_params{ nullptr };

    template<class T>
    inline bool reset_ptr(T* &m, T* ptr, const std::function< void (bool) > &on_change = {}) {
        if (m != ptr) {
            m = ptr;

            if (on_change) {
                on_change(m != nullptr);
            }
        }

        return m != nullptr;
    }

    void update_vignetting() noexcept;
    void on_player_transform(RETransform* transform) noexcept;
    void on_disabled() noexcept;

    void set_vignette_type(via::render::ToneMapping::Vignetting value) noexcept;
    void set_vignette_brightness(float value) noexcept;
    void set_fov(float fov, float aiming_fov) noexcept;
};
