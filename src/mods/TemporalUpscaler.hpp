#pragma once

#include <array>

#include "vr/D3D12Component.hpp"
#include "Mod.hpp"

namespace sdk {
namespace renderer {
class RenderLayer;

namespace layer {
class Scene;
}
}
}

class TemporalUpscaler : public Mod {
public:
    static std::shared_ptr<TemporalUpscaler>& get();

    std::string_view get_name() const { return "TemporalUpscaler"; }

    std::optional<std::string> on_initialize() override;

    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    void on_draw_ui() override;
    void on_early_present() override; // early because it needs to run before VR.
    void on_post_present() override;
    void on_device_reset() override;

    void on_pre_application_entry(void* entry, const char* name, size_t hash) override;
    void on_application_entry(void* entry, const char* name, size_t hash) override;

    void on_view_get_size(REManagedObject* scene_view, float* result) override;
    void on_camera_get_projection_matrix(REManagedObject* camera, Matrix4x4f* result) override;

    void on_scene_layer_update(sdk::renderer::layer::Scene* scene_layer, void* render_context) override;

    bool ready() const {
        return m_initialized && m_backend_loaded && m_enabled && !m_wants_reinitialize;
    }

    uint32_t get_evaluate_id(uint32_t counter) {
        return (counter % 2) + 1;
    }

    auto& get_old_vp(uint32_t counter) {
        return m_old_projections[counter % 2];
    }

    template<typename T>
    T* get_upscaled_texture(int32_t index) {
        if (index < 0 || index > m_upscaled_textures.size()) {
            return nullptr;
        }

        return (T*)m_upscaled_textures[index];
    }

    enum PDGraphicsAPI {
        D3D11,
        D3D12,
        VULKAN
    };

    enum PDUpscaleType {
        DLSS,
        FSR2,
        XESS
    };

    enum PDPerfQualityLevel {
        Performance,
        Balanced,
        Quality,
        UltraPerformance
    };


private:
    template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    bool on_first_frame();
    bool init_upscale_features();
    void release_upscale_features();

    bool m_first_frame_finished{false};
    bool m_initialized{false};
    bool m_is_d3d12{false};
    bool m_backend_loaded{false};
    bool m_backbuffer_inconsistency{false};
    bool m_enabled{true};
    bool m_upscale{true};
    bool m_rendering{false};
    bool m_set_view{false};
    bool m_jitter{true};
    bool m_sharpness{true};
    bool m_allow_taa{false}; // the engine has its own TAA implementation, it can't be used with the upscaler
    bool m_wants_reinitialize{false};

    std::unordered_map<std::string, size_t> m_available_upscale_methods{};
    std::vector<std::string> m_available_upscale_method_names{};
    std::array<uint32_t, 2> m_jitter_indices{0, 0};

    uint32_t m_available_upscale_type{0};
    PDUpscaleType m_upscale_type{PDUpscaleType::FSR2};
    PDPerfQualityLevel m_upscale_quality{PDPerfQualityLevel::Balanced};

    uint32_t m_backbuffer_inconsistency_start{};
    std::array<uint32_t, 2> m_backbuffer_size{};

    std::array<void*, 2> m_upscaled_textures{nullptr, nullptr};
    sdk::renderer::layer::Scene* m_scene_layer{nullptr};
    sdk::renderer::layer::Output* m_output_layer{nullptr};
    void* m_motion_vectors{nullptr};
    void* m_depth{nullptr};
    void* m_color{nullptr};

    float m_nearz{0.0f};
    float m_farz{0.0f};
    float m_fov{90.0f};
    float m_sharpness_amount{0.0f};

    float m_jitter_offsets[2][2]{0.0f, 0.0f};
    float m_jitter_scale[2]{2.0f, -2.0f};
    float m_motion_scale[2]{-1.0f, 1.0f};
    float m_jitter_evaluate_scale{1.0f};

    vrmod::D3D12Component::ResourceCopier m_copier{};
    ComPtr<ID3D12Resource> m_old_backbuffer{};

    std::array<ComPtr<ID3D12Resource>, 2> m_prev_motion_vectors_d3d12{};
    std::array<Matrix4x4f, 2> m_old_projections{};
};