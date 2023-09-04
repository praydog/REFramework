#include <d3d11.h>
#include <d3d12.h>
#include <wrl.h>

#include <utility/Module.hpp>
#include <utility/Scan.hpp>
#include <utility/ScopeGuard.hpp>
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
#ifdef RE4
#include "sdk/regenny/re4/via/Window.hpp"
#include "sdk/regenny/re4/via/SceneView.hpp"
#elif defined(SF6)
#include "sdk/regenny/sf6/via/Window.hpp"
#include "sdk/regenny/sf6/via/SceneView.hpp"
#else
#include "sdk/regenny/re8/via/Window.hpp"
#include "sdk/regenny/re8/via/SceneView.hpp"
#endif
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
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }

    if (!ready()) {
        return;
    }
}

void TemporalUpscaler::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }

    if (!ready()) {
        return;
    }
}

void TemporalUpscaler::on_draw_ui() {
    if (!ImGui::CollapsingHeader(this->get_name().data())) {
        return;
    }

#if TDB_VER < 69
    ImGui::TextWrapped("TemporalUpscaler is not yet supported on this version of the engine.");
    ImGui::TextWrapped("Supported: RE2/RE3/RE7 (RT latest, not beta builds), RE4, RE8, SF6");
    return;
#else
    if (!m_backend_loaded) {
        ImGui::TextWrapped("Backend is not loaded, TemporalUpscaler will not work.");
        ImGui::TextWrapped("Make sure you've downloaded UpscalerBasePlugin (PDPerfPlugin.dll)");
        ImGui::TextWrapped("And the corresponding DLLs for your preferred upscaler(s) (DLSS/FSR2/XeSS)");
        return;
    }

    //ImGui::Checkbox("Enabled", &m_enabled);
    m_enabled->draw("Enabled");

    if (!ready()) {
        return;
    }
    
    //if (ImGui::Checkbox("Use Native Res (DLAA)", &m_use_native_resolution)) {
    if (m_use_native_resolution->draw("Use Native Res (DLAA)")) {
        update_motion_scale();
    }

    //if (ImGui::Checkbox("Sharpness", &m_sharpness)) {
    if (m_sharpness->draw("Sharpness")) {
        release_upscale_features();
        init_upscale_features();
    }

    //ImGui::DragFloat("Sharpness Amount", &m_sharpness_amount, 0.01f, 0.0f, 5.0f);
    m_sharpness_amount->draw("Sharpness Amount");

    const auto w = (float)get_render_width();
    const auto h = (float)get_render_height();

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

    /*if (ImGui::Combo("Quality Level", (int*)&m_upscale_quality, "Performance\0Balanced\0Quality\0UltraPerformance\0")) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        release_upscale_features();
        init_upscale_features();
    }*/

    if (m_upscale_quality->draw("Quality Level")) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        release_upscale_features();
        init_upscale_features();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);

    if (ImGui::TreeNode("Debug Options")) {
        ImGui::Checkbox("Upscale", &m_upscale);
        ImGui::Checkbox("Jitter", &m_jitter);
        ImGui::Checkbox("Allow Engine TAA", &m_allow_taa);

        ImGui::SliderInt("Displayed Scene", &m_displayed_scene, 0, 1);
        ImGui::DragFloat("Jitter Scale X", &m_jitter_scale[0], 0.01f, -5.0f, 5.0f);
        ImGui::DragFloat("Jitter Scale Y", &m_jitter_scale[1], 0.01f, -5.0f, 5.0f);

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

        ImGui::Text("OptimalBias: %f", GetOptimalMipmapBias(get_evaluate_id(0)));

        ImGui::TreePop();
    }
#endif
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
            for (auto& state : m_eye_states) {
                if (state.scene_layer == nullptr) {
                    state.depth = nullptr;
                    state.color = nullptr;
                    state.motion_vectors = nullptr;
                    state.depth_copy = nullptr;
                    state.motion_vectors_copy = nullptr;
                    state.color_copy = nullptr;
                    continue;
                }

                const auto new_depth = state.scene_layer->get_depth_stencil_d3d12();
                const auto new_motion_vectors = state.scene_layer->get_motion_vectors_d3d12();

                if (new_depth != nullptr && (state.depth_copy == nullptr || new_depth != state.depth.Get())) {
                    state.depth_copy = state.scene_layer->get_depth_stencil()->clone();
                    state.depth = new_depth;

                    spdlog::info("[TemporalUpscaler] Made clone of depth stencil @ {:x}", (uintptr_t)state.depth_copy.get());
                }

                if (new_motion_vectors != nullptr && (state.motion_vectors_copy == nullptr || new_motion_vectors != state.motion_vectors.Get())) {
                    const auto motion_vectors_state = state.scene_layer->get_motion_vectors_state();

                    if (motion_vectors_state != nullptr) {
                        const auto rtv = motion_vectors_state->get_rtv(0);

                        if (rtv != nullptr) {
                            auto tex = rtv->get_texture_d3d12();

                            if (tex != nullptr) {
                                state.motion_vectors_copy = tex->clone();
                                state.motion_vectors = new_motion_vectors;

                                spdlog::info("[TemporalUpscaler] Made clone of motion vectors @ {:x}", (uintptr_t)state.motion_vectors_copy.get());
                            }
                        }
                    }
                }
            }

            for (auto i = 0; i < 2; ++i) {
                const auto& state = m_eye_states[i];

                if (state.color == nullptr || state.depth_copy == nullptr || state.motion_vectors_copy == nullptr || state.color_copy == nullptr) {
                    spdlog::error("[TemporalUpscaler] Missing eye state for eye {} (D3D12)", i);
                    return;
                }

                if (state.depth_copy->get_d3d12_resource_container() == nullptr || 
                    state.motion_vectors_copy->get_d3d12_resource_container() == nullptr ||
                    state.color_copy->get_d3d12_resource_container() == nullptr) 
                {
                    spdlog::error("[TemporalUpscaler] Missing resource for eye {} (D3D12)", i);
                    return;
                }

                const auto motion_vectors = state.motion_vectors_copy->get_d3d12_resource_container()->get_native_resource();
                const auto depth = state.depth_copy->get_d3d12_resource_container()->get_native_resource();
                const auto color = state.color_copy->get_d3d12_resource_container()->get_native_resource();

                if (motion_vectors == nullptr || depth == nullptr || color == nullptr) {
                    spdlog::error("[TemporalUpscaler] Missing native resource for eye {} (D3D12)", i);
                    return;
                }

                const auto evaluate_id = get_evaluate_id(i);
                const auto evaluate_index = evaluate_id - 1;

                UpscaleParams params{};
                params.id = (int)evaluate_id;
                params.execute = i == 1;
                params.reset = false;
                params.color = color;
                params.motionVector = motion_vectors;
                params.depth = depth;
                params.mask = nullptr;
                params.destination = nullptr;
                params.motionScaleX = m_motion_scale[0];
                params.motionScaleY = m_motion_scale[1];
                params.renderSizeX = get_render_width();
                params.renderSizeY = get_render_height();
                params.jitterOffsetX = m_jitter_offsets[evaluate_index][0];
                params.jitterOffsetY = m_jitter_offsets[evaluate_index][1];
                params.sharpness = m_sharpness_amount->value();
                params.nearPlane = m_nearz;
                params.farPlane = m_farz;
                params.verticalFOV = m_fov;

                EvaluateUpscaler(&params);
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

                UpscaleParams params{};
                params.id = (int)evaluate_id;
                params.execute = true;
                params.reset = false;
                params.color = state.color.Get();
                params.motionVector = state.motion_vectors.Get();
                params.depth = state.depth.Get();
                params.mask = nullptr;
                params.destination = nullptr;
                params.motionScaleX = m_motion_scale[0];
                params.motionScaleY = m_motion_scale[1];
                params.renderSizeX = get_render_width();
                params.renderSizeY = get_render_height();
                params.jitterOffsetX = m_jitter_offsets[evaluate_index][0];
                params.jitterOffsetY = m_jitter_offsets[evaluate_index][1];
                params.sharpness = m_sharpness_amount->value();
                params.nearPlane = m_nearz;
                params.farPlane = m_farz;
                params.verticalFOV = m_fov;

                EvaluateUpscaler(&params);
                //SimpleEvaluate(
                    //evaluate_id, params.color, params.motionVector, params.depth, params.mask, params.destination,
                    //params.renderSizeX, params.renderSizeY, params.sharpness, params.jitterOffsetX, params.jitterOffsetY, params.motionScaleX, params.motionScaleY, params.reset, params.nearPlane, params.farPlane, params.verticalFOV, params.execute);

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

    InitLogDelegate([](char* msg, int size) {
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
        m_big_copier.setup();
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
    InitParams params{};
    params.id = get_evaluate_id(0);
    params.upscaleMethod = m_upscale_type;
    params.qualityLevel = m_upscale_quality->value();
    params.displaySizeX = out_w;
    params.displaySizeY = out_h;
    params.format = out_format;
    params.isContentHDR = false;
    params.depthInverted = true;
    params.YAxisInverted = false;
    params.motionVetorsJittered = false;
    params.enableSharpening = m_sharpness->value();
    params.enableAutoExposure = false;
    m_upscaled_textures[0] = InitUpscaler(&params);

    // Right eye.
    if (VR::get()->is_hmd_active()) {
        params.id = get_evaluate_id(1);
        m_upscaled_textures[1] = InitUpscaler(&params);
    }

    update_motion_scale();

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
    m_big_copier.reset();
    
    for (auto& state : m_eye_states) {
        state.color.Reset();
        state.depth.Reset();
        state.motion_vectors.Reset();
        state.scene_layer = nullptr;

        state.color_copy.reset();
        state.depth_copy.reset();
        state.motion_vectors_copy.reset();
    }

    m_big_motion_vectors.Reset();
    m_big_depth.Reset();
    m_big_color.Reset();
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
        result[0] = (float)get_render_width();
        result[1] = (float)get_render_height();
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

    auto& vr = VR::get();

    // Other layers appear when using scopes or mirrors are displayed
    if (!layer->is_fully_rendered() || !layer->has_main_camera()) {
        return;
    }

    // Multiple layers are not supported when not using VR
    if (!vr->is_hmd_active() || !vr->is_using_multipass()) {
        if (layer != m_eye_states[0].scene_layer) {
            return;
        }
    }

    if (vr->is_hmd_active() && vr->is_using_multipass()) {
        if (layer != m_eye_states[0].scene_layer && layer != m_eye_states[1].scene_layer) {
            return;
        }
    }

    auto scene_info = layer->get_scene_info();
    auto depth_distortion_scene_info = layer->get_depth_distortion_scene_info();
    auto filter_scene_info = layer->get_filter_scene_info();
    auto jitter_disable_scene_info = layer->get_jitter_disable_scene_info();
    auto jitter_disable_post_scene_info = layer->get_jitter_disable_post_scene_info();
    auto z_prepass_scene_info = layer->get_z_prepass_scene_info();

    uint32_t vr_index = 0;

    const auto vr_enabled = vr->is_hmd_active();
    const auto is_vr_multipass = vr_enabled && vr->is_using_multipass();

    if (vr->is_using_multipass()) {
        auto output_layer = sdk::renderer::get_output_layer();

        if (output_layer != nullptr) {
            const auto scenes = vr->get_camera_duplicator().get_relevant_scene_layers();

            if (!scenes.empty()) {
                if (layer == m_eye_states[0].scene_layer) {
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

void TemporalUpscaler::on_overlay_layer_draw(sdk::renderer::layer::Overlay* layer, void* render_context) {
    if (!ready()) {
        return;
    }

    auto context = (sdk::renderer::RenderContext*)render_context;
    auto scene_layer = (sdk::renderer::layer::Scene*)layer->get_parent();

    if (scene_layer == nullptr) {
        return;
    }

    for (auto& state : m_eye_states) {
        if (state.scene_layer != scene_layer) {
            continue;
        }

        auto depth = scene_layer->get_depth_stencil();

        if (depth != nullptr && state.depth_copy != nullptr) {
            context->copy_texture(state.depth_copy, depth);
        }

        auto motion_vectors_state = scene_layer->get_motion_vectors_state();

        if (motion_vectors_state != nullptr && state.motion_vectors_copy != nullptr) {
            auto rtv = motion_vectors_state->get_rtv(0);

            if (rtv != nullptr) {
                if (auto motion_vectors = rtv->get_texture_d3d12(); motion_vectors != nullptr) {
                    context->copy_texture(state.motion_vectors_copy, motion_vectors);
                }
            }
        }
    }
}

void TemporalUpscaler::on_prepare_output_layer_draw(sdk::renderer::layer::PrepareOutput* layer, void* render_context) {
    if (!ready()) {
        return;
    }

    auto context = (sdk::renderer::RenderContext*)render_context;
    auto scene_layer = (sdk::renderer::layer::Scene*)layer->get_parent();

    if (scene_layer == nullptr) {
        return;
    }

    const auto output_state = layer->get_output_state();

    if (output_state == nullptr) {
        return;
    }

    const auto rtv = output_state->get_rtv(0);

    if (rtv == nullptr) {
        return;
    }

    const auto tex = rtv->get_texture_d3d12();

    if (tex == nullptr) {
        return;
    }

    for (auto& state : m_eye_states) {
        if (state.scene_layer != scene_layer) {
            continue;
        }

        if (state.color_copy != nullptr) {
            context->copy_texture(state.color_copy, tex);
        }
    }
}

bool TemporalUpscaler::on_pre_output_layer_draw(sdk::renderer::layer::Output* layer, void* render_context) {
    return true;
}

void TemporalUpscaler::on_output_layer_draw(sdk::renderer::layer::Output* layer, void* render_context) {
}

bool TemporalUpscaler::on_pre_output_layer_update(sdk::renderer::layer::Output* layer, void* render_context) {
    return true;
}

void TemporalUpscaler::on_pre_application_entry(void* entry, const char* name, size_t hash) {
    if (hash == "BeginRendering"_fnv) {
        finish_release_resources();
    }

    if (!ready()) {
        return;
    }

    if (hash == "BeginRendering"_fnv) {
        //update_extra_scene_layer();
    }

    if (hash == "EndRendering"_fnv) {
        fix_output_layer();

        auto& vr = VR::get();
        const auto is_vr_multipass = vr->is_hmd_active() && vr->is_using_multipass();
        auto root_layer = sdk::renderer::get_root_layer();

        if (root_layer != nullptr) {
            auto [output_parent, output_layer] = root_layer->find_layer_recursive("via.render.layer.Output");
            auto valid_scene_layers =  is_vr_multipass ? vr->get_camera_duplicator().get_relevant_scene_layers() : (*output_layer)->find_fully_rendered_scene_layers();

            if (valid_scene_layers.empty()) {
                m_eye_states[0].scene_layer = nullptr;
                m_eye_states[1].scene_layer = nullptr;
                return;
            }

            if (valid_scene_layers.size() > 1) {
                if (!is_vr_multipass) {
                    if (m_displayed_scene == 0) {
                        m_eye_states[0].scene_layer = valid_scene_layers[0];
                    } else {
                        m_eye_states[0].scene_layer = valid_scene_layers[1];
                    }

                    m_eye_states[1].scene_layer = nullptr;
                } else {
                    m_eye_states[0].scene_layer = valid_scene_layers[0];
                    m_eye_states[1].scene_layer = valid_scene_layers[1];
                }
            } else {
                m_eye_states[0].scene_layer = valid_scene_layers[0];
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
                auto prepareoutput_layer = (sdk::renderer::layer::PrepareOutput**)state.scene_layer->find_layer(potype);

                if (prepareoutput_layer == nullptr) {
                    state.depth.Reset();
                    state.motion_vectors.Reset();
                    state.color.Reset();
                    continue;
                }

                if (!is_vr_multipass) {
                    auto new_depth = state.scene_layer->get_depth_stencil_d3d12();
                    auto new_motion_vectors = state.scene_layer->get_motion_vectors_d3d12();

                    state.depth = new_depth;
                    state.motion_vectors = new_motion_vectors;
                }

                if (prepareoutput_layer != nullptr && *prepareoutput_layer != nullptr) {
                    const auto current_target_state = (*prepareoutput_layer)->get_output_state();

                    if (current_target_state != nullptr) {
                        const auto new_color = current_target_state->get_native_resource_d3d12();

                        if (is_vr_multipass && new_color != nullptr && (state.color == nullptr || new_color != state.color.Get() || state.color_copy == nullptr)) {
                            const auto color_tex = current_target_state->get_rtv(0)->get_texture_d3d12();
                            state.color_copy = color_tex->clone();
                            state.color = new_color;

                            spdlog::info("[TemporalUpscaler] Created color copy");
                        } else if (!is_vr_multipass) {
                            state.color = new_color;
                        }
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

        static auto get_image_quality_rate_method = render_config_t->get_method("get_ImageQualityRate");
        static auto set_image_quality_rate_method = render_config_t->get_method("set_ImageQualityRate");

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

        // It's necessary to force the image quality to 1.0 otherwise
        // the motion & depth buffers become misaligned with the color buffer
        if (get_image_quality_rate_method != nullptr) {
            const auto image_quality_rate = get_image_quality_rate_method->call<float>(context, render_config);

            if (image_quality_rate != 1.0f) {
                set_image_quality_rate_method->call<void*>(context, render_config, 1.0f);
                spdlog::info("[TemporalUpscaler] Image quality rate set to 1.0");
            }
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

void TemporalUpscaler::update_extra_scene_layer() {
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

    auto scene_layers = output_layer->find_fully_rendered_scene_layers();
    if (scene_layers.empty()) {
        return;
    }

    if (m_made_extra_scene_layer && scene_layers.size() > 1) {
        /*if (!scene_layers.empty()) {
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
        }*/

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

    /*const auto tdef = sdk::find_type_definition("via.render.Mirror");
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
    }*/

    const auto tdef = sdk::find_type_definition("via.render.RenderOutput");
    if (tdef == nullptr) {
        spdlog::error("[TemporalUpscaler] Failed to find via.render.RenderOutput type definition");
        return;
    }

    const auto runtime_type = tdef->get_runtime_type();

    if (runtime_type == nullptr) {
        spdlog::error("[TemporalUpscaler] Failed to get via.render.RenderOutput runtime type");
        return;
    }

    auto comp = sdk::call_object_func_easy<sdk::renderer::RenderOutput*>(camera_gameobject, "getComponent(System.Type)", runtime_type);

    if (comp == nullptr) {
        spdlog::error("[TemporalUpscaler] Failed to get via.render.RenderOutput component");
        return;
    }

    if (m_eye_states[0].scene_layer == nullptr) {
        return; // wait.
    }

    auto& scene_layers_output = comp->get_scene_layers();

    if (scene_layers_output.empty()) {
        return; // wait.
    }

    if (scene_layers.size() == 1 && scene_layers[0] == scene_layers_output[0]) {
        scene_layers_output.emplace();
        scene_layers_output.num = 0;
    } else if (scene_layers.size() == 2 && scene_layers_output.size() == 1) {
        scene_layers_output[0] = (sdk::renderer::layer::Scene*)scene_layers[0];
        if (scene_layers_output.num_allocated == 1) {
            scene_layers_output.emplace();
        }
        scene_layers_output[1] = (sdk::renderer::layer::Scene*)scene_layers[1];
        m_made_extra_scene_layer = true;

        spdlog::info("[TemporalUpscaler] Made extra scene layer");

        if (!m_hooked_resource_release) {
            spdlog::info("[TemporalUpscaler] Hooking resource release");
            m_render_resource_release_hook = std::make_unique<FunctionHook>(sdk::renderer::RenderResource::get_release_fn(), &render_resource_release_hook);
            m_render_resource_release_hook->create();
            m_hooked_resource_release = true;

            spdlog::info("[TemporalUpscaler] Hooked resource release");
        }
    }
}

uint32_t TemporalUpscaler::get_render_width() const {
    if (m_use_native_resolution->value()) {
        // we subtract 1 from the native res because
        // the game will create a separate color buffer we can use
        // otherwise it will be null.
        return m_backbuffer_size[0] - 1;
    }

    return GetRenderWidth(get_evaluate_id(0));
}

uint32_t TemporalUpscaler::get_render_height() const {
    if (m_use_native_resolution->value()) {
        return m_backbuffer_size[1] - 1;
    }
    
    return GetRenderHeight(get_evaluate_id(0));
}

void TemporalUpscaler::on_render_resource_release(sdk::renderer::RenderResource* resource) {
    std::scoped_lock _{m_queued_release_resources_mutex};

    if (resource == nullptr) {
        return;
    }

    //if (resource->m_ref_count == 1) {
        m_queued_release_resources.push_back(resource);
    /*} else {
        const auto original = m_render_resource_release_hook->get_original<decltype(render_resource_release_hook)>();
        original(resource);
    }*/
}

void TemporalUpscaler::finish_release_resources() {
    if (!m_hooked_resource_release) {
        return;
    }

    std::scoped_lock _{m_queued_release_resources_mutex};

    if (!m_queued_release_resources.empty()) {
        const auto original = m_render_resource_release_hook->get_original<decltype(render_resource_release_hook)>();

        for (auto resource : m_queued_release_resources) {
            original(resource);
        }

        m_queued_release_resources.clear();
    }
}

void TemporalUpscaler::update_motion_scale() {
    m_motion_scale[0] = (float)get_render_width() / 2.0f;
    m_motion_scale[1] = -1.0f * ((float)get_render_height() / 2.0f);

    SetMotionScaleX(get_evaluate_id(0), (float)m_motion_scale[0]);
    SetMotionScaleY(get_evaluate_id(0), (float)m_motion_scale[1]);

    if (VR::get()->is_hmd_active()) {
        SetMotionScaleX(get_evaluate_id(1), (float)m_motion_scale[0]);
        SetMotionScaleY(get_evaluate_id(1), (float)m_motion_scale[1]);
    }
}

void TemporalUpscaler::render_resource_release_hook(sdk::renderer::RenderResource* resource) {
    TemporalUpscaler::get()->on_render_resource_release(resource);
}