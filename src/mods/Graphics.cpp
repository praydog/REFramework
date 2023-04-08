#include <sdk/SceneManager.hpp>

#include "VR.hpp"
#include "Graphics.hpp"

void Graphics::on_config_load(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }
}

void Graphics::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}

void Graphics::on_draw_ui() {
    ImGui::SetNextItemOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);
    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
    if (ImGui::TreeNode("RE4 Scope Tweaks")) {
        m_scope_tweaks->draw("Enable Scope Tweaks");

        if (m_scope_tweaks->value()) {
            m_scope_interlaced_rendering->draw("Enable Interlaced Rendering");
            m_scope_image_quality->draw("Scope Image Quality");
        }

        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
    if (ImGui::TreeNode("Ultrawide/FOV Options")) {
        if (m_ultrawide_fix->draw("Ultrawide/FOV/Aspect Ratio Fix") && m_ultrawide_fix->value() == false) {
            set_vertical_fov(false);
            do_ultrawide_fov_restore(true);
        }

        if (m_ultrawide_fix->value()) {
            m_ultrawide_vertical_fov->draw("Ultrawide: Enable Vertical FOV");
            m_ultrawide_fov->draw("Ultrawide: Override FOV");
            m_ultrawide_fov_multiplier->draw("Ultrawide: FOV Multiplier");
        }

        m_force_render_res_to_window->draw("Force Render Resolution to Window Size");

        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
    if (ImGui::TreeNode("GUI Options")) {
        m_disable_gui->draw("Hide GUI");
        ImGui::TreePop();
    }
}

void Graphics::on_present() {
    if (g_framework->is_dx11()) {
        const auto& hook = g_framework->get_d3d11_hook();
        const auto swapchain = hook->get_swap_chain();

        if (swapchain == nullptr) {
            return;
        }

        Microsoft::WRL::ComPtr<ID3D11Texture2D> backbuffer{};
        if (FAILED(swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer))) || backbuffer == nullptr) {
            return;
        }

        D3D11_TEXTURE2D_DESC desc{};
        backbuffer->GetDesc(&desc);

        const auto width = desc.Width;
        const auto height = desc.Height;

        if (m_backbuffer_size.has_value()) {
            (*m_backbuffer_size)[0] = width;
            (*m_backbuffer_size)[1] = height;
        } else {
            m_backbuffer_size = std::array<uint32_t, 2>{width, height};
        }
    } else {
        const auto& hook = g_framework->get_d3d12_hook();
        const auto swapchain = hook->get_swap_chain();

        if (swapchain == nullptr) {
            return;
        }

        Microsoft::WRL::ComPtr<ID3D12Resource> backbuffer{};
        if (FAILED(swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer))) || backbuffer == nullptr) {
            return;
        }

        const auto desc = backbuffer->GetDesc();
        const auto width = (uint32_t)desc.Width;
        const auto height = (uint32_t)desc.Height;

        if (m_backbuffer_size.has_value()) {
            (*m_backbuffer_size)[0] = width;
            (*m_backbuffer_size)[1] = height;
        } else {
            m_backbuffer_size = std::array<uint32_t, 2>{width, height};
        }
    }
}

void Graphics::on_pre_application_entry(void* entry, const char* name, size_t hash) {
    // To fix the world-space GUI icons.
    if (hash == "UpdateBehavior"_fnv) {
        do_ultrawide_fix();
    }

    if (hash == "UnlockScene"_fnv) {
        do_ultrawide_fov_restore();
    }
}

void Graphics::on_application_entry(void* entry, const char* name, size_t hash) {
    if (hash == "UpdateBehavior"_fnv) {
        do_ultrawide_fov_restore();
    }

    // To actually fix the rendering.
    if (hash == "LockScene"_fnv) {
        do_ultrawide_fix();
    }
}

bool Graphics::on_pre_gui_draw_element(REComponent* gui_element, void* primitive_context) {
    if (m_disable_gui->value()) {
        return false;
    }

    if (!m_ultrawide_fix->value()) {
        return true;
    }

    // Only stuff for RE4 right now.
#ifndef RE4
    if (true) {
        return true;
    }
#endif

    auto game_object = utility::re_component::get_game_object(gui_element);

    if (game_object != nullptr && game_object->transform != nullptr) {
        const auto name = utility::re_string::get_string(game_object->name);
        const auto name_hash = utility::hash(name);

        switch(name_hash) {
#if defined(RE4)
        case "Gui_ui2510"_fnv: // Black bars in cutscenes
            game_object->shouldDraw = false;
            return false;

        case "AcBackGround"_fnv: // Various screens that show the game background
        case "Gui_ArmouryTab"_fnv: // Typewriter storage
        case "Gui_ui3030"_fnv: // in inventory
        case "Gui_ui3040"_fnv: // just picked up an item
            if (game_object->shouldDraw && game_object->shouldUpdate) {
                std::unique_lock _{m_re4.time_mtx};
                m_re4.last_inventory_open = std::chrono::steady_clock::now();
            }
            break;
#endif

        default:
            break;
        }
    }

    return true;
}

void Graphics::on_view_get_size(REManagedObject* scene_view, float* result) {
    if (!m_force_render_res_to_window->value() || !m_backbuffer_size.has_value()) {
        return;
    }

    result[0] = (float)(*m_backbuffer_size)[0];
    result[1] = (float)(*m_backbuffer_size)[1];
}

void Graphics::do_scope_tweaks(sdk::renderer::layer::Scene* layer) {
#ifdef RE4
    if (!m_scope_tweaks->value()) {
        return;
    }
    const auto camera = layer->get_camera();

    if (camera == nullptr || !layer->is_enabled()) {
        return;
    }

    const auto camera_gameobject = utility::re_component::get_game_object(camera);

    if (camera_gameobject == nullptr || camera_gameobject->name == nullptr) {
        return;
    }

    const auto name = utility::re_string::get_view(camera_gameobject->name);

    if (name != L"ScopeCamera") {
        return;
    }

    static auto render_output_t = sdk::find_type_definition("via.render.RenderOutput");
    static auto render_output_tt = render_output_t->get_type();

    auto render_output = utility::re_component::find(camera, render_output_tt);

    if (render_output == nullptr) {
        return;
    }

    static auto set_image_quality_method = render_output_t->get_method("set_ImageQuality");
    static auto set_interleave_method = render_output_t->get_method("set_Interleave");

    if (set_image_quality_method != nullptr) {
        set_image_quality_method->call(sdk::get_thread_context(), render_output, m_scope_image_quality->value());
    }

    if (set_interleave_method != nullptr) {
        set_interleave_method->call(sdk::get_thread_context(), render_output, m_scope_interlaced_rendering->value());
    }
#endif
}

void Graphics::on_scene_layer_update(sdk::renderer::layer::Scene* layer, void* render_context) {
#ifdef RE4
    do_scope_tweaks(layer);
#endif
}

void Graphics::do_ultrawide_fix() {
    if (!m_ultrawide_fix->value()) {
        return;
    }

    // No need to perform ultrawide fix if VR is running.
    if (VR::get()->is_hmd_active()) {
        return;
    }

#if defined(RE4)
    {
        std::shared_lock _{m_re4.time_mtx};

        const auto now = std::chrono::steady_clock::now();
        if (now - m_re4.last_inventory_open < std::chrono::milliseconds(100)) {
            set_vertical_fov(false);
            return;
        }
    }
#endif

    set_vertical_fov(m_ultrawide_vertical_fov->value());

    static auto via_scene_view = sdk::find_type_definition("via.SceneView");
    static auto set_display_type_method = via_scene_view->get_method("set_DisplayType");

    auto main_view = sdk::get_main_view();

    if (main_view == nullptr) {
        return;
    }

    if (set_display_type_method != nullptr) {
        set_display_type_method->call(sdk::get_thread_context(), main_view, via::DisplayType::Fit);
    }
}

void Graphics::do_ultrawide_fov_restore(bool force) {
    if (!m_ultrawide_fix->value() && !force) {
        return;
    }

    // No need to perform ultrawide fix if VR is running.
    if (VR::get()->is_hmd_active()) {
        return;
    }

#if defined(RE4)
    const auto now = std::chrono::steady_clock::now();
    if (now - m_re4.last_inventory_open < std::chrono::milliseconds(100)) {
        return;
    }
#endif

    static auto via_camera = sdk::find_type_definition("via.Camera");
    static auto set_fov_method = via_camera->get_method("set_FOV");

    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    if (set_fov_method != nullptr) {
        if (m_ultrawide_fov->value()) {
            set_fov_method->call(sdk::get_thread_context(), camera, m_old_fov);
        }
    }
}

void Graphics::set_vertical_fov(bool enable) {
    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    static auto via_camera = sdk::find_type_definition("via.Camera");
    static auto set_vertical_enable_method = via_camera->get_method("set_VerticalEnable");
    static auto get_fov_method = via_camera->get_method("get_FOV");
    static auto set_fov_method = via_camera->get_method("set_FOV");

    if (set_vertical_enable_method != nullptr) {
        set_vertical_enable_method->call(sdk::get_thread_context(), camera, enable);
    }

    
#if defined(RE4)
    const auto now = std::chrono::steady_clock::now();
    if (now - m_re4.last_inventory_open < std::chrono::milliseconds(100)) {
        return;
    }
#endif

    if (m_ultrawide_fov->value() && get_fov_method != nullptr && set_fov_method != nullptr) {
        const auto hfov = get_fov_method->call<float>(sdk::get_thread_context(), camera);
        const auto vfov = std::clamp(hfov * m_ultrawide_fov_multiplier->value(), 0.01f, 179.9f);

        m_old_fov = hfov;
        set_fov_method->call(sdk::get_thread_context(), camera, vfov);
    }
}