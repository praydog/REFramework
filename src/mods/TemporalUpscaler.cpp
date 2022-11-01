#include <d3d11.h>
#include <d3d12.h>
#include <wrl.h>

#include <utility/Module.hpp>
#include <PDPerfPlugin.h>

#include <sdk/Renderer.hpp>
#include <sdk/SceneManager.hpp>
#include <sdk/Memory.hpp>

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
    } else {
        for (auto i = 0; i <= TemporalUpscaler::PDUpscaleType::XESS; ++i) {
            const auto is_available = IsUpscaleMethodAvailable(i);
            const auto upscale_name = GetUpscaleMethodName(i);

            if (upscale_name == nullptr) {
                continue;
            }

            if (is_available) {
                m_available_upscale_methods[upscale_name] = i;
                m_available_upscale_method_names.push_back(upscale_name);
                spdlog::info("[TemporalUpscaler] Upscale method {} is available", i, upscale_name);
            } else {
                spdlog::info("[TemporalUpscaler] Upscale method {} is not available", i, upscale_name);
            }
        }

        if (m_available_upscale_methods.empty()) {
            spdlog::info("[TemporalUpscaler] No upscale methods are available, TemporalUpscaler will not work");
            m_backend_loaded = false;
        } else {
            m_upscale_type = (PDUpscaleType)m_available_upscale_methods[m_available_upscale_method_names[m_available_upscale_type]];
        }
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

    if (ImGui::SliderInt("Displayed Scene", &m_displayed_scene, 0, 1)) {

    }

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

    std::vector<const char*> imgui_combo_names{};

    for (auto& m : m_available_upscale_method_names) {
        imgui_combo_names.push_back(m.c_str());
    }
    
    if (ImGui::Combo("Upscale Type", (int*)&m_available_upscale_type, imgui_combo_names.data(), imgui_combo_names.size())) {
        if (m_available_upscale_type < 0 || m_available_upscale_type > m_available_upscale_method_names.size()) {
            m_available_upscale_type = 0;
            m_upscale_type = (PDUpscaleType)m_available_upscale_methods[m_available_upscale_method_names[0]];
        } else {
            m_upscale_type = (PDUpscaleType)m_available_upscale_methods[m_available_upscale_method_names[m_available_upscale_type]];
        }

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

void TemporalUpscaler::on_early_present() {
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

    if (m_eye_states[0].scene_layer == nullptr) {
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

        auto bb_desc = backbuffer->GetDesc();

        m_backbuffer_size[0] = bb_desc.Width;
        m_backbuffer_size[1] = bb_desc.Height;

        auto vr = VR::get();
        const auto vr_enabled = vr->is_hmd_active();
        const auto is_vr_multipass = vr_enabled && vr->is_using_multipass();
        const auto frame = vr->get_render_frame_count();

        m_copier.wait(INFINITE);

        if (vr_enabled && is_vr_multipass) {
            if (m_eye_states[0].color == nullptr || m_eye_states[0].depth == nullptr || m_eye_states[0].motion_vectors == nullptr) {
                spdlog::error("[TemporalUpscaler] Missing eye state for left eye (D3D12)");
                return;
            }

            if (m_eye_states[1].color == nullptr || m_eye_states[1].depth == nullptr || m_eye_states[1].motion_vectors == nullptr) {
                spdlog::error("[TemporalUpscaler] Missing eye state for right eye (D3D12)");
                return;
            }

            // Create one giant texture to encompass both eyes
            /*if (m_big_color == nullptr) {
                auto desc = m_eye_states[0].color->GetDesc();
                desc.Width *= 2;

                D3D12_HEAP_PROPERTIES heap_props{};
                heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
                heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
                heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

                if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_big_color)))) {
                    spdlog::error("[TemporalUpscaler] Failed to create big color texture");
                    return;
                }

                if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_blank_big_color)))) {
                    spdlog::error("[TemporalUpscaler] Failed to create blank big motion vectors texture");
                    return;
                }


                spdlog::info("[TemporalUpscaler] Created big color texture");
            }

            if (m_big_depth == nullptr) {
                auto desc = m_eye_states[0].depth->GetDesc();
                spdlog::info("Depth width {}", (uint32_t)desc.Width);
                spdlog::info("Depth height {}", (uint32_t)desc.Height);

                desc.Width *= 2;

                spdlog::info("Depth dimension {}", (uint32_t)desc.Dimension);

                D3D12_HEAP_PROPERTIES heap_props{};
                heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
                heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
                heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

                if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_big_depth)))) {
                    spdlog::error("[TemporalUpscaler] Failed to create big depth texture");
                    return;
                }

                if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_blank_big_depth)))) {
                    spdlog::error("[TemporalUpscaler] Failed to create blank big motion vectors texture");
                    return;
                }

                spdlog::info("[TemporalUpscaler] Created big depth texture");
            }

            if (m_big_motion_vectors == nullptr) {
                auto desc = m_eye_states[0].motion_vectors->GetDesc();
                spdlog::info("Motion vectors width {}", (uint32_t)desc.Width);
                spdlog::info("Motion vectors height {}", (uint32_t)desc.Height);

                desc.Width *= 2;

                spdlog::info("Motion vectors dimension {}", (uint32_t)desc.Dimension);

                D3D12_HEAP_PROPERTIES heap_props{};
                heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
                heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
                heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

                if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_big_motion_vectors)))) {
                    spdlog::error("[TemporalUpscaler] Failed to create big motion vectors texture");
                    return;
                }

                if (FAILED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_blank_big_motion_vectors)))) {
                    spdlog::error("[TemporalUpscaler] Failed to create blank big motion vectors texture");
                    return;
                }

                spdlog::info("[TemporalUpscaler] Created big motion vectors texture");
            }

            m_big_copier.wait(INFINITE);

            m_big_copier.copy(m_blank_big_color.Get(), m_big_color.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COMMON);
            m_big_copier.copy(m_blank_big_depth.Get(), m_big_depth.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COMMON);
            m_big_copier.copy(m_blank_big_motion_vectors.Get(), m_big_motion_vectors.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COMMON);


            for (auto i = 0; i < m_eye_states.size(); ++i) {
                auto& state = m_eye_states[i];

                const auto color_desc = state.color->GetDesc();
                const auto depth_desc = state.depth->GetDesc();
                const auto motion_vectors_desc = state.motion_vectors->GetDesc();

                D3D12_BOX box{};
                box.left = 0;
                box.top = 0;
                box.right = color_desc.Width;
                box.bottom = color_desc.Height;
                box.front = 0;
                box.back = 1;

                m_big_copier.copy_region(state.color.Get(), m_big_color.Get(), i * color_desc.Width, 0, 0, &box, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COMMON);

                box.right = depth_desc.Width;
                box.bottom = depth_desc.Height;

                m_big_copier.copy_region(state.depth.Get(), m_big_depth.Get(), i * depth_desc.Width, 0, 0, &box, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COMMON);

                box.right = motion_vectors_desc.Width;
                box.bottom = motion_vectors_desc.Height;

                m_big_copier.copy_region(state.motion_vectors.Get(), m_big_motion_vectors.Get(), i * motion_vectors_desc.Width, 0, 0, &box, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COMMON);
            }

            m_big_copier.execute();

            const auto evaluate_id = get_evaluate_id(0);
            const auto evaluate_index = evaluate_id - 1;

            EvaluateUpscale(evaluate_id, m_big_color.Get(), m_big_motion_vectors.Get(), m_big_depth.Get(), nullptr, m_sharpness_amount, 
                m_jitter_offsets[evaluate_index][0], m_jitter_offsets[evaluate_index][1], false, m_nearz, m_farz, m_fov);*/
            
            for (auto i = 0; i < 2; ++i) {
                const auto& state = m_eye_states[i];
                const auto evaluate_id = get_evaluate_id(i);
                const auto evaluate_index = evaluate_id - 1;
                EvaluateUpscaleDX12(evaluate_id, i == 1, state.color.Get(), state.motion_vectors.Get(), state.depth.Get(), nullptr, m_sharpness_amount, 
                                    m_jitter_offsets[evaluate_index][0], m_jitter_offsets[evaluate_index][1], false, m_nearz, m_farz, m_fov);
            }
        } else {
            for (auto i = 0; i < m_eye_states.size(); ++i) {
                if (!vr_enabled && i > 0) {
                    break;
                }

                auto& state = m_eye_states[i];

                if (state.depth == nullptr) {
                    if (i == 0) {
                        spdlog::error("[TemporalUpscaler] Failed to get depth stencil (D3D12)");
                    }

                    continue;
                }

                if (state.motion_vectors == nullptr) {
                    if (i == 0) {
                        spdlog::error("[TemporalUpscaler] Failed to get motion vectors (D3D12)");
                    }

                    continue;
                }

                if (state.color == nullptr) {
                    if (i == 0) {
                        spdlog::error("[TemporalUpscaler] Failed to get color buffer (D3D12)");
                    }

                    continue;
                }

                const auto vr_index = is_vr_multipass ? i : frame;
                const auto index = vr_enabled ? vr_index : i;

                const auto evaluate_id = get_evaluate_id(index);
                const auto evaluate_index = evaluate_id - 1;

                EvaluateUpscale(evaluate_id, state.color.Get(), state.motion_vectors.Get(), state.depth.Get(), nullptr, m_sharpness_amount, 
                                m_jitter_offsets[evaluate_index][0], m_jitter_offsets[evaluate_index][1], false, m_nearz, m_farz, m_fov);

                if (i == 0) {
                    m_copier.copy((ID3D12Resource*)m_upscaled_textures[evaluate_index], backbuffer.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PRESENT);
                }
            }
        }

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
        false, true, false, false, m_sharpness, false, out_format
    );

    // Right eye.
    if (VR::get()->is_hmd_active()) {
        m_upscaled_textures[1] = InitUpscaleFeature(
            get_evaluate_id(1), m_upscale_type, m_upscale_quality, out_w, out_h, 
            false, true, false, false, m_sharpness, false, out_format
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
    
    for (auto& state : m_eye_states) {
        state.color.Reset();
        state.depth.Reset();
        state.motion_vectors.Reset();
        state.scene_layer = nullptr;
    }

    for (auto& motion_vector : m_prev_motion_vectors_d3d12) {
        motion_vector.Reset();
    }

    for (auto& g_buffer : m_prev_g_buffer_d3d12) {
        for (auto& tex : g_buffer.textures) {
            tex.Reset();
        }

        g_buffer.textures.clear();
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

    const auto evaluate_id = get_evaluate_id(0);

    /*if (vr->is_hmd_active() && vr->is_using_multipass()) {
        result[0] = (float)GetRenderWidth(evaluate_id) / 2.0f;
        result[1] = (float)GetRenderHeight(evaluate_id);
    } else {*/
        result[0] = (float)GetRenderWidth(evaluate_id);
        result[1] = (float)GetRenderHeight(evaluate_id);
    //}

    m_set_view = true;
}

void TemporalUpscaler::on_camera_get_projection_matrix(REManagedObject* camera, Matrix4x4f* result) {
    if (!ready()) {
        return;
    }
}

void TemporalUpscaler::on_scene_layer_update(sdk::renderer::layer::Scene* layer, void* render_context) {
    if (!ready()) {
        return;
    }

    auto scene_info = layer->get_scene_info();
    auto depth_distortion_scene_info = layer->get_depth_distortion_scene_info();
    auto filter_scene_info = layer->get_filter_scene_info();
    auto jitter_disable_scene_info = layer->get_jitter_disable_scene_info();
    auto jitter_disable_post_scene_info = layer->get_jitter_disable_post_scene_info();
    auto z_prepass_scene_info = layer->get_z_prepass_scene_info();

    auto vr = VR::get();

    uint32_t vr_index = 0;

    const auto vr_enabled = vr->is_hmd_active();
    const auto is_vr_multipass = vr_enabled && vr->is_using_multipass();

    if (vr->is_using_multipass()) {
        auto output_layer = sdk::renderer::get_output_layer();

        if (output_layer != nullptr) {
            static auto t = sdk::find_type_definition("via.render.layer.Scene")->get_type();
            const auto scenes = output_layer->find_layers(t);

            if (!scenes.empty()) {
                if (layer == scenes[0]) {
                    vr_index = 0;
                } else {
                    vr_index = 1;
                }
            }
        }
    } else if (vr_enabled) {
        vr_index = vr->get_game_frame_count();
    }

    const auto evaluate_id = get_evaluate_id(vr_enabled ? vr_index : 0);
    const auto evaluate_index = evaluate_id - 1;

    const auto phase = GetJitterPhaseCount(evaluate_id);
    const auto w = (float)GetRenderWidth(evaluate_id);
    const auto h = (float)GetRenderHeight(evaluate_id);

    float x = 0.0f;
    float y = 0.0f;

    if (m_jitter) {
        m_jitter_indices[evaluate_index]++;
        GetJitterOffset(&x, &y, m_jitter_indices[evaluate_index], phase);

        m_jitter_offsets[evaluate_index][0] = -x;
        m_jitter_offsets[evaluate_index][1] = -y;

        // from FSR2 code
        x = m_jitter_scale[0] * (x / w);
        y = m_jitter_scale[1] * (y / h);
    } else {
        m_jitter_offsets[evaluate_index][0] = 0.0f;
        m_jitter_offsets[evaluate_index][1] = 0.0f;
    }

    auto add_jitter = [&](int32_t i, sdk::renderer::SceneInfo* scene_info) {
        if (scene_info == nullptr) {
            return;
        }

        this->m_old_projection_matrix[evaluate_index][i][2][0] += x;
        this->m_old_projection_matrix[evaluate_index][i][2][1] += y;

        scene_info->old_view_projection_matrix = this->m_old_projection_matrix[evaluate_index][i] * this->m_old_view_matrix[evaluate_index][i];

        this->m_old_projection_matrix[evaluate_index][i] = scene_info->projection_matrix;
        this->m_old_view_matrix[evaluate_index][i] = scene_info->view_matrix;

        scene_info->projection_matrix[2][0] += x;
        scene_info->projection_matrix[2][1] += y;
        scene_info->inverse_projection_matrix = glm::inverse(scene_info->projection_matrix);

        scene_info->view_projection_matrix = scene_info->projection_matrix * scene_info->view_matrix;
        scene_info->inverse_view_projection_matrix = glm::inverse(scene_info->view_projection_matrix);
    };

    add_jitter(0, scene_info);
    add_jitter(1, depth_distortion_scene_info);
    add_jitter(2, filter_scene_info);
    add_jitter(3, jitter_disable_scene_info);
    add_jitter(4, jitter_disable_post_scene_info);
    add_jitter(5, z_prepass_scene_info);
}

bool TemporalUpscaler::on_pre_output_layer_draw(sdk::renderer::layer::Output* layer, void* render_context) {
    return true;
}

bool TemporalUpscaler::on_pre_output_layer_update(sdk::renderer::layer::Output* layer, void* render_context) {
    return true;
}

void TemporalUpscaler::on_pre_application_entry(void* entry, const char* name, size_t hash) {
    if (!ready()) {
        return;
    }

    if (hash == "BeginRendering"_fnv) {
        update_mirror();
    }

    if (hash == "EndRendering"_fnv) {
        fix_output_layer();

        const auto is_vr_multipass = VR::get()->is_hmd_active() && VR::get()->is_using_multipass();
        auto root_layer = sdk::renderer::get_root_layer();

        if (root_layer != nullptr) {
            auto [output_parent, output_layer] = root_layer->find_layer_recursive("via.render.layer.Output");
            auto scene_layers = (*output_layer)->find_layers(sdk::find_type_definition("via.render.layer.Scene")->get_type());

            if (scene_layers.size() > 1) {
                if (!is_vr_multipass) {
                    if (m_displayed_scene == 0) {
                        m_eye_states[0].scene_layer = (sdk::renderer::layer::Scene*)scene_layers[0];
                    } else {
                        m_eye_states[0].scene_layer = (sdk::renderer::layer::Scene*)scene_layers[1];
                    }

                    m_eye_states[1].scene_layer = nullptr;
                } else {
                    m_eye_states[0].scene_layer = (sdk::renderer::layer::Scene*)scene_layers[0];
                    m_eye_states[1].scene_layer = (sdk::renderer::layer::Scene*)scene_layers[1];
                }
            } else {
                m_eye_states[0].scene_layer = (sdk::renderer::layer::Scene*)scene_layers[0];
                m_eye_states[1].scene_layer = nullptr;
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
            m_eye_states[0].scene_layer = nullptr;
            m_eye_states[1].scene_layer = nullptr;
            m_output_layer = nullptr;
        }

        for (auto& state : m_eye_states) {
            if (state.scene_layer == nullptr) {
                state.depth.Reset();
                state.motion_vectors.Reset();
                state.color.Reset();
                continue;
            }

            if (m_is_d3d12) {
                static auto potype = sdk::find_type_definition("via.render.layer.PrepareOutput")->get_type();
                auto prepareoutput_layer = state.scene_layer->find_layer(potype);

                if (prepareoutput_layer == nullptr) {
                    state.depth.Reset();
                    state.motion_vectors.Reset();
                    state.color.Reset();
                    continue;
                }

                state.depth = (ID3D12Resource*)state.scene_layer->get_depth_stencil_d3d12();
                state.motion_vectors = (ID3D12Resource*)state.scene_layer->get_motion_vectors_d3d12();

                if (prepareoutput_layer != nullptr && *prepareoutput_layer != nullptr) {
                    const auto current_target_state = ((sdk::renderer::layer::PrepareOutput*)*prepareoutput_layer)->get_output_state();

                    if (current_target_state != nullptr) {
                        state.color = (ID3D12Resource*)current_target_state->get_native_resource_d3d12();
                    } else {
                        state.color.Reset();
                    }
                } else {
                    state.color.Reset();
                }
            } else {
                state.depth.Reset();
                state.motion_vectors.Reset();
                state.color.Reset();
            }
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

    if (hash == "EndRendering"_fnv) {
        fix_output_layer();
    }
}

void TemporalUpscaler::fix_output_layer() {
    if (m_last_output_layer != nullptr && m_cloned_output_layer != nullptr && m_original_output_layer != nullptr) {
        const auto current_state = *(sdk::renderer::TargetState**)((uintptr_t)m_last_output_layer + 0x88);

        if (current_state != m_last_output_state) {
            spdlog::info("[TemporalUpscaler] Output layer state changed!");

            if (m_last_output_layer != m_original_output_layer) {
                *(sdk::renderer::TargetState**)((uintptr_t)m_original_output_layer + 0x88) = m_last_output_state;
            } else {
                *(sdk::renderer::TargetState**)((uintptr_t)m_cloned_output_layer + 0x88) = m_last_output_state;
            }

            m_last_output_state = current_state;
        }
    }
}

void TemporalUpscaler::update_mirror() {
    if (!VR::get()->is_hmd_active() || !VR::get()->is_using_multipass()) {
        return;
    }

    static auto last_time = std::chrono::high_resolution_clock::now();
    const auto now = std::chrono::high_resolution_clock::now();

    if (now - last_time < std::chrono::milliseconds(1000)) {
        return;
    }

    last_time = now;

    auto output_layer = sdk::renderer::get_output_layer();
    if (output_layer == nullptr) {
        return;
    }

    auto scene_layers = output_layer->find_layers(sdk::find_type_definition("via.render.layer.Scene")->get_type());
    if (scene_layers.empty()) {
        return;
    }

    if (m_made_mirror) {
        if (!scene_layers.empty()) {
            // Remove the mirror. This will cause the scene to be enabled.
            // Then the scene layer created by the mirror and the main layer will render at the same time.
            sdk::call_object_func_easy<void*>(scene_layers[0], "set_Mirror", nullptr);

            if (scene_layers.size() > 1) {
                static auto potype = sdk::find_type_definition("via.render.layer.PrepareOutput")->get_type();
                auto prepare_output_layer = (sdk::renderer::layer::PrepareOutput**)scene_layers[0]->find_layer(potype);

                if (prepare_output_layer == nullptr) {
                    spdlog::error("[TemporalUpscaler] Failed to find PrepareOutput layer");
                    return;
                }

                const auto current_target_state = (*prepare_output_layer)->get_output_state();

                if (m_new_target_state == nullptr) {
                    if (current_target_state != nullptr) {
                        m_new_target_state = current_target_state->clone();
                        (*prepare_output_layer)->set_output_state(m_new_target_state);

                        spdlog::info("[TemporalUpscaler] Created new target state");
                    } else {
                        spdlog::error("[TemporalUpscaler] Current target state is null");
                    }
                } else if (current_target_state != m_new_target_state) {
                    spdlog::error("[TemporalUpscaler] Target state mismatch");
                    m_new_target_state = nullptr;
                }
            } else {
                //spdlog::error("[TemporalUpscaler] Failed to find second scene layer");
            }
        }

        return;
    }

    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    auto camera_gameobject = sdk::call_object_func_easy<::REGameObject*>(camera, "get_GameObject");

    if (camera_gameobject == nullptr) {
        return;
    }

    const auto tdef = sdk::find_type_definition("via.render.Mirror");
    if (tdef == nullptr) {
        spdlog::error("[TemporalUpscaler] Failed to find via.render.Mirror type definition");
        return;
    }

    const auto runtime_type = tdef->get_runtime_type();

    if (runtime_type == nullptr) {
        spdlog::error("[TemporalUpscaler] Failed to get via.render.Mirror runtime type");
        return;
    }

    const auto new_comp = sdk::call_object_func_easy<::REComponent*>(camera_gameobject, "createComponent(System.Type)", runtime_type);

    if (new_comp == nullptr) {
        spdlog::error("[TemporalUpscaler] Failed to create via.render.Mirror component");
        return;
    }

    m_made_mirror = true;
}