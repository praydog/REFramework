#pragma once

#include <chrono>
#include <unordered_map>

#include <sdk/intrusive_ptr.hpp>
#include <sdk/ManagedObject.hpp>
#include <sdk/renderer/PipelineState.hpp>

#include "Mod.hpp"

class REManagedObject;
class REComponent;

class Graphics : public Mod {
public:
    static std::shared_ptr<Graphics>& get();

public:
    std::string_view get_name() const override { return "Graphics"; };

    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    void on_frame() override;
    void on_draw_ui() override;
    void on_present() override;

    void on_pre_application_entry(void* entry, const char* name, size_t hash) override;
    void on_application_entry(void* entry, const char* name, size_t hash) override;

    bool on_pre_gui_draw_element(REComponent* gui_element, void* primitive_context) override;
    void on_view_get_size(REManagedObject* scene_view, float* result) override;

    void on_scene_layer_update(sdk::renderer::layer::Scene* layer, void* render_context) override;

private:
    void fix_ui_element(REComponent* gui_element);
    void do_scope_tweaks(sdk::renderer::layer::Scene* layer);
    void do_ultrawide_fix();
    void do_ultrawide_fov_restore(bool force = false);
    void set_ultrawide_fov(bool enable);

#if TDB_VER >= 69
    void setup_path_trace_hook();
    void setup_shader_interception_hook();
    void setup_rt_component();
    void apply_ray_tracing_tweaks();

    static void* rt_draw_hook(REComponent* rt, void* draw_context, void* r8, void* r9);
    static void* rt_draw_impl_hook(void* rt_impl, void* draw_context, void* r8, void* r9, void* unk);
    static sdk::renderer::PipelineState* find_pipeline_state_hook(void* shader_resource, uint32_t murmur_hash, void* unk);

    static inline std::string make_replacement_shader() {
        std::string result{};
        result.resize(1024);
        return result;
    }

    enum ShaderDispatchMode {
        Dispatch,
        Dispatch32BitConstant,
        DispatchRay
    };

    static const inline std::array<const char*, 3> s_shader_dispatch_modes {
        "Dispatch",
        "Dispatch32BitConstant",
        "DispatchRay"
    };

    struct ReplacementShader {
        std::string shader{[]() {
            std::string result{};
            result.resize(1024);
            return result;
        }()};

        uint32_t hash{ 0 };
        ShaderDispatchMode dispatch_mode{ Dispatch };

        uint32_t thread_group_x{ 2560 };
        uint32_t thread_group_y{ 1440 };
        uint32_t thread_group_z{ 1 };
        uint32_t constant{0};

        bool valid_hash{false};
    };

    struct InterceptedShader {
        std::array<ReplacementShader, 8> replacement_shaders{};

        std::string name = []() {
            std::string result{};
            result.resize(1024);
            return result;
        }();

        uint32_t hash{ 0 };
    };

    std::array<InterceptedShader, 8> m_intercepted_shaders{};
    bool is_intercepted(uint32_t hash) {
        for (const auto& shader : m_intercepted_shaders) {
            if (shader.hash == hash) {
                return true;
            }
        }

        return false;
    }

    InterceptedShader* get_intercepted(uint32_t hash) {
        for (auto& shader : m_intercepted_shaders) {
            if (shader.hash == hash) {
                return &shader;
            }
        }

        return nullptr;
    }

    bool m_disable_path_space{false};

    std::unique_ptr<FunctionHook> m_rt_draw_hook{};
    std::unique_ptr<FunctionHook> m_rt_draw_impl_hook{};
    std::unique_ptr<FunctionHook> m_find_pipeline_state_hook{};
    bool m_attempted_path_trace_hook{ false };
    bool m_attempted_shader_interception_hook{ false };

    std::optional<size_t> m_rt_type_offset{};
    sdk::intrusive_ptr<sdk::ManagedObject> m_rt_component{};
    sdk::intrusive_ptr<sdk::ManagedObject> m_rt_cloned_component{};
    void* m_pt_pipeline_resource{nullptr};
    void* m_dxr_shader_resource{nullptr};

    struct {
        void* impl;
        void* context;
        void* r8;
        void* r9;
        void* unk;
    } m_rt_draw_args{};

    bool m_within_rt_draw{ false };
    bool m_cloning_dispatch{false};
#endif

    std::recursive_mutex m_fov_mutex{};
    std::unordered_map<::REManagedObject*, float> m_fov_map{};
    std::unordered_map<::REManagedObject*, bool> m_vertical_fov_map{};

    struct {
        std::shared_mutex time_mtx{};
        std::chrono::steady_clock::time_point last_inventory_open{};
    } m_re4;
    
    const ModToggle::Ptr m_ultrawide_fix{ ModToggle::create(generate_name("UltrawideFix"), false) };
    const ModToggle::Ptr m_ultrawide_vertical_fov{ ModToggle::create(generate_name("UltrawideFixVerticalFOV_V2"), false) };
    const ModToggle::Ptr m_ultrawide_custom_fov{ModToggle::create(generate_name("UltrawideCustomFOV"), false)};
    const ModSlider::Ptr m_ultrawide_fov_multiplier{ ModSlider::create(generate_name("UltrawideFOVMultiplier_V2"), 0.01f, 3.0f, 1.0f) };
    const ModToggle::Ptr m_disable_gui{ ModToggle::create(generate_name("DisableGUI"), false) };
    const ModToggle::Ptr m_force_render_res_to_window{ ModToggle::create(generate_name("ForceRenderResToWindow"), false) };
    const ModKey::Ptr m_disable_gui_key{ ModKey::create(generate_name("DisableGUIKey")) };

#if TDB_VER >= 69
    const ModToggle::Ptr m_shader_playground { ModToggle::create(generate_name("ShaderPlayground"), false) };
    const ModToggle::Ptr m_ray_tracing_tweaks { ModToggle::create(generate_name("RayTracingTweaks"), false) };

    enum class RayTraceType : uint8_t {
        Disabled = 0,
        AmbientOcclusion = 1,
        Hybrid = 2,
        Pure = 3,
        PathSpaceFilter = 4,
        ScreenSpacePhotonMapping = 5,
        Debug = 6,
        ASVGF = 7,
    };

    static const inline std::vector<std::string> s_ray_trace_type {
        "Disabled",
        "Ambient Occlusion",
        "Hybrid Path Tracing",
        "Pure Path Tracing",
        "Path Space Filter",
        "Screen Space Photon Mapping",
        "Debug",
        "ASVGF",
    };

    static const inline std::vector<std::string> s_bounce_count {
        "0",
        "1",
        "2",
        "3",
        "7"
    };

    static const inline std::vector<std::string> s_samples_per_pixel {
        "1",
        "2",
        "4"
    };

    const ModCombo::Ptr m_ray_trace_type{ ModCombo::create(generate_name("RayTraceType"), s_ray_trace_type) };
    const ModToggle::Ptr m_ray_trace_disable_raster_shadows{ ModToggle::create(generate_name("RayTraceDisableRasterShadowsWithPT"), true) };
    bool m_was_shadows_disabled{ false };
    const ModCombo::Ptr m_ray_trace_clone_type_true{ ModCombo::create(generate_name("RayTraceTrueCloneType"), s_ray_trace_type) };
    const ModCombo::Ptr m_ray_trace_clone_type_pre{ ModCombo::create(generate_name("RayTraceCloneTypePre"), s_ray_trace_type) };
    const ModCombo::Ptr m_ray_trace_clone_type_post{ ModCombo::create(generate_name("RayTraceCloneTypePost"), s_ray_trace_type) };

    const ModCombo::Ptr m_bounce_count{ ModCombo::create(generate_name("BounceCount"), s_bounce_count, 1) };
    const ModCombo::Ptr m_samples_per_pixel{ ModCombo::create(generate_name("SamplesPerPixel"), s_samples_per_pixel, 1) };
#endif

#ifdef RE4
    const ModToggle::Ptr m_scope_tweaks{ ModToggle::create(generate_name("ScopeTweaks"), false) };
    const ModToggle::Ptr m_scope_interlaced_rendering{ ModToggle::create(generate_name("ScopeInterlacedRendering"), false) };
    const ModSlider::Ptr m_scope_image_quality{ ModSlider::create(generate_name("ScopeImageQuality"), 0.01f, 2.0f, 1.0f) };
#endif

    std::optional<std::array<uint32_t, 2>> m_backbuffer_size{};

    ValueList m_options{
        *m_ultrawide_fix,
        *m_ultrawide_vertical_fov,
        *m_ultrawide_custom_fov,
        *m_ultrawide_fov_multiplier,
        *m_disable_gui,
        *m_force_render_res_to_window,
        *m_disable_gui_key,

#if TDB_VER >= 69
        *m_shader_playground,
        *m_ray_tracing_tweaks,
        *m_ray_trace_type,
        *m_ray_trace_disable_raster_shadows,
        *m_ray_trace_clone_type_true,
        *m_ray_trace_clone_type_pre,
        *m_ray_trace_clone_type_post,
        *m_bounce_count,
        *m_samples_per_pixel,
#endif

#ifdef RE4
        *m_scope_tweaks,
        *m_scope_interlaced_rendering,
        *m_scope_image_quality,
#endif
    };
};
