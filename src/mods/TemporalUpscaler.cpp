#include <d3d11.h>
#include <d3d12.h>
#include <wrl.h>

#include <utility/Module.hpp>
#include <PDPerfPlugin.h>

#include <sdk/Renderer.hpp>
#include <sdk/SceneManager.hpp>

#if TDB_VER <= 49
#include "sdk/regenny/re7/via/Window.hpp"
#include "sdk/regenny/re7/via/SceneView.hpp"
#elif TDB_VER < 69
#include "sdk/regenny/re3/via/Window.hpp"
#include "sdk/regenny/re3/via/SceneView.hpp"
#elif TDB_VER == 69
#include "sdk/regenny/re8/via/Window.hpp"
#include "sdk/regenny/re8/via/SceneView.hpp"
#elif TDB_VER == 70
#include "sdk/regenny/re2_tdb70/via/Window.hpp"
#include "sdk/regenny/re2_tdb70/via/SceneView.hpp"
#elif TDB_VER >= 71
#include "sdk/regenny/re8/via/Window.hpp"
#include "sdk/regenny/re8/via/SceneView.hpp"
#endif

#include "TemporalUpscaler.hpp"

#include "VR.hpp"

std::shared_ptr<TemporalUpscaler>& TemporalUpscaler::get() {
    static std::shared_ptr instance = std::make_shared<TemporalUpscaler>();
    return instance;
}

std::optional<std::string> TemporalUpscaler::on_initialize() {
    m_backend_loaded = GetModuleHandleA("PDPerfPlugin.dll") != nullptr ||
                       utility::load_module_from_current_directory(L"PDPerfPlugin.dll") != nullptr;

    if (!m_backend_loaded) {
        spdlog::info("[TemporalUpscaler] Could not load PDPerfPlugin.dll, TemporalUpscaler will not work");
    }

    return Mod::on_initialize();
}

void TemporalUpscaler::on_config_load(const utility::Config& cfg) {
    if (!ready()) {
        return;
    }
}

void TemporalUpscaler::on_config_save(utility::Config& cfg) {
    if (!ready()) {
        return;
    }
}

void TemporalUpscaler::on_draw_ui() {
    if (!ImGui::CollapsingHeader(this->get_name().data())) {
        return;
    }

    if (!m_backend_loaded) {
        ImGui::Text("Backend is not loaded, TemporalUpscaler will not work.");
        return;
    }

    ImGui::Checkbox("Enabled", &m_enabled);

    if (!ready()) {
        return;
    }

    ImGui::Checkbox("Upscale", &m_upscale);
    ImGui::Checkbox("Jitter", &m_jitter);
    if (ImGui::Checkbox("Sharpness", &m_sharpness)) {
        release_upscale_features();
        init_upscale_features();
    }
    
    ImGui::Checkbox("Allow Engine TAA", &m_allow_taa);

    ImGui::DragFloat("Jitter Scale X", &m_jitter_scale[0], 0.01f, -5.0f, 5.0f);
    ImGui::DragFloat("Jitter Scale Y", &m_jitter_scale[1], 0.01f, -5.0f, 5.0f);

    ImGui::DragFloat("Sharpness Amount", &m_sharpness_amount, 0.01f, 0.0f, 5.0f);

    const auto w = (float)GetRenderWidth();
    const auto h = (float)GetRenderHeight();

    if (ImGui::DragFloat("MotionScale X", &m_motion_scale[0], 0.01f, -w, w) ||
        ImGui::DragFloat("MotionScale Y", &m_motion_scale[1], 0.01f, -h, h)) 
    {
        SetMotionScaleX(get_evaluate_id(0), (float)m_motion_scale[0]);
        SetMotionScaleY(get_evaluate_id(0), (float)m_motion_scale[1]);

        if (VR::get()->is_hmd_active()) {
            SetMotionScaleX(get_evaluate_id(1), (float)m_motion_scale[0]);
            SetMotionScaleY(get_evaluate_id(1), (float)m_motion_scale[1]);
        }
    }
    
    if (ImGui::Combo("Upscale Type", (int*)&m_upscale_type, "DLSS\0FSR2\0XESS\0")) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        release_upscale_features();
        init_upscale_features();
    }

    if (ImGui::Combo("Quality Level", (int*)&m_upscale_quality, "Performance\0Balanced\0Quality\0UltraPerformance\0")) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        release_upscale_features();
        init_upscale_features();
    }
}

void TemporalUpscaler::on_present() {
    m_rendering = true;

    if (!m_backend_loaded) {
        return;
    }

    if (!m_first_frame_finished) {
        if (!on_first_frame()) {
            m_backend_loaded = false;
            m_initialized = false;
            return;
        }
    }

    if (m_wants_reinitialize) {
        init_upscale_features();
        m_wants_reinitialize = false;
        return;
    }

    if (!ready() || !m_upscale) {
        return;
    }

    if (m_scene_layer == nullptr) {
        return;
    }

    if (m_is_d3d12) {
        auto& hook = g_framework->get_d3d12_hook();
        auto swapchain = hook->get_swap_chain();
        auto device = hook->get_device();
        ComPtr<ID3D12Resource> backbuffer{};

        if (FAILED(swapchain->GetBuffer(swapchain->GetCurrentBackBufferIndex(), IID_PPV_ARGS(&backbuffer)))) {
            spdlog::error("[TemporalUpscaler] Failed to get backbuffer (D3D12)");
            return;
        }

        if (m_depth == nullptr) {
            spdlog::error("[TemporalUpscaler] Failed to get depth stencil (D3D12)");
            return;
        }

        if (m_motion_vectors == nullptr) {
            spdlog::error("[TemporalUpscaler] Failed to get motion vectors (D3D12)");
            return;
        }

        if (m_color == nullptr) {
            spdlog::error("[TemporalUpscaler] Failed to get color buffer (D3D12)");
            return;
        }

        /*if (m_prev_motion_vectors_d3d12[0] == nullptr) {
            // Create two new stored motion vectors based
            // on the desc of the current motion vectors
            auto mv = (ID3D12Resource*)m_motion_vectors;
            const auto desc = mv->GetDesc();

            D3D12_HEAP_PROPERTIES heap_props{};
            heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
            heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

            for (auto i = 0; i < m_prev_motion_vectors_d3d12.size(); ++i) {
                if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 
                    nullptr, IID_PPV_ARGS(&m_prev_motion_vectors_d3d12[i])))) 
                {
                    spdlog::error("[TemporalUpscaler] Failed to create previous motion vectors (D3D12)");
                    return;
                } else {
                    spdlog::info("[TemporalUpscaler] Created previous motion vectors ({}) (D3D12)", i);
                }
            }
        }*/

        auto bb_desc = backbuffer->GetDesc();

        m_backbuffer_size[0] = bb_desc.Width;
        m_backbuffer_size[1] = bb_desc.Height;

        auto vr = VR::get();

        const auto evaluate_id = get_evaluate_id(vr->is_hmd_active() ? vr->get_render_frame_count() : 0);
        const auto evaluate_index = evaluate_id - 1;

        EvaluateUpscale(evaluate_id, m_color, m_motion_vectors, m_depth, nullptr, m_sharpness_amount, 
                        m_jitter_offsets[evaluate_index][0], m_jitter_offsets[evaluate_index][1], false, m_nearz, m_farz, m_fov);

        const auto& cur_motion_vector = m_prev_motion_vectors_d3d12[evaluate_index];
        const auto& next_motion_vector = m_prev_motion_vectors_d3d12[(evaluate_index + 1) % 2];

        m_copier.wait(INFINITE);
        m_copier.copy((ID3D12Resource*)m_upscaled_textures[evaluate_index], backbuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_PRESENT);
        //m_copier.copy((ID3D12Resource*)m_motion_vectors, cur_motion_vector.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        //m_copier.copy(next_motion_vector.Get(), (ID3D12Resource*)m_motion_vectors, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COMMON);
        m_copier.execute();

        static bool once = true;

        if (once) {
            spdlog::info("Successfully rendered with TemporalUpscaler");
            once = false;
        }
    } else {
        auto& hook = g_framework->get_d3d11_hook();
        auto swapchain = hook->get_swap_chain();
        auto device = hook->get_device();

        // Get the context.
        ComPtr<ID3D11DeviceContext> context{};
        device->GetImmediateContext(&context);

        // Get the back buffer.
        ComPtr<ID3D11Texture2D> backbuffer{};
        swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));

        if (backbuffer == nullptr) {
            spdlog::error("[TemporalUpscaler] Failed to get backbuffer (D3D11)");
            return;
        }
    }
}

bool TemporalUpscaler::on_first_frame() {
    spdlog::info("[TemporalUpscaler] Initializing first frame...");

    m_first_frame_finished = true;
    m_is_d3d12 = g_framework->is_dx12();

    InitCSharpDelegate([](char* msg, int size) {
        spdlog::info("[TemporalUpscaler] {}", msg);
    });

    if (m_is_d3d12) {
        auto& hook = g_framework->get_d3d12_hook();
        SetupDirectX(hook->get_command_queue(), PDGraphicsAPI::D3D12);
    } else {
        auto& hook = g_framework->get_d3d11_hook();
        SetupDirectX(hook->get_device(), PDGraphicsAPI::D3D11);
    }

    if (!init_upscale_features()) {
        return false;
    }

    m_initialized = true;

    return true;
}

bool TemporalUpscaler::init_upscale_features() {
    spdlog::info("[TemporalUpscaler] Initializing upscale features...");

    uint32_t out_w = 0;
    uint32_t out_h = 0;
    uint32_t out_format = 0;

    if (m_is_d3d12) {
        auto& hook = g_framework->get_d3d12_hook();

        auto swapchain = hook->get_swap_chain();
        ComPtr<ID3D12Resource> backbuffer{};

        if (FAILED(swapchain->GetBuffer(swapchain->GetCurrentBackBufferIndex(), IID_PPV_ARGS(&backbuffer)))) {
            spdlog::error("[TemporalUpscaler] Failed to get backbuffer (D3D12)");
            return false;
        }

        // Get bb desc
        const auto bb_desc = backbuffer->GetDesc();

        m_backbuffer_size[0] = bb_desc.Width;
        m_backbuffer_size[1] = bb_desc.Height;

        out_w = bb_desc.Width;
        out_h = bb_desc.Height;
        out_format = bb_desc.Format;

        m_copier.setup();
    } else {
        auto& hook = g_framework->get_d3d11_hook();

        auto swapchain = hook->get_swap_chain();
        auto device = hook->get_device();

        // Get the context.
        ComPtr<ID3D11DeviceContext> context{};
        device->GetImmediateContext(&context);

        // Get the back buffer.
        ComPtr<ID3D11Texture2D> backbuffer{};
        swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));

        if (backbuffer == nullptr) {
            spdlog::error("[TemporalUpscaler] Failed to get backbuffer (D3D11)");
            return false;
        }

        // Get bb desc
        D3D11_TEXTURE2D_DESC bb_desc{};
        backbuffer->GetDesc(&bb_desc);

        out_w = bb_desc.Width;
        out_h = bb_desc.Height;
        out_format = bb_desc.Format;
    }

    // Left eye.
    m_upscaled_textures[0] = InitUpscaleFeature(
        get_evaluate_id(0), m_upscale_type, m_upscale_quality, out_w, out_h, 
        false, true, false, true, m_sharpness, false, out_format
    );

    // Right eye.
    if (VR::get()->is_hmd_active()) {
        m_upscaled_textures[1] = InitUpscaleFeature(
            get_evaluate_id(1), m_upscale_type, m_upscale_quality, out_w, out_h, 
            false, true, false, true, m_sharpness, false, out_format
        );
    }

    m_motion_scale[0] = (float)GetRenderWidth(get_evaluate_id(0)) / 2.0f;
    m_motion_scale[1] = -1.0f * ((float)GetRenderHeight(get_evaluate_id(0)) / 2.0f);

    SetMotionScaleX(get_evaluate_id(0), (float)m_motion_scale[0]);
    SetMotionScaleY(get_evaluate_id(0), (float)m_motion_scale[1]);

    if (VR::get()->is_hmd_active()) {
        SetMotionScaleX(get_evaluate_id(1), (float)m_motion_scale[0]);
        SetMotionScaleY(get_evaluate_id(1), (float)m_motion_scale[1]);
    }

    if (m_is_d3d12) {
        const auto desc = ((ID3D12Resource*)m_upscaled_textures[0])->GetDesc();

        spdlog::info("[TemporalUpscaler] Upscaled texture size: {}x{}", desc.Width, desc.Height);
    } else {
        ComPtr<ID3D11Texture2D> texture = (ID3D11Texture2D*)m_upscaled_textures[0];
        D3D11_TEXTURE2D_DESC desc{};
        texture->GetDesc(&desc);

        spdlog::info("[TemporalUpscaler] Upscaled texture size: {}x{}", desc.Width, desc.Height);
    }

    spdlog::info("[TemporalUpscaler] Wanted render resolution: {}x{}", GetRenderWidth(get_evaluate_id(0)), GetRenderHeight(get_evaluate_id(0)));
    spdlog::info("[TemporalUpscaler] Created upscaled texture(s)");

    return true;
}

void TemporalUpscaler::release_upscale_features() {
    if (m_upscaled_textures[0] == nullptr && m_upscaled_textures[1] == nullptr) {
        return;
    }

    m_copier.wait(2000);

    if (m_upscaled_textures[0] != nullptr) {
        ReleaseUpscaleFeature(get_evaluate_id(0));
        m_upscaled_textures[0] = nullptr;
    }

    if (m_upscaled_textures[1] != nullptr) {
        ReleaseUpscaleFeature(get_evaluate_id(1));
        m_upscaled_textures[1] = nullptr;
    }

    m_wants_reinitialize = true;
    m_copier.reset();

    for (auto& motion_vector : m_prev_motion_vectors_d3d12) {
        motion_vector.Reset();
    }
}

void TemporalUpscaler::on_post_present() {
    m_rendering = false;

    if (!ready()) {
        return;
    }
}

void TemporalUpscaler::on_device_reset() {
    release_upscale_features();
    m_wants_reinitialize = true;
}

void TemporalUpscaler::on_view_get_size(REManagedObject* scene_view, float* result) {
    if (!ready() && (!m_rendering || !m_set_view)) {
        m_set_view = false;
        return;
    }

    /*auto regenny_view = (regenny::via::SceneView*)scene_view;
    auto window = regenny_view->window;

    static auto via_scene_view = sdk::find_type_definition("via.SceneView");
    static auto set_display_type_method = via_scene_view->get_method("set_DisplayType");

    // Force the display to stretch to the window size
    if (set_display_type_method != nullptr) {
        set_display_type_method->call(sdk::get_thread_context(), regenny_view, via::DisplayType::Fit);
    } else {
#if not defined(RE7) || TDB_VER <= 49
        static auto is_sunbreak = utility::get_module_path(utility::get_executable())->find("MHRiseSunbreakDemo") != std::string::npos;

        if (is_sunbreak) {
            *(regenny::via::DisplayType*)((uintptr_t)&regenny_view->display_type + 4) = regenny::via::DisplayType::Fit;
        } else {
            regenny_view->display_type = regenny::via::DisplayType::Fit;
        }
#else
        *(regenny::via::DisplayType*)((uintptr_t)&regenny_view->display_type + 4) = regenny::via::DisplayType::Fit;
#endif
    }

    auto wanted_width = 0.0f;
    auto wanted_height = 0.0f;

    // Set the window size, which will increase the size of the backbuffer
    if (window != nullptr) {
        if (m_enabled) {
#if TDB_VER <= 49
            if (!g_previous_size) {
                g_previous_size = regenny::via::Size{ (float)window->width, (float)window->height };
            }
#endif
            window->width = GetRenderWidth();
            window->height = GetRenderHeight();

            if (m_is_d3d12) {
                if (m_backbuffer_size[0] > 0 && m_backbuffer_size[1] > 0) {
                    if (std::abs((int)m_backbuffer_size[0] - (int)window->width) > 50 || std::abs((int)m_backbuffer_size[1] - (int)window->height) > 50) {
                        const auto now = VR::get()->get_game_frame_count();

                        if (!m_backbuffer_inconsistency) {
                            m_backbuffer_inconsistency_start = now;
                            m_backbuffer_inconsistency = true;
                        }

                        const auto is_true_inconsistency = (now - m_backbuffer_inconsistency_start) >= 5;

                        if (is_true_inconsistency) {
                            // Force a reset of the backbuffer size
                            window->width = GetRenderWidth() + 1;
                            window->height = GetRenderHeight() + 1;

                            spdlog::info("[TemporalUpscaler] Previous backbuffer size: {}x{}", m_backbuffer_size[0], m_backbuffer_size[1]);
                            spdlog::info("[TemporalUpscaler] Backbuffer size inconsistency detected, resetting backbuffer size to {}x{}", window->width, window->height);

                            // m_backbuffer_inconsistency gets set to false on device reset.
                        }
                    }
                } else {
                    m_backbuffer_inconsistency = false;
                }
            }
        } else {
            m_backbuffer_inconsistency = false;

#if TDB_VER > 49
            window->width = (uint32_t)window->borderless_size.w;
            window->height = (uint32_t)window->borderless_size.h;
#else
            if (g_previous_size) {
                window->width = (uint32_t)g_previous_size->w;
                window->height = (uint32_t)g_previous_size->h;

                g_previous_size = std::nullopt;
            }
#endif
        }

        wanted_width = (float)window->width;
        wanted_height = (float)window->height;
    }*/

    // spoof the size to the HMD's size
    auto vr = VR::get();
    const auto evaluate_index = get_evaluate_id(vr->is_hmd_active() ? vr->get_game_frame_count() : 0);

    result[0] = (float)GetRenderWidth(evaluate_index);
    result[1] = (float)GetRenderHeight(evaluate_index);
    m_set_view = true;
}

void TemporalUpscaler::on_camera_get_projection_matrix(REManagedObject* camera, Matrix4x4f* result) {
    if (!ready()) {
        return;
    }
}

void TemporalUpscaler::on_scene_layer_update(sdk::renderer::layer::Scene* layer, void* render_context) {
    if (!ready() || !m_jitter) {
        return;
    }

    auto scene_info = layer->get_scene_info();
    auto depth_distortion_scene_info = layer->get_depth_distortion_scene_info();
    auto filter_scene_info = layer->get_filter_scene_info();
    auto jitter_disable_scene_info = layer->get_jitter_disable_scene_info();
    auto jitter_disable_post_scene_info = layer->get_jitter_disable_post_scene_info();
    auto z_prepass_scene_info = layer->get_z_prepass_scene_info();

    auto vr = VR::get();

    const auto evaluate_id = get_evaluate_id(vr->is_hmd_active() ? vr->get_game_frame_count() : 0);
    const auto evaluate_index = evaluate_id - 1;

    const auto phase = GetJitterPhaseCount(evaluate_id);
    const auto w = (float)GetRenderWidth(evaluate_id);
    const auto h = (float)GetRenderHeight(evaluate_id);

    float x = 0.0f;
    float y = 0.0f;

    m_jitter_indices[evaluate_index]++;
    GetJitterOffset(&x, &y, m_jitter_indices[evaluate_index], phase);

    m_jitter_offsets[evaluate_index][0] = -x;
    m_jitter_offsets[evaluate_index][1] = -y;

    // from FSR2 code
    x = m_jitter_scale[0] * (x / w);
    y = m_jitter_scale[1] * (y / h);

    // this is what I send back to DLSS
    //m_jitter_offset[0] = (-x * w) / m_jitter_scale[0];
    //m_jitter_offset[1] = (-y * h) / m_jitter_scale[1];

    auto add_jitter = [&](sdk::renderer::SceneInfo* scene_info) {
        if (scene_info == nullptr) {
            return;
        }

        scene_info->projection_matrix[2][0] += x;
        scene_info->projection_matrix[2][1] += y;
        scene_info->inverse_projection_matrix = glm::inverse(scene_info->projection_matrix);

        scene_info->view_projection_matrix = scene_info->projection_matrix * scene_info->view_matrix;
        scene_info->inverse_view_projection_matrix = glm::inverse(scene_info->view_projection_matrix);
    };

    add_jitter(scene_info);
    add_jitter(depth_distortion_scene_info);
    add_jitter(filter_scene_info);
    add_jitter(jitter_disable_scene_info);
    add_jitter(jitter_disable_post_scene_info);
    add_jitter(z_prepass_scene_info);
}

void TemporalUpscaler::on_pre_application_entry(void* entry, const char* name, size_t hash) {
    if (!ready()) {
        return;
    }

    if (hash == "EndRendering"_fnv) {
        auto root_layer = sdk::renderer::get_root_layer();

        if (root_layer != nullptr) {
            auto [scene_parent, scene_layer] = root_layer->find_layer_recursive("via.render.layer.Scene");
            auto [output_parent, output_layer] = root_layer->find_layer_recursive("via.render.layer.Output");

            if (scene_layer != nullptr && *scene_layer != nullptr) {
                m_scene_layer = (decltype(m_scene_layer))*scene_layer;

                if (m_scene_layer == nullptr) {
                    spdlog::error("[TemporalUpscaler] Failed to find scene layer");
                }
            } else {
                spdlog::error("[TemporalUpscaler] Failed to find scene layer");
                m_scene_layer = nullptr;
            }

            if (output_layer != nullptr && *output_layer != nullptr) {
                m_output_layer = (decltype(m_output_layer))*output_layer;

                if (m_output_layer == nullptr) {
                    spdlog::error("[TemporalUpscaler] Failed to find output layer");
                }
            } else {
                spdlog::error("[TemporalUpscaler] Failed to find output layer");
                m_output_layer = nullptr;
            }
        } else {
            spdlog::error("[TemporalUpscaler] Failed to get root layer");
            m_scene_layer = nullptr;
            m_output_layer = nullptr;
        }

        if (m_scene_layer != nullptr && m_is_d3d12) {
            m_depth = m_scene_layer->get_depth_stencil_d3d12();
            m_motion_vectors = m_scene_layer->get_motion_vectors_d3d12();
            // m_color = m_scene_layer->get_hdr_target_d3d12();
        } else {
            m_depth = nullptr;
            m_motion_vectors = nullptr;
        }

        if (m_output_layer != nullptr && m_is_d3d12) {
            m_color = m_output_layer->get_output_target_d3d12();
        } else {
            m_color = nullptr;
        }

        auto camera = sdk::get_primary_camera();

        if (camera == nullptr) {
            spdlog::error("[TemporalUpscaler] Failed to get primary camera");
            return;
        }

        static auto via_camera = sdk::find_type_definition("via.Camera");
        static auto get_near_clip_plane_method = via_camera->get_method("get_NearClipPlane");
        static auto get_far_clip_plane_method = via_camera->get_method("get_FarClipPlane");
        static auto get_fov_method = via_camera->get_method("get_FOV");
        static auto get_projection_matrix_method = via_camera->get_method("get_ProjectionMatrix");

        auto context = sdk::get_thread_context();
        m_nearz = get_near_clip_plane_method->call<float>(context, camera);
        m_farz = get_far_clip_plane_method->call<float>(context, camera);
        //m_fov = glm::radians(get_fov_method->call<float>(sdk::get_thread_context(), camera));

        Matrix4x4f projection_matrix{};
        get_projection_matrix_method->call<void>(&projection_matrix, context, camera);
        m_fov = 2.0f * std::atan(1.0f / projection_matrix[1][1]);

        static auto renderer_t = sdk::find_type_definition("via.render.Renderer");
        static auto render_config_t = sdk::find_type_definition("via.render.RenderConfig");
        static auto get_render_config_method = renderer_t->get_method("get_RenderConfig");

        static auto get_antialiasing_method = render_config_t->get_method("get_AntiAliasing");
        static auto set_antialiasing_method = render_config_t->get_method("set_AntiAliasing");

        auto renderer = renderer_t->get_instance();
        auto render_config = get_render_config_method->call<::REManagedObject*>(context, renderer);
        const auto antialiasing = get_antialiasing_method->call<via::render::RenderConfig::AntiAliasingType>(context, render_config);

        // Disable TAA
        switch (antialiasing) {
            case via::render::RenderConfig::AntiAliasingType::TAA: [[fallthrough]];
            case via::render::RenderConfig::AntiAliasingType::FXAA_TAA:
                if (!m_allow_taa) {
                    set_antialiasing_method->call<void*>(context, render_config, via::render::RenderConfig::AntiAliasingType::NONE);
                    spdlog::info("[TemporalUpscaler] TAA disabled");
                }

                break;
            case via::render::RenderConfig::AntiAliasingType::NONE:
                if (m_allow_taa) {
                    set_antialiasing_method->call<void*>(context, render_config, via::render::RenderConfig::AntiAliasingType::TAA);
                }

                break;
            default:
                break;
        }
    }
}

void TemporalUpscaler::on_application_entry(void* entry, const char* name, size_t hash) {
    if (!ready()) {
        return;
    }
}
