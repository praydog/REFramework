#include <sdk/SceneManager.hpp>

#include "VR.hpp"
#include "Graphics.hpp"

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
#ifdef SF6
#include "sdk/regenny/sf6/via/Window.hpp"
#include "sdk/regenny/sf6/via/SceneView.hpp"
#elif defined(RE4)
#include "sdk/regenny/re4/via/Window.hpp"
#include "sdk/regenny/re4/via/SceneView.hpp"
#else
#include "sdk/regenny/mhrise_tdb71/via/Window.hpp"
#include "sdk/regenny/mhrise_tdb71/via/SceneView.hpp"
#endif
#endif

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

void Graphics::on_frame() {
    if (m_disable_gui_key->is_key_down_once()) {
        m_disable_gui->toggle();
    }
}

void Graphics::on_draw_ui() {
    ImGui::SetNextItemOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);
    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

#ifdef RE4
    ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
    if (ImGui::TreeNode("RE4 Scope Tweaks")) {
        m_scope_tweaks->draw("Enable Scope Tweaks");

        if (m_scope_tweaks->value()) {
            m_scope_interlaced_rendering->draw("Enable Interlaced Rendering");
            m_scope_image_quality->draw("Scope Image Quality");
        }

        ImGui::TreePop();
    }
#endif

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
        m_disable_gui_key->draw("Hide GUI key");
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
        // SF6 has some weird behavior where it doesn't restore the FOV correctly
        // corrupting the value
#ifndef SF6
        do_ultrawide_fix();
#endif
    }

    if (hash == "UnlockScene"_fnv) {
        do_ultrawide_fov_restore();
    }
}

void Graphics::on_application_entry(void* entry, const char* name, size_t hash) {
    if (hash == "UpdateBehavior"_fnv) {
#ifndef SF6
        do_ultrawide_fov_restore();
#endif
    }

    // To actually fix the rendering.
    if (hash == "LockScene"_fnv) {
        do_ultrawide_fix();
    }
}

void Graphics::fix_ui_element(REComponent* gui_element) {
    if (gui_element == nullptr) {
        return;
    }

    auto game_object = utility::re_component::get_game_object(gui_element);

    if (game_object == nullptr || game_object->transform == nullptr) {
        return;
    }

    const auto gui_component = utility::re_component::find<REComponent*>(game_object->transform, "via.gui.GUI");

    if (gui_component == nullptr) {
        return;
    }

    static const auto gui_t = sdk::find_type_definition("via.gui.GUI");
    static const auto view_t = sdk::find_type_definition("via.gui.View");

    if (gui_t == nullptr || view_t == nullptr) {
        return;
    }

    static const auto set_res_adjust_scale = view_t->get_method("set_ResAdjustScale(via.gui.ResolutionAdjustScale)");
    static const auto set_res_adjust_anchor = view_t->get_method("set_ResAdjustAnchor(via.gui.ResolutionAdjustAnchor)");
    static const auto set_resolution_adjust = view_t->get_method("set_ResolutionAdjust(System.Boolean)");
    static const auto get_view_type = view_t->get_method("get_ViewType");

    if (set_res_adjust_scale == nullptr || set_res_adjust_anchor == nullptr || set_resolution_adjust == nullptr || get_view_type == nullptr) {
        return;
    }

    static const auto get_view_method = gui_t->get_method("get_View");

    if (get_view_method == nullptr) {
        return;
    }

    const auto view = get_view_method->call<::REManagedObject*>(sdk::get_thread_context(), gui_component);

    if (view == nullptr) {
        return;
    }

    const auto is_screen_view = get_view_type != nullptr && 
                                get_view_type->call<int32_t>(sdk::get_thread_context(), view) == (int32_t)via::gui::ViewType::Screen;

    if (is_screen_view) {
        set_res_adjust_scale->call<void>(sdk::get_thread_context(), view, (int32_t)via::gui::ResolutionAdjustScale::FitSmallRatioAxis);
        set_res_adjust_anchor->call<void>(sdk::get_thread_context(), view, (int32_t)via::gui::ResolutionAdjustAnchor::CenterCenter);
        set_resolution_adjust->call<void>(sdk::get_thread_context(), view, true); // Causes the options to be applied/used
    }
}

bool Graphics::on_pre_gui_draw_element(REComponent* gui_element, void* primitive_context) {
    if (m_disable_gui->value()) {
        return false;
    }

    if (!m_ultrawide_fix->value()) {
        return true;
    }

    // TODO: Check how this interacts with the other games, could be useful for them too.
#if defined(SF6)
    fix_ui_element(gui_element);
#endif

    auto game_object = utility::re_component::get_game_object(gui_element);

    if (game_object != nullptr && game_object->transform != nullptr) {
        const auto name = utility::re_string::get_string(game_object->name);
        const auto name_hash = utility::hash(name);

        switch(name_hash) {
        // RE2/3?
        case "GUI_PillarBox"_fnv:
        case "GUIEventPillar"_fnv:
            game_object->shouldDraw = false;
            return false;

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
#ifdef SF6
    if (m_ultrawide_fix->value()) {
        auto regenny_view = (regenny::via::SceneView*)scene_view;
        auto window = regenny_view->window;

        if (window != nullptr) {
            window->borderless_size.w = (float)window->width;
            window->borderless_size.h = (float)window->height;
        }   
    }
#endif

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

    if (set_fov_method != nullptr && m_ultrawide_fov->value()) {
        std::scoped_lock _{m_fov_mutex};

        for (auto it : m_fov_map) {
            auto camera = it.first;
            set_fov_method->call(sdk::get_thread_context(), camera, m_fov_map[camera]);
            utility::re_managed_object::release(camera);
        }

        m_fov_map.clear();
    }
}

void Graphics::set_vertical_fov(bool enable) {
    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    static auto via_camera = sdk::find_type_definition("via.Camera");
    static auto get_vertical_enable_method = via_camera->get_method("get_VerticalEnable");
    static auto set_vertical_enable_method = via_camera->get_method("set_VerticalEnable");
    static auto get_fov_method = via_camera->get_method("get_FOV");
    static auto set_fov_method = via_camera->get_method("set_FOV");
    static auto get_aspect_method = via_camera->get_method("get_AspectRatio");

    bool was_vertical_fov_enabled = false;

    if (get_vertical_enable_method != nullptr) {
        was_vertical_fov_enabled = get_vertical_enable_method->call<bool>(sdk::get_thread_context(), camera);
    }

    if (set_vertical_enable_method != nullptr) {
        set_vertical_enable_method->call(sdk::get_thread_context(), camera, enable);
    }

    
#if defined(RE4)
    const auto now = std::chrono::steady_clock::now();
    if (now - m_re4.last_inventory_open < std::chrono::milliseconds(100)) {
        return;
    }
#endif

    if (m_ultrawide_fov->value() && get_fov_method != nullptr && set_fov_method != nullptr && get_aspect_method != nullptr) {
        const auto fov = get_fov_method->call<float>(sdk::get_thread_context(), camera);

        {
            std::scoped_lock _{m_fov_mutex};
            
            if (!m_fov_map.contains(camera)) {
                m_fov_map[camera] = fov;
                utility::re_managed_object::add_ref(camera);
            } else {
                m_fov_map[camera] = fov;
            }
        }

        if (!was_vertical_fov_enabled) {
            constexpr auto old_aspect_ratio = 16.0f / 9.0f;
            // convert hfov to vfov in radians
            const auto vfov_og = 2.0f * glm::atan(glm::tan(glm::radians(fov / 2.0f)) / old_aspect_ratio);

            const auto new_aspect_ratio = get_aspect_method->call<float>(sdk::get_thread_context(), camera);
            // convert vfov to hfov for new aspect ratio and convert to degrees
            const auto new_hfov = glm::degrees(2.0f * glm::atan(glm::tan(vfov_og / 2.0f) * new_aspect_ratio)) * m_ultrawide_fov_multiplier->value();

            set_fov_method->call(sdk::get_thread_context(), camera, new_hfov);   
        } else { // No need to calculate new hfov if vertical fov is enabled by the engine itself.
            const auto new_vfov = fov * m_ultrawide_fov_multiplier->value();

            set_fov_method->call(sdk::get_thread_context(), camera, new_vfov);
        }
    }
}
