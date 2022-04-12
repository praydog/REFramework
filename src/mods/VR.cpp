#define NOMINMAX

#include <fstream>
#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtx/transform.hpp>

#if defined(RE2) || defined(RE3) || defined(DMC5)
#include "sdk/regenny/re3/via/Window.hpp"
#include "sdk/regenny/re3/via/SceneView.hpp"
#elif defined(RE7)
#include "sdk/regenny/re7/via/Window.hpp"
#include "sdk/regenny/re7/via/SceneView.hpp"
#else
#include "sdk/regenny/re8/via/Window.hpp"
#include "sdk/regenny/re8/via/SceneView.hpp"
#endif

#include "sdk/Math.hpp"
#include "sdk/SceneManager.hpp"
#include "sdk/RETypeDB.hpp"
#include "sdk/Renderer.hpp"
#include "sdk/Application.hpp"
#include "sdk/Renderer.hpp"
#include "sdk/REMath.hpp"

#include "utility/Scan.hpp"
#include "utility/FunctionHook.hpp"
#include "utility/Module.hpp"
#include "utility/Memory.hpp"

#include "FirstPerson.hpp"
#include "ManualFlashlight.hpp"
#include "VR.hpp"

constexpr std::string_view COULD_NOT_LOAD_OPENVR = "Could not load openvr_api.dll";

bool inside_on_end = false;
uint32_t actual_frame_count = 0;

thread_local bool inside_gui_draw = false;

std::shared_ptr<VR>& VR::get() {
    static std::shared_ptr<VR> inst{};

    if (inst == nullptr) {
        inst = std::make_shared<VR>();
    }

    return inst;
}

std::unique_ptr<FunctionHook> g_get_size_hook{};
std::unique_ptr<FunctionHook> g_input_hook{};
std::unique_ptr<FunctionHook> g_projection_matrix_hook{};
std::unique_ptr<FunctionHook> g_projection_matrix_hook2{};
std::unique_ptr<FunctionHook> g_view_matrix_hook{};
std::unique_ptr<FunctionHook> g_overlay_draw_hook{};
std::unique_ptr<FunctionHook> g_post_effect_draw_hook{};
std::unique_ptr<FunctionHook> g_wwise_listener_update_hook{};
//std::unique_ptr<FunctionHook> g_get_sharpness_hook{};

#ifdef RE7
std::optional<regenny::via::Size> g_previous_size{};
#endif

// Purpose: spoof the render target size to the size of the HMD displays
float* VR::get_size_hook(REManagedObject* scene_view, float* result) {
    auto original_func = g_get_size_hook->get_original<decltype(VR::get_size_hook)>();

    if (!g_framework->is_ready()) {
        return original_func(scene_view, result);
    }

    auto mod = VR::get();

    if (mod->m_disable_backbuffer_size_override) {
        return original_func(scene_view, result);
    }

    auto regenny_view = (regenny::via::SceneView*)scene_view;
    auto window = regenny_view->window;

    // Force the display to stretch to the window size
    regenny_view->display_type = regenny::via::DisplayType::Fit;

    auto wanted_width = 0.0f;
    auto wanted_height = 0.0f;

    // Set the window size, which will increase the size of the backbuffer
    if (window != nullptr) {
        if (mod->m_is_hmd_active) {
#ifdef RE7
            if (!g_previous_size) {
                g_previous_size = regenny::via::Size{ (float)window->width, (float)window->height };
            }
#endif
            window->width = mod->get_hmd_width();
            window->height = mod->get_hmd_height();

            if (mod->m_is_d3d12 && mod->m_d3d12.is_initialized()) {
                const auto& backbuffer_size = mod->m_d3d12.get_backbuffer_size();

                if (backbuffer_size[0] > 0 && backbuffer_size[1] > 0) {
                    if (std::abs((int)backbuffer_size[0] - (int)window->width) > 50 || std::abs((int)backbuffer_size[1] - (int)window->height) > 50) {
                        const auto now = mod->m_frame_count;

                        if (!mod->m_backbuffer_inconsistency) {
                            mod->m_backbuffer_inconsistency_start = now;
                            mod->m_backbuffer_inconsistency = true;
                        }

                        const auto is_true_inconsistency = (now - mod->m_backbuffer_inconsistency_start) >= 5;

                        if (is_true_inconsistency) {
                            // Force a reset of the backbuffer size
                            window->width = mod->get_hmd_width() + 1;
                            window->height = mod->get_hmd_height() + 1;

                            // m_backbuffer_inconsistency gets set to false on device reset.
                        }
                    }
                } else {
                    mod->m_backbuffer_inconsistency = false;
                }
            }
        } else {
            mod->m_backbuffer_inconsistency = false;

#ifndef RE7
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
    }

    auto out = original_func(scene_view, result);

    if (!mod->m_in_render) {
        //return original_func(scene_view, result);
    }

    // spoof the size to the HMD's size
    out[0] = wanted_width;
    out[1] = wanted_height;

    return out;
}

Matrix4x4f* VR::camera_get_projection_matrix_hook(REManagedObject* camera, Matrix4x4f* result) {
    auto original_func = g_projection_matrix_hook->get_original<decltype(VR::camera_get_projection_matrix_hook)>();

    auto vr = VR::get();

    if (result == nullptr || !g_framework->is_ready() || !vr->m_is_hmd_active || vr->m_disable_projection_matrix_override) {
        return original_func(camera, result);
    }

    /*if (camera != sdk::get_primary_camera()) {
        return original_func(camera, result);
    }*/

    if (!vr->m_in_render) {
       // return original_func(camera, result);
    }

    if (vr->m_in_lightshaft) {
        //return original_func(camera, result);
    }

    // Get the projection matrix for the correct eye
    // For some reason we need to flip the projection matrix here?
    *result = vr->get_current_projection_matrix(false);

    return result;
}

Matrix4x4f* VR::gui_camera_get_projection_matrix_hook(REManagedObject* camera, Matrix4x4f* result) {
    auto original_func = g_projection_matrix_hook2->get_original<decltype(VR::gui_camera_get_projection_matrix_hook)>();

    auto vr = VR::get();

    if (result == nullptr || !g_framework->is_ready() || !vr->m_is_hmd_active || vr->m_disable_gui_camera_projection_matrix_override) {
        return original_func(camera, result);
    }

    /*if (camera != sdk::get_primary_camera()) {
        return original_func(camera, result);
    }*/

    if (!vr->m_in_render) {
       // return original_func(camera, result);
    }

    if (vr->m_in_lightshaft) {
        //return original_func(camera, result);
    }

    // Get the projection matrix for the correct eye
    // For some reason we need to flip the projection matrix here?
#ifndef RE7
    *result = vr->get_current_projection_matrix(false);
#else
    *result = vr->get_current_projection_matrix(true);
#endif

    return result;
}

Matrix4x4f* VR::camera_get_view_matrix_hook(REManagedObject* camera, Matrix4x4f* result) {
    auto original_func = g_view_matrix_hook->get_original<decltype(VR::camera_get_view_matrix_hook)>();

    if (result == nullptr || !g_framework->is_ready()) {
        return original_func(camera, result);
    }

    auto vr = VR::get();

    if (!vr->m_is_hmd_active || vr->m_disable_view_matrix_override) {
        return original_func(camera, result);
    }

    if (camera != sdk::get_primary_camera()) {
        return original_func(camera, result);
    }

    original_func(camera, result);

    auto& mtx = *result;

    //get the flipped eye to get the correct transform. something something right->left handedness i think
    const auto current_eye_transform = vr->get_current_eye_transform(true);
    //auto current_head_pos = -(glm::inverse(vr->get_rotation(0)) * ((vr->get_position(0)) - vr->m_standing_origin));
    //current_head_pos.w = 0.0f;

    // Apply the complete eye transform. This fixes the need for parallel projections on all canted headsets like Pimax
    mtx = current_eye_transform * mtx;

    return result;
}

void VR::inputsystem_update_hook(void* ctx, REManagedObject* input_system) {
    auto original_func = g_input_hook->get_original<decltype(VR::inputsystem_update_hook)>();

    if (!g_framework->is_ready()) {
        original_func(ctx, input_system);
        return;
    }

    auto mod = VR::get();
    const auto now = std::chrono::steady_clock::now();
    auto is_using_controller = (now - mod->get_last_controller_update()) <= std::chrono::seconds(10);

    if (mod->get_controllers().empty()) {
        // no controllers connected, don't do anything
        original_func(ctx, input_system);
        return;
    }

    auto lstick = sdk::call_object_func<REManagedObject*>(input_system, "get_LStick", sdk::get_thread_context());
    auto rstick = sdk::call_object_func<REManagedObject*>(input_system, "get_RStick", sdk::get_thread_context());

    if (lstick == nullptr || rstick == nullptr) {
        original_func(ctx, input_system);
        return;
    }

    auto button_bits_obj = sdk::call_object_func<REManagedObject*>(input_system, "get_ButtonBits", sdk::get_thread_context(), input_system);

    if (button_bits_obj == nullptr) {
        original_func(ctx, input_system);
        return;
    }

    auto left_axis = mod->get_left_stick_axis();
    auto right_axis = mod->get_right_stick_axis();
    const auto left_axis_len = glm::length(left_axis);
    const auto right_axis_len = glm::length(right_axis);

    // Current actual button bits used by the game
    auto& button_bits_down = *sdk::get_object_field<uint64_t>(button_bits_obj, "Down");
    auto& button_bits_on = *sdk::get_object_field<uint64_t>(button_bits_obj, "On");
    auto& button_bits_up = *sdk::get_object_field<uint64_t>(button_bits_obj, "Up");

    //button_bits_down |= mod->m_button_states_down.to_ullong();
    //button_bits_on |= mod-> m_button_states_on.to_ullong();
    //button_bits_up |= mod->m_button_states_up.to_ullong();

    auto keep_button_down = [&](app::ropeway::InputDefine::Kind button) {
        if ((mod->m_button_states_on.to_ullong() & (uint64_t)button) == 0 && (mod->m_button_states_down.to_ullong() & (uint64_t)button) == 0) {
            return;
        }

        if ((mod->m_button_states_on.to_ullong() & (uint64_t)button) == 0) {
            if (mod->m_button_states_down.to_ullong() & (uint64_t)button) {
                button_bits_on |= (uint64_t)button;
                button_bits_down &= ~(uint64_t)button;
            } else {
                button_bits_down |= (uint64_t)button;
            }
        } else {
            button_bits_down &= ~(uint64_t)button;
            button_bits_on |= (uint64_t)button;
        }
    };

    const auto deadzone = mod->m_joystick_deadzone->value();

    if (left_axis_len > deadzone) {
        mod->m_last_controller_update = now;
        is_using_controller = true;

        // Override the left stick's axis values to the VR controller's values
        Vector3f axis{ left_axis.x, left_axis.y, 0.0f };
        sdk::call_object_func<void*>(lstick, "update", sdk::get_thread_context(), lstick, &axis, &axis);

        keep_button_down(app::ropeway::InputDefine::Kind::UI_L_STICK);
    }

    if (right_axis_len > deadzone) {
        mod->m_last_controller_update = now;
        is_using_controller = true;

        // Override the right stick's axis values to the VR controller's values
        Vector3f axis{ right_axis.x, right_axis.y, 0.0f };
        sdk::call_object_func<void*>(rstick, "update", sdk::get_thread_context(), rstick, &axis, &axis);

        keep_button_down(app::ropeway::InputDefine::Kind::UI_R_STICK);
    }

    // Causes the right stick to take effect properly
    if (is_using_controller) {
        sdk::call_object_func<void*>(input_system, "set_InputMode", sdk::get_thread_context(), input_system, app::ropeway::InputDefine::InputMode::Pad);
    }

    original_func(ctx, input_system);

    mod->openvr_input_to_re2_re3(input_system);
}

void VR::RenderLayerHook<sdk::renderer::layer::Overlay>::draw(sdk::renderer::layer::Overlay* layer, void* render_ctx) {
    auto mod = VR::get();
    auto original_func = mod->m_layer_hooks.overlay.draw_hook->get_original<decltype(VR::RenderLayerHook<sdk::renderer::layer::Overlay>::draw)>();

    if (!g_framework->is_ready()) {
        original_func(layer, render_ctx);
        return;
    }

    // just don't render anything at all.
    // overlays just seem to break stuff in VR.
    if (!mod->m_is_hmd_active) {
        original_func(layer, render_ctx);
        return;
    }

    // NOT RE3
    // for some reason RE3 has weird issues with the overlay rendering
    // causing double vision
#ifndef RE3
    if (mod->m_allow_engine_overlays->value()) {
        original_func(layer, render_ctx);
    }
#endif
}

void VR::RenderLayerHook<sdk::renderer::layer::Overlay>::update(sdk::renderer::layer::Overlay* layer, void* render_ctx) {
    auto mod = VR::get();
    auto original_func = mod->m_layer_hooks.overlay.update_hook->get_original<decltype(VR::RenderLayerHook<sdk::renderer::layer::Overlay>::update)>();

    if (!g_framework->is_ready() || !mod->m_is_hmd_active) {
        original_func(layer, render_ctx);
        return;
    }

    original_func(layer, render_ctx);
}

void VR::RenderLayerHook<sdk::renderer::layer::PostEffect>::draw(sdk::renderer::layer::PostEffect* layer, void* render_ctx) {
    auto mod = VR::get();
    auto original_func = mod->m_layer_hooks.post_effect.draw_hook->get_original<decltype(VR::RenderLayerHook<sdk::renderer::layer::PostEffect>::draw)>();

    if (!g_framework->is_ready() || !mod->m_is_hmd_active) {
        original_func(layer, render_ctx);
        return;
    }

    auto scene_layer = layer->get_parent();
    uint32_t previous_distortion_type = 0;

    const auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        original_func(layer, render_ctx);
        return;
    }
    
    static auto render_output_type = sdk::find_type_definition("via.render.RenderOutput")->get_type();
    auto render_output_component = utility::re_component::find(camera, render_output_type);

    if (render_output_component == nullptr) {
        original_func(layer, render_ctx);
        return;
    }

    if (!mod->m_disable_post_effect_fix) {
        // Set the distortion type back to flatscreen mode
        // this will fix various graphical bugs
        //sdk::call_object_func_easy<void*>(render_output_component, "set_DistortionType", 0); // None

        if (scene_layer != nullptr) {
            previous_distortion_type = sdk::call_object_func_easy<uint32_t>(scene_layer, "get_DistortionType");
            sdk::call_object_func_easy<void*>(scene_layer, "set_DistortionType", 0); // None
        }
    }

    // call the original func
    original_func(layer, render_ctx);
    
    if (!mod->m_disable_post_effect_fix) {
        // Restore the distortion type back to VR mode
        // to fix TAA
        if (scene_layer != nullptr) {
            sdk::call_object_func_easy<void*>(scene_layer, "set_DistortionType", previous_distortion_type); // Left
        }

        //mod->fix_temporal_effects();
    }
}

void VR::RenderLayerHook<sdk::renderer::layer::PostEffect>::update(sdk::renderer::layer::PostEffect* layer, void* render_ctx) {
    auto mod = VR::get();
    auto original_func = mod->m_layer_hooks.post_effect.update_hook->get_original<decltype(VR::RenderLayerHook<sdk::renderer::layer::PostEffect>::update)>();

    if (!g_framework->is_ready() || !mod->m_is_hmd_active) {
        original_func(layer, render_ctx);
        return;
    }

    original_func(layer, render_ctx);
}

void VR::RenderLayerHook<sdk::renderer::layer::Scene>::draw(sdk::renderer::layer::Scene* layer, void* render_ctx) {
    auto mod = VR::get();
    auto original_func = mod->m_layer_hooks.scene.draw_hook->get_original<decltype(VR::RenderLayerHook<sdk::renderer::layer::Scene>::draw)>();

    if (!g_framework->is_ready() || !mod->m_is_hmd_active) {
        original_func(layer, render_ctx);
        return;
    }

    original_func(layer, render_ctx);
}

void VR::RenderLayerHook<sdk::renderer::layer::Scene>::update(sdk::renderer::layer::Scene* layer, void* render_ctx) {
    auto mod = VR::get();
    auto original_func = mod->m_layer_hooks.scene.update_hook->get_original<decltype(VR::RenderLayerHook<sdk::renderer::layer::Scene>::update)>();

    if (!g_framework->is_ready() || !mod->m_is_hmd_active) {
        original_func(layer, render_ctx);
        return;
    }

    auto scene_info = layer->get_scene_info();

    if (mod->m_disable_temporal_fix) {
        original_func(layer, render_ctx);
        return;
    }

    // Temporal fix
    const auto prev_view_proj = scene_info->view_projection_matrix;
    original_func(layer, render_ctx);
    scene_info->old_view_projection_matrix = prev_view_proj;
}

void VR::wwise_listener_update_hook(void* listener) {
    auto original_func = g_wwise_listener_update_hook->get_original<decltype(VR::wwise_listener_update_hook)>();

    if (!g_framework->is_ready()) {
        original_func(listener);
        return;
    }

    auto mod = VR::get();

    if (!mod->m_is_hmd_active || !mod->m_openvr_loaded) {
        original_func(listener);
        return;
    }

    if (!mod->m_hmd_oriented_audio->value()) {
        original_func(listener);
        return;
    }

    std::scoped_lock _{mod->m_wwise_mtx};

#if defined(RE2) || defined(RE3)
    const auto skip_camera_set = FirstPerson::get()->will_be_used();
#else
    const auto skip_camera_set = false;
#endif

    if (!skip_camera_set) {
        mod->update_audio_camera();
    }

#ifndef RE7
    constexpr auto CAMERA_OFFSET = 0x50;
#else
    constexpr auto CAMERA_OFFSET = 0x58;
#endif

    auto& listener_camera = *(::REManagedObject**)((uintptr_t)listener + CAMERA_OFFSET);
    bool changed = false;

    if (listener_camera == nullptr) {
        auto primary_camera = sdk::get_primary_camera();

        if (primary_camera != nullptr) {
            listener_camera = primary_camera;
            changed = true;
        }
    }

    original_func(listener);

    if (changed) {
        listener_camera = nullptr;
    }

    if (!skip_camera_set) {
        mod->restore_audio_camera();
    }
}

// put it on the backburner
/*
float VR::get_sharpness_hook(void* tonemapping) {
    auto original_func = g_get_sharpness_hook->get_original<decltype(get_sharpness_hook)>();
    
    if (!g_framework->is_ready()) {
        return original_func(tonemapping);
    }

    auto mod = VR::get();

    if (mod->m_disable_sharpening) {
        return 0.0f;
    }

    return original_func(tonemapping);
}
*/

// Called when the mod is initialized
std::optional<std::string> VR::on_initialize() {
    auto openvr_error = initialize_openvr();

    if (openvr_error || !m_openvr_loaded) {
        m_is_hmd_active = false;
        m_was_hmd_active = false;
        m_needs_wgp_update = false;

        if (!m_openvr_loaded) {
            // this is okay. we're not going to fail the whole thing entirely
            // so we're just going to return OK, but
            // when the VR mod draws its menu, it'll say "VR is not available"
            return Mod::on_initialize();
        }

        return openvr_error;
    }

    auto openxr_error = initialize_openxr();

    if (openxr_error || !m_openxr.loaded) {
        // TODO: do something here
    }

    auto hijack_error = hijack_resolution();

    if (hijack_error) {
        return hijack_error;
    }

    hijack_error = hijack_input();

    if (hijack_error) {
        return hijack_error;
    }

    hijack_error = hijack_camera();

    if (hijack_error) {
        return hijack_error;
    }

    hijack_error = hijack_render_layer(m_layer_hooks.overlay);

    if (hijack_error) {
        return hijack_error;
    }

    hijack_error = hijack_render_layer(m_layer_hooks.post_effect);

    if (hijack_error) {
        return hijack_error;
    }

    hijack_error = hijack_render_layer(m_layer_hooks.scene);

    if (hijack_error) {
        return hijack_error;
    }

    hijack_error = hijack_wwise_listeners();

    if (hijack_error) {
        return hijack_error;
    }

    // all OK
    return Mod::on_initialize();
}

void VR::on_lua_state_created(sol::state& lua) {
    lua.new_usertype<VR>("VR",
        "get_controllers", &VR::get_controllers,
        "get_position", &VR::get_position,
        "get_velocity", &VR::get_velocity,
        "get_angular_velocity", &VR::get_angular_velocity,
        "get_rotation", &VR::get_rotation,
        "get_transform", &VR::get_transform,
        "get_left_stick_axis", &VR::get_left_stick_axis,
        "get_right_stick_axis", &VR::get_right_stick_axis,
        "get_current_eye_transform", &VR::get_current_eye_transform,
        "get_current_projection_matrix", &VR::get_current_projection_matrix,
        "get_standing_origin", &VR::get_standing_origin,
        "set_standing_origin", &VR::set_standing_origin,
        "get_rotation_offset", &VR::get_rotation_offset,
        "set_rotation_offset", &VR::set_rotation_offset,
        "recenter_view", &VR::recenter_view,
        "get_gui_rotation_offset", &VR::get_gui_rotation_offset,
        "set_gui_rotation_offset", &VR::set_gui_rotation_offset,
        "recenter_gui", &VR::recenter_gui,
        "get_action_set", &VR::get_action_set,
        "get_active_action_set", &VR::get_active_action_set,
        "get_action_trigger", &VR::get_action_trigger,
        "get_action_grip", &VR::get_action_grip,
        "get_action_joystick", &VR::get_action_joystick,
        "get_action_joystick_click", &VR::get_action_joystick_click,
        "get_action_a_button", &VR::get_action_a_button,
        "get_action_b_button", &VR::get_action_b_button,
        "get_action_weapon_dial", &VR::get_action_weapon_dial,
        "get_action_minimap", &VR::get_action_minimap,
        "get_action_block", &VR::get_action_block,
        "get_action_dpad_up", &VR::get_action_dpad_up,
        "get_action_dpad_down", &VR::get_action_dpad_down,
        "get_action_dpad_left", &VR::get_action_dpad_left,
        "get_action_dpad_right", &VR::get_action_dpad_right,
        "get_action_heal", &VR::get_action_heal,
        "get_left_joystick", &VR::get_left_joystick,
        "get_right_joystick", &VR::get_right_joystick,
        "is_using_controllers", &VR::is_using_controllers,
        "is_openvr_loaded", &VR::is_openvr_loaded,
        "is_hmd_active", &VR::is_hmd_active,
        "is_action_active", &VR::is_action_active,
        "is_using_hmd_oriented_audio", &VR::is_using_hmd_oriented_audio,
        "toggle_hmd_oriented_audio", &VR::toggle_hmd_oriented_audio,
        "apply_hmd_transform", [](VR* vr, glm::quat& rotation, Vector4f& position) {
            vr->apply_hmd_transform(rotation, position);
        },
        "trigger_haptic_vibration", &VR::trigger_haptic_vibration,
        "get_last_render_matrix", &VR::get_last_render_matrix
    );

    lua["vrmod"] = this;
}

std::optional<std::string> VR::initialize_openvr() {
    m_openvr_loaded = false;

    if (LoadLibraryA("openvr_api.dll") == nullptr) {
        m_openvr_dll_missing = true;
        m_openvr_error = COULD_NOT_LOAD_OPENVR.data();
        return Mod::on_initialize();
    }

    m_d3d12.on_reset(this);
    m_d3d11.on_reset(this);

    m_needs_wgp_update = true;
    m_wgp_initialized = false;
    m_is_hmd_active = true;
    m_was_hmd_active = true;

    auto error = vr::VRInitError_None;
	m_hmd = vr::VR_Init(&error, vr::VRApplication_Scene);

    // check if error
    if (error != vr::VRInitError_None) {
        m_openvr_error = "VR_Init failed: " + std::string{vr::VR_GetVRInitErrorAsEnglishDescription(error)};
        return Mod::on_initialize();
    }

    if (m_hmd == nullptr) {
        m_openvr_error = "VR_Init failed: HMD is null";
        return Mod::on_initialize();
    }

    // get render target size
    m_hmd->GetRecommendedRenderTargetSize(&m_w, &m_h);

    if (vr::VRCompositor() == nullptr) {
        m_openvr_error = "VRCompositor failed to initialize.";
        return Mod::on_initialize();
    }

    auto input_error = initialize_openvr_input();

    if (input_error) {
        m_openvr_error = *input_error;
        return Mod::on_initialize();
    }

    auto overlay_error = m_overlay_component.on_initialize_openvr();

    if (overlay_error) {
        m_openvr_error = *overlay_error;
        return Mod::on_initialize();
    }
    
    m_openvr_loaded = true;
    m_openvr_error = std::nullopt;

    return Mod::on_initialize();
}

std::optional<std::string> VR::initialize_openvr_input() {
    const auto module_directory = *utility::get_module_directory(g_framework->get_module().as<HMODULE>());

    // write default actions and bindings with the static strings we have
    for (auto& it : m_binding_files) {
        spdlog::info("Writing default binding file {}", it.first);

        std::ofstream file{ module_directory + "/" + it.first };
        file << it.second;
    }

    const auto actions_path = module_directory + "/actions.json";
    auto input_error = vr::VRInput()->SetActionManifestPath(actions_path.c_str());

    if (input_error != vr::VRInputError_None) {
        return "VRInput failed to set action manifest path: " + std::to_string((uint32_t)input_error);
    }

    // get action set
    auto action_set_error = vr::VRInput()->GetActionSetHandle("/actions/default", &m_action_set);

    if (action_set_error != vr::VRInputError_None) {
        return "VRInput failed to get action set: " + std::to_string((uint32_t)action_set_error);
    }

    if (m_action_set == vr::k_ulInvalidActionSetHandle) {
        return "VRInput failed to get action set handle.";
    }

    for (auto& it : m_action_handles) {
        auto error = vr::VRInput()->GetActionHandle(it.first.c_str(), &it.second.get());

        if (error != vr::VRInputError_None) {
            return "VRInput failed to get action handle: (" + it.first + "): " + std::to_string((uint32_t)error);
        }

        if (it.second == vr::k_ulInvalidActionHandle) {
            return "VRInput failed to get action handle: (" + it.first + ")";
        }
    }

    m_active_action_set.ulActionSet = m_action_set;
    m_active_action_set.ulRestrictedToDevice = vr::k_ulInvalidInputValueHandle;
    m_active_action_set.nPriority = 0;

    detect_controllers();

    return std::nullopt;
}

std::optional<std::string> VR::initialize_openxr() {
    if (LoadLibraryA("openxr_loader.dll") == nullptr) {
        m_openxr.loaded = false;
        m_openxr.error = "Could not load openxr_loader.dll";

        return std::nullopt;
    }

    // Step 1: Create an instance
    spdlog::info("[VR] Creating OpenXR instance");

    std::vector<const char*> extensions{};

    if (g_framework->is_dx12()) {
        extensions.push_back(XR_KHR_D3D12_ENABLE_EXTENSION_NAME);
    } else {
        extensions.push_back(XR_KHR_D3D11_ENABLE_EXTENSION_NAME);
    }

    XrInstanceCreateInfo instance_create_info{XR_TYPE_INSTANCE_CREATE_INFO};
    instance_create_info.next = nullptr;
    instance_create_info.enabledExtensionCount = (uint32_t)extensions.size();
    instance_create_info.enabledExtensionNames = extensions.data();

    strcpy(instance_create_info.applicationInfo.applicationName, g_framework->get_game_name());
    instance_create_info.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    auto result = xrCreateInstance(&instance_create_info, &m_openxr.instance);

    // we can't convert the result to a string here
    // because the function requires the instance to be valid
    if (result != XR_SUCCESS) {
        m_openxr.error = "Could not create openxr instance: " + std::to_string((uint32_t)result);
        spdlog::error("[VR] {}", m_openxr.error.value());

        return std::nullopt;
    }
    
    // Step 2: Create a system
    spdlog::info("[VR] Creating OpenXR system");

    XrSystemGetInfo system_info{XR_TYPE_SYSTEM_GET_INFO};
    system_info.formFactor = m_openxr.form_factor;

    result = xrGetSystem(m_openxr.instance, &system_info, &m_openxr.system);

    std::string result_string{};
    result_string.resize(XR_MAX_RESULT_STRING_SIZE);

    if (result != XR_SUCCESS) {
        xrResultToString(m_openxr.instance, result, result_string.data());
        m_openxr.error = "Could not create openxr system: " + result_string;
        spdlog::error("[VR] {}", m_openxr.error.value());

        return std::nullopt;
    }

    // Step 3: Create a session
    spdlog::info("[VR] Creating OpenXR session");

    XrSessionCreateInfo session_create_info{XR_TYPE_SESSION_CREATE_INFO};

    if (g_framework->is_dx12()) {
        m_d3d12.openxr().initialize();
        session_create_info.next = &m_d3d12.openxr().binding;
    } else {
        m_d3d11.openxr().initialize();
        session_create_info.next = &m_d3d11.openxr().binding;
    }

    session_create_info.systemId = m_openxr.system;

    result = xrCreateSession(m_openxr.instance, &session_create_info, &m_openxr.session);

    if (result != XR_SUCCESS) {
        xrResultToString(m_openxr.instance, result, result_string.data());
        m_openxr.error = "Could not create openxr session: " + result_string;
        spdlog::error("[VR] {}", m_openxr.error.value());

        return std::nullopt;
    }

    // Step 4: Create a space
    spdlog::info("[VR] Creating OpenXR space");

    XrReferenceSpaceCreateInfo space_create_info{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
    space_create_info.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
    space_create_info.poseInReferenceSpace = {};
    space_create_info.poseInReferenceSpace.orientation.w = 1.0f;

    result = xrCreateReferenceSpace(m_openxr.session, &space_create_info, &m_openxr.space);

    if (result != XR_SUCCESS) {
        xrResultToString(m_openxr.instance, result, result_string.data());
        m_openxr.error = "Could not create openxr space: " + result_string;
        spdlog::error("[VR] {}", m_openxr.error.value());

        return std::nullopt;
    }

    // Step 5: Get the system properties
    spdlog::info("[VR] Getting OpenXR system properties");

    XrSystemProperties system_properties{XR_TYPE_SYSTEM_PROPERTIES};
    result = xrGetSystemProperties(m_openxr.instance, m_openxr.system, &system_properties);

    if (result != XR_SUCCESS) {
        xrResultToString(m_openxr.instance, result, result_string.data());
        m_openxr.error = "Could not get system properties: " + result_string;
        spdlog::error("[VR] {}", m_openxr.error.value());

        return std::nullopt;
    }

    spdlog::info("[VR] OpenXR system Name: {}", system_properties.systemName);
    spdlog::info("[VR] OpenXR system Vendor: {}", system_properties.vendorId);
    spdlog::info("[VR] OpenXR system max width: {}", system_properties.graphicsProperties.maxSwapchainImageWidth);
    spdlog::info("[VR] OpenXR system max height: {}", system_properties.graphicsProperties.maxSwapchainImageHeight);
    spdlog::info("[VR] OpenXR system supports {} layers", system_properties.graphicsProperties.maxLayerCount);
    spdlog::info("[VR] OpenXR system orientation: {}", system_properties.trackingProperties.orientationTracking);
    spdlog::info("[VR] OpenXR system position: {}", system_properties.trackingProperties.positionTracking);

    // Step 6: Get the view configuration properties
    uint32_t view_count{};
    result = xrEnumerateViewConfigurationViews(m_openxr.instance, m_openxr.system, m_openxr.view_config, 0, &view_count, nullptr);
    if (result != XR_SUCCESS) {
        xrResultToString(m_openxr.instance, result, result_string.data());
        m_openxr.error = "Could not get view configuration properties: " + result_string;
        spdlog::error("[VR] {}", m_openxr.error.value());

        return std::nullopt;
    }

    m_openxr.view_configs.resize(view_count, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
    result = xrEnumerateViewConfigurationViews(m_openxr.instance, m_openxr.system, m_openxr.view_config, view_count, &view_count, m_openxr.view_configs.data());
    if (result != XR_SUCCESS) {
        xrResultToString(m_openxr.instance, result, result_string.data());
        m_openxr.error = "Could not get view configuration properties: " + result_string;
        spdlog::error("[VR] {}", m_openxr.error.value());

        return std::nullopt;
    }

    // Step 7: Create a view
    m_openxr.views.resize(view_count, {XR_TYPE_VIEW});

    // Step 8: Create a swapchain
    spdlog::info("[VR] Creating OpenXR swapchain");

    if (view_count == 0) {
        m_openxr.error = "No view configurations found";
        spdlog::error("[VR] {}", m_openxr.error.value());

        return std::nullopt;
    }

    if (g_framework->is_dx12()) {
        auto err = m_d3d12.openxr().create_swapchains();

        if (err) {
            m_openxr.error = err.value();
            spdlog::error("[VR] {}", m_openxr.error.value());

            return std::nullopt;
        }
    } else {
        auto err = m_d3d11.openxr().create_swapchains();

        if (err) {
            m_openxr.error = err.value();
            spdlog::error("[VR] {}", m_openxr.error.value());

            return std::nullopt;
        }
    }

    m_openxr.loaded = true;

    return std::nullopt;
}

std::optional<std::string> VR::hijack_resolution() {
    // We're going to hook via.SceneView.get_Size so we can
    // spoof the render target size to the HMD's resolution.
    auto get_size_func = sdk::find_native_method("via.SceneView", "get_Size");

    if (get_size_func == nullptr) {
        return "VR init failed: via.SceneView.get_Size function not found.";
    }

    spdlog::info("via.SceneView.get_Size: {:x}", (uintptr_t)get_size_func);

    // Pattern scan for the native function call
    auto ref = utility::scan((uintptr_t)get_size_func, 0x100, "49 8B C8 E8");

    if (!ref) {
        return "VR init failed: via.SceneView.get_Size native function not found. Pattern scan failed.";
    }

    auto native_func = utility::calculate_absolute(*ref + 4);

    // Hook the native function
    g_get_size_hook = std::make_unique<FunctionHook>(native_func, get_size_hook);

    if (!g_get_size_hook->create()) {
        return "VR init failed: via.SceneView.get_Size native function hook failed.";
    }

    return std::nullopt;
}

std::optional<std::string> VR::hijack_input() {
#if defined(RE2) || defined(RE3)
    // We're going to hook InputSystem.update so we can
    // override the analog stick values with the VR controller's
    auto func = sdk::find_native_method(game_namespace("InputSystem"), "update");

    if (func == nullptr) {
        return "VR init failed: InputSystem.update function not found.";
    }

    spdlog::info("InputSystem.update: {:x}", (uintptr_t)func);

    // Hook the native function
    g_input_hook = std::make_unique<FunctionHook>(func, inputsystem_update_hook);

    if (!g_input_hook->create()) {
        return "VR init failed: InputSystem.update native function hook failed.";
    }
#endif

    return std::nullopt;
}

std::optional<std::string> VR::hijack_camera() {
    // We're going to hook via.Camera.get_ProjectionMatrix so we can
    // override the camera's Projection matrix with the HMD's Projection matrix (per-eye)
    auto func = sdk::find_native_method("via.Camera", "get_ProjectionMatrix");

    if (func == nullptr) {
        return "VR init failed: via.Camera.get_ProjectionMatrix function not found.";
    }

    spdlog::info("via.Camera.get_ProjectionMatrix: {:x}", (uintptr_t)func);
    
    // Pattern scan for the native function call
    auto ref = utility::scan((uintptr_t)func, 0x100, "49 8B C8 E8");

    if (!ref) {
        return "VR init failed: via.Camera.get_ProjectionMatrix native function not found. Pattern scan failed.";
    }

    auto native_func = utility::calculate_absolute(*ref + 4);

    // Hook the native function
    g_projection_matrix_hook = std::make_unique<FunctionHook>(native_func, camera_get_projection_matrix_hook);

    if (!g_projection_matrix_hook->create()) {
        return "VR init failed: via.Camera.get_ProjectionMatrix native function hook failed.";
    }

    spdlog::info("Hooked via.Camera.get_ProjectionMatrix");

    const auto get_projection_matrix = native_func;

    ///////////////////////////////
    // Hook GUI camera projection matrix start
    ///////////////////////////////
    func = sdk::find_native_method("via.gui.GUICamera", "get_ProjectionMatrix");

    if (func != nullptr) {
        spdlog::info("via.gui.GUICamera.get_ProjectionMatrix: {:x}", (uintptr_t)func);
        
        // Pattern scan for the native function call
        ref = utility::scan((uintptr_t)func, 0x100, "49 8B C8 E8");

        if (ref) {
            native_func = utility::calculate_absolute(*ref + 4);

            if (native_func != get_projection_matrix) {
                // Hook the native function
                g_projection_matrix_hook2 = std::make_unique<FunctionHook>(native_func, gui_camera_get_projection_matrix_hook);

                if (g_projection_matrix_hook2->create()) {
                    spdlog::info("Hooked via.gui.GUICamera.get_ProjectionMatrix");
                }
            } else {
                spdlog::info("Did not hook via.gui.GUICamera.get_ProjectionMatrix, same as via.Camera.get_ProjectionMatrix");
            }
        }
    }

    ///////////////////////////////
    // Hook view matrix start
    ///////////////////////////////
    func = sdk::find_native_method("via.Camera", "get_ViewMatrix");

    if (func == nullptr) {
        return "VR init failed: via.Camera.get_ViewMatrix function not found.";
    }

    spdlog::info("via.Camera.get_ViewMatrix: {:x}", (uintptr_t)func);

    // Pattern scan for the native function call
    ref = utility::scan((uintptr_t)func, 0x100, "49 8B C8 E8");

    if (!ref) {
        return "VR init failed: via.Camera.get_ViewMatrix native function not found. Pattern scan failed.";
    }

    native_func = utility::calculate_absolute(*ref + 4);

    // Hook the native function
    g_view_matrix_hook = std::make_unique<FunctionHook>(native_func, camera_get_view_matrix_hook);

    if (!g_view_matrix_hook->create()) {
        return "VR init failed: via.Camera.get_ViewMatrix native function hook failed.";
    }

    return std::nullopt;
}

std::optional<std::string> VR::hijack_render_layer(VR::RenderLayerHook<sdk::renderer::RenderLayer>& hook) {
    // We're going to make via.render.layer.Overlay.Draw() return early
    // For some reason this fixes 3D GUI rendering in RE3 in VR
    auto t = sdk::find_type_definition(hook.name);

    if (t == nullptr) {
        return std::string{"VR init failed: "} + hook.name + " type not found.";
    }

    void* fake_obj = t->create_instance();

    if (fake_obj == nullptr) { 
        return std::string{"VR init failed: "} + "Failed to create fake " + hook.name + " instance.";
    }

    auto obj_vtable = *(uintptr_t**)fake_obj;

    if (obj_vtable == nullptr) {
        return std::string{"VR init failed: "} + hook.name + " vtable not found.";
    }

    spdlog::info("{:s} vtable: {:x}", hook.name, (uintptr_t)obj_vtable - g_framework->get_module());

    auto draw_native = obj_vtable[sdk::renderer::RenderLayer::DRAW_VTABLE_INDEX];

    if (draw_native == 0) {
        return std::string{"VR init failed: "} + hook.name + " draw native not found.";
    }

    spdlog::info("{:s}.Draw: {:x}", hook.name, (uintptr_t)draw_native);

    // Set the first byte to the ret instruction
    //m_overlay_draw_patch = Patch::create(draw_native, { 0xC3 });

    if (!utility::is_stub_code((uint8_t*)draw_native)) {
        if (!hook.hook_draw(draw_native)) {
            return std::string{"VR init failed: "} + hook.name + " draw native function hook failed.";
        }
    } else {
        spdlog::info("Skipping draw hook for {:s}, stub code detected", hook.name);
    }

    auto update_native = obj_vtable[sdk::renderer::RenderLayer::UPDATE_VTABLE_INDEX];

    if (update_native == 0) {
        return std::string{"VR init failed: "} + hook.name + " update native not found.";
    }

    spdlog::info("{:s}.Update: {:x}", hook.name, (uintptr_t)update_native);

    if (!utility::is_stub_code((uint8_t*)update_native)) {
        if (!hook.hook_update(update_native)) {
            return std::string{"VR init failed: "} + hook.name + " update native function hook failed.";
        }
    } else {
        spdlog::info("Skipping update hook for {:s}, stub code detected", hook.name);
    }

    // Hook get_Sharpness
    /*auto get_sharpness = sdk::find_native_method("via.render.ToneMapping", "get_Sharpness");

    if (get_sharpness == nullptr) {
        return "Could not find get_Sharpness";
    }

    spdlog::info("via.render.ToneMapping.get_Sharpness: {:x}", (uintptr_t)get_sharpness);

    // Scan for the native function call (jmp)
    auto ref = utility::scan((uintptr_t)get_sharpness, 0x20, "E9");

    if (!ref) {
        return "VR init failed: via.render.ToneMapping.get_Sharpness native function not found. Pattern scan failed.";
    }

    auto native_func = utility::calculate_absolute(*ref + 1);

    g_get_sharpness_hook = std::make_unique<FunctionHook>(native_func, get_sharpness_hook);

    if (!g_get_sharpness_hook->create()) {
        return "VR init failed: via.render.ToneMapping.get_Sharpness native function hook failed.";
    }*/

    // ASDAFASFF
    /*t = sdk::find_type_definition("via.render.layer.PostShadowCast");

    if (t == nullptr) {
        return "VR init failed: via.render.layer.PostShadowCast type not found.";
    }

    fake_obj = t->create_instance();

    if (fake_obj == nullptr) {
        return "VR init failed: Failed to create fake via.render.layer.PostShadowCast instance.";
    }

    obj_vtable = *(uintptr_t**)fake_obj;

    if (obj_vtable == nullptr) {
        return "VR init failed: via.render.layer.PostShadowCast vtable not found.";
    }

    draw_native = obj_vtable[LAYER_DRAW_INDEX];

    if (draw_native == 0) {
        return "VR init failed: via.render.layer.PostShadowCast draw native not found.";
    }

    spdlog::info("via.render.layer.PostShadowCast.Draw: {:x}", (uintptr_t)draw_native);

    // Set the first byte to the ret instruction
    static auto shadow_patch = Patch::create(draw_native, { 0xC3 });*/

    return std::nullopt;
}

std::optional<std::string> VR::hijack_wwise_listeners() {
    const auto t = sdk::find_type_definition("via.wwise.WwiseListener");

    if (t == nullptr) {
        return "VR init failed: via.wwise.WwiseListener type not found.";
    }

    const auto update_method = t->get_method("update");

    if (update_method == nullptr) {
        return "VR init failed: via.wwise.WwiseListener.update method not found.";
    }

    const auto func_wrapper = update_method->get_function();

    if (func_wrapper == nullptr) {
        return "VR init failed: via.wwise.WwiseListener.update native function not found.";
    }
    
    spdlog::info("via.wwise.WwiseListener.update: {:x}", (uintptr_t)func_wrapper);
    
    // Use hde to disassemble the method and find the first jmp, which jmps to the real function
    // in the vtable
    const auto jmp = utility::scan_disasm((uintptr_t)func_wrapper, 10, "48 FF");

    if (!jmp) {
        return "VR init failed: could not find jmp opcode in via.wwise.WwiseListener.update native function.";
    }

    const auto vtable_index = *(uint8_t*)(*jmp + 3) / sizeof(void*);
    spdlog::info("via.wwise.WwiseListener.update vtable index: {}", vtable_index);

    const void* fake_obj = t->create_instance_full();

    if (fake_obj == nullptr) {
        return "VR init failed: Failed to create fake via.wwise.WwiseListener instance.";
    }

    auto obj_vtable = *(void***)fake_obj;

    if (obj_vtable == nullptr) {
        return "VR init failed: via.wwise.WwiseListener vtable not found.";
    }

    spdlog::info("via.wwise.WwiseListener vtable: {:x}", (uintptr_t)obj_vtable - g_framework->get_module());

    auto update_native = obj_vtable[vtable_index];

    if (update_native == 0) {
        return "VR init failed: via.wwise.WwiseListener update native not found.";
    }

    spdlog::info("via.wwise.WwiseListener.update: {:x}", (uintptr_t)update_native);

    g_wwise_listener_update_hook = std::make_unique<FunctionHook>(update_native, wwise_listener_update_hook);

    if (!g_wwise_listener_update_hook->create()) {
        return "VR init failed: via.wwise.WwiseListener update native function hook failed.";
    }

    return std::nullopt;
}

bool VR::detect_controllers() {
    // already detected
    if (!m_controllers.empty()) {
        return true;
    }

    auto left_joystick_origin_error = vr::EVRInputError::VRInputError_None;
    auto right_joystick_origin_error = vr::EVRInputError::VRInputError_None;

    vr::InputOriginInfo_t left_joystick_origin_info{};
    vr::InputOriginInfo_t right_joystick_origin_info{};

    // Get input origin info for the joysticks
    // get the source input device handles for the joysticks
    auto left_joystick_error = vr::VRInput()->GetInputSourceHandle("/user/hand/left", &m_left_joystick);

    if (left_joystick_error != vr::VRInputError_None) {
        return false;
    }

    auto right_joystick_error = vr::VRInput()->GetInputSourceHandle("/user/hand/right", &m_right_joystick);

    if (right_joystick_error != vr::VRInputError_None) {
        return false;
    }

    left_joystick_origin_info = {};
    right_joystick_origin_info = {};

    left_joystick_origin_error = vr::VRInput()->GetOriginTrackedDeviceInfo(m_left_joystick, &left_joystick_origin_info, sizeof(left_joystick_origin_info));
    right_joystick_origin_error = vr::VRInput()->GetOriginTrackedDeviceInfo(m_right_joystick, &right_joystick_origin_info, sizeof(right_joystick_origin_info));
    if (left_joystick_origin_error != vr::EVRInputError::VRInputError_None || right_joystick_origin_error != vr::EVRInputError::VRInputError_None) {
        return false;
    }

    // Instead of manually going through the devices,
    // We do this. The order of the devices isn't always guaranteed to be
    // Left, and then right. Using the input state handles will always
    // Get us the correct device indices.
    m_controllers.push_back(left_joystick_origin_info.trackedDeviceIndex);
    m_controllers.push_back(right_joystick_origin_info.trackedDeviceIndex);
    m_controllers_set.insert(left_joystick_origin_info.trackedDeviceIndex);
    m_controllers_set.insert(right_joystick_origin_info.trackedDeviceIndex);

    spdlog::info("Left Hand: {}", left_joystick_origin_info.trackedDeviceIndex);
    spdlog::info("Right Hand: {}", right_joystick_origin_info.trackedDeviceIndex);

    return true;
}

bool VR::is_any_action_down() {
    if (!m_is_hmd_active || !m_openvr_loaded || !is_using_controllers()) {
        return false;
    }

    const auto left_axis = get_left_stick_axis();
    const auto right_axis = get_right_stick_axis();

    if (glm::length(left_axis) >= m_joystick_deadzone->value()) {
        return true;
    }

    if (glm::length(right_axis) >= m_joystick_deadzone->value()) {
        return true;
    }

    for (auto& it : m_action_handles) {
        if (is_action_active(it.second, m_left_joystick) || is_action_active(it.second, m_right_joystick)) {
            return true;
        }
    }

    return false;
}

void VR::update_hmd_state() {
    //update_action_states();

    vr::VRCompositor()->SetTrackingSpace(vr::TrackingUniverseStanding);
    vr::VRCompositor()->WaitGetPoses(m_real_render_poses.data(), vr::k_unMaxTrackedDeviceCount, m_real_game_poses.data(), vr::k_unMaxTrackedDeviceCount);

    bool wants_reset_origin = false;

    // Process events
    vr::VREvent_t event{};
    while (m_hmd->PollNextEvent(&event, sizeof(event))) {
        switch ((vr::EVREventType)event.eventType) {
            // Detect whether video settings changed
            case vr::VREvent_SteamVRSectionSettingChanged: {
                spdlog::info("VR: VREvent_SteamVRSectionSettingChanged");
                m_hmd->GetRecommendedRenderTargetSize(&m_w, &m_h);
            } break;

            // Detect whether SteamVR reset the standing/seated pose
            case vr::VREvent_SeatedZeroPoseReset: [[fallthrough]];
            case vr::VREvent_StandingZeroPoseReset: {
                spdlog::info("VR: VREvent_SeatedZeroPoseReset");
                wants_reset_origin = true;
            } break;

            case vr::VREvent_DashboardActivated: {
                m_handle_pause = true;
            } break;

            default:
                spdlog::info("VR: Unknown event: {}", (uint32_t)event.eventType);
                break;
        }
    }

    // Update the poses used for the game
    // If we used the data directly from the WaitGetPoses call, we would have to lock a different mutex and wait a long time
    // This is because the WaitGetPoses call is blocking, and we don't want to block any game logic
    {
        std::unique_lock _{ m_pose_mtx };

        memcpy(m_game_poses.data(), m_real_game_poses.data(), sizeof(m_game_poses));
        memcpy(m_render_poses.data(), m_real_render_poses.data(), sizeof(m_render_poses));

        if (wants_reset_origin) {
            set_rotation_offset(glm::identity<glm::quat>());
            m_standing_origin = get_position_unsafe(vr::k_unTrackedDeviceIndex_Hmd);
        }
    }

    {
        std::unique_lock __{ m_eyes_mtx };
        const auto local_left = m_hmd->GetEyeToHeadTransform(vr::Eye_Left);
        const auto local_right = m_hmd->GetEyeToHeadTransform(vr::Eye_Right);

        m_eyes[vr::Eye_Left] = glm::rowMajor4(Matrix4x4f{ *(Matrix3x4f*)&local_left } );
        m_eyes[vr::Eye_Right] = glm::rowMajor4(Matrix4x4f{ *(Matrix3x4f*)&local_right } );

        auto pleft = m_hmd->GetProjectionMatrix(vr::Eye_Left, m_nearz, m_farz);
        auto pright = m_hmd->GetProjectionMatrix(vr::Eye_Right, m_nearz, m_farz);

        m_projections[vr::Eye_Left] = glm::rowMajor4(Matrix4x4f{ *(Matrix4x4f*)&pleft } );
        m_projections[vr::Eye_Right] = glm::rowMajor4(Matrix4x4f{ *(Matrix4x4f*)&pright } );

        m_hmd->GetProjectionRaw(vr::Eye_Left, &m_raw_projections[vr::Eye_Left][0], &m_raw_projections[vr::Eye_Left][1], &m_raw_projections[vr::Eye_Left][2], &m_raw_projections[vr::Eye_Left][3]);
        m_hmd->GetProjectionRaw(vr::Eye_Right, &m_raw_projections[vr::Eye_Right][0], &m_raw_projections[vr::Eye_Right][1], &m_raw_projections[vr::Eye_Right][2], &m_raw_projections[vr::Eye_Right][3]);
    }

    // On first run, set the standing origin to the headset position
    if (!m_wgp_initialized) {
        m_standing_origin = get_position(vr::k_unTrackedDeviceIndex_Hmd);
    }

    m_wgp_initialized = true;

    // Forcefully update the camera transform after submitting the frame
    // because the game logic thread does not run in sync with the rendering thread
    // This will massively improve HMD rotation smoothness for the user
    // if this is not done, the left eye will jitter a lot
#if defined(RE2) || defined(RE3)
    auto camera = sdk::get_primary_camera();

    if (camera != nullptr && camera->ownerGameObject != nullptr && camera->ownerGameObject->transform != nullptr) {
        FirstPerson::get()->on_update_transform(camera->ownerGameObject->transform);
    }
#endif
}

void VR::update_action_states() {
    if (m_request_reinitialize_openvr) {
        return;
    }

    const auto start_time = std::chrono::high_resolution_clock::now();

    auto error = vr::VRInput()->UpdateActionState(&m_active_action_set, sizeof(m_active_action_set), 1);

    if (error != vr::VRInputError_None) {
        spdlog::error("VRInput failed to update action state: {}", (uint32_t)error);
    }

    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto time_delta = end_time - start_time;

    m_last_input_delay = time_delta;
    m_avg_input_delay = (m_avg_input_delay + time_delta) / 2;

    if ((end_time - start_time) >= std::chrono::milliseconds(30)) {
        spdlog::warn("VRInput update action state took too long: {}ms", std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count());

        //reinitialize_openvr();
        m_request_reinitialize_openvr = true;
    }

    if (m_recenter_view_key->is_key_down_once()) {
        recenter_view();
    }

    if (m_set_standing_key->is_key_down_once()) {
        set_standing_origin(get_position(0));
    }
}

void VR::update_camera() {
    if (!m_is_hmd_active) {
        m_needs_camera_restore = false;
        return;
    }

    if (inside_on_end) {
        return;
    }

    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    static auto via_camera = sdk::find_type_definition("via.Camera");
    static auto get_near_clip_plane_method = via_camera->get_method("get_NearClipPlane");
    static auto get_far_clip_plane_method = via_camera->get_method("get_FarClipPlane");
    static auto set_far_clip_plane_method = via_camera->get_method("set_FarClipPlane");
    static auto set_fov_method = via_camera->get_method("set_FOV");
    static auto set_vertical_enable_method = via_camera->get_method("set_VerticalEnable");
    static auto set_aspect_ratio_method = via_camera->get_method("set_AspectRatio");

    if (m_use_custom_view_distance->value()) {
        set_far_clip_plane_method->call<void*>(sdk::get_thread_context(), camera, m_view_distance->value());
    }

    m_nearz = get_near_clip_plane_method->call<float>(sdk::get_thread_context(), camera);
    m_farz = get_far_clip_plane_method->call<float>(sdk::get_thread_context(), camera);

    // Disable certain effects like the 3D overlay during the sewer gators
    // section in RE7
    disable_bad_effects();
    // Disable lens distortion
    set_lens_distortion(false);

#if defined(RE2) || defined(RE3)
    if (FirstPerson::get()->will_be_used()) {
        m_needs_camera_restore = false;

        auto camera_object = utility::re_component::get_game_object(camera);

        if (camera_object == nullptr || camera_object->transform == nullptr) {
            return;
        }

        auto camera_joint = utility::re_transform::get_joint(*camera_object->transform, 0);

        if (camera_joint == nullptr) {
            return;
        }

        m_original_camera_position = sdk::get_joint_position(camera_joint);
        m_original_camera_rotation = sdk::get_joint_rotation(camera_joint);
        m_original_camera_matrix = Matrix4x4f{m_original_camera_rotation};
        m_original_camera_matrix[3] = m_original_camera_position;

        return;
    }
#endif

    update_camera_origin();

    auto projection_matrix = get_current_projection_matrix(true);

    // Steps towards getting lens flares and volumetric lighting working
    // Get the FOV from the projection matrix
    const auto vfov = glm::degrees(2.0f * std::atan(1.0f / projection_matrix[1][1]));
    const auto aspect = projection_matrix[1][1] / projection_matrix[0][0];
    const auto hfov = vfov * aspect;
    
    //spdlog::info("vFOV: {}", vfov);
    //spdlog::info("Aspect: {}", aspect);

    set_fov_method->call<void*>(sdk::get_thread_context(), camera, vfov);
    set_vertical_enable_method->call<void*>(sdk::get_thread_context(), camera, true);
    set_aspect_ratio_method->call<void*>(sdk::get_thread_context(), camera, aspect);

    m_needs_camera_restore = true;
}

void VR::update_camera_origin() {
    if (!m_is_hmd_active) {
        return;
    }

    // this means that the user just put the headset on or something
    // and we caught them in a bad frame
    if (inside_on_end && !m_needs_camera_restore) {
        return;
    }

    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        m_needs_camera_restore = false;
        return;
    }

    auto camera_object = utility::re_component::get_game_object(camera);

    if (camera_object == nullptr || camera_object->transform == nullptr) {
        m_needs_camera_restore = false;
        return;
    }

    auto camera_joint = utility::re_transform::get_joint(*camera_object->transform, 0);

    if (camera_joint == nullptr) {
        m_needs_camera_restore = false;
        return;
    }

    if (!inside_on_end) {
        m_original_camera_position = sdk::get_joint_position(camera_joint);
        m_original_camera_rotation = sdk::get_joint_rotation(camera_joint);
        m_original_camera_matrix = Matrix4x4f{m_original_camera_rotation};
        m_original_camera_matrix[3] = m_original_camera_position;
    }

    apply_hmd_transform(camera_joint);
}

void VR::apply_hmd_transform(glm::quat& rotation, Vector4f& position) {
    const auto rotation_offset = get_rotation_offset();
    const auto current_hmd_rotation = glm::normalize(rotation_offset * glm::quat{get_rotation(0)});
    
    glm::quat new_rotation{};
    glm::quat camera_rotation{};
    
    if (!m_decoupled_pitch->value()) {
        camera_rotation = rotation;
        new_rotation = glm::normalize(rotation * current_hmd_rotation);
    } else if (m_decoupled_pitch->value()) {
        // facing forward matrix
        const auto camera_rotation_matrix = utility::math::remove_y_component(Matrix4x4f{rotation});
        camera_rotation = glm::quat{camera_rotation_matrix};
        new_rotation = glm::normalize(camera_rotation * current_hmd_rotation);
    }

    auto current_relative_pos = rotation_offset * (get_position(0) - m_standing_origin) /*+ current_relative_eye_pos*/;
    current_relative_pos.w = 0.0f;

    auto current_head_pos = camera_rotation * current_relative_pos;

    rotation = new_rotation;
    position = position + current_head_pos;
}

void VR::apply_hmd_transform(::REJoint* camera_joint) {
    auto rotation = m_original_camera_rotation;
    auto position = m_original_camera_position;

    apply_hmd_transform(rotation, position);

    sdk::set_joint_rotation(camera_joint, rotation);

    if (m_positional_tracking) {
        sdk::set_joint_position(camera_joint, position);   
    }
}

void VR::update_audio_camera() {
    if (!m_is_hmd_active) {
        return;
    }

    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    auto camera_object = utility::re_component::get_game_object(camera);

    if (camera_object == nullptr || camera_object->transform == nullptr) {
        return;
    }

    auto camera_joint = utility::re_transform::get_joint(*camera_object->transform, 0);

    if (camera_joint == nullptr) {
        return;
    }

    m_original_audio_camera_position = sdk::get_joint_position(camera_joint);
    m_original_audio_camera_rotation = sdk::get_joint_rotation(camera_joint);
    m_needs_audio_restore = true;

    auto rotation = m_original_audio_camera_rotation;
    auto position = m_original_audio_camera_position;

    apply_hmd_transform(rotation, position);

    sdk::set_joint_rotation(camera_joint, rotation);

    if (m_positional_tracking) {
        sdk::set_joint_position(camera_joint, position);   
    }
}

void VR::update_render_matrix() {
    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    auto camera_object = utility::re_component::get_game_object(camera);

    if (camera_object == nullptr || camera_object->transform == nullptr) {
        return;
    }

    auto camera_joint = utility::re_transform::get_joint(*camera_object->transform, 0);

    if (camera_joint == nullptr) {
        return;
    }

    m_render_camera_matrix = Matrix4x4f{sdk::get_joint_rotation(camera_joint)};
    m_render_camera_matrix[3] = sdk::get_joint_position(camera_joint);
}

void VR::restore_audio_camera() {
    if (!m_needs_audio_restore) {
        return;
    }

#if defined(RE2) || defined(RE3)
    if (FirstPerson::get()->will_be_used()) {
        m_needs_audio_restore = false;
        return;
    }
#endif

    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        m_needs_audio_restore = false;
        return;
    }

    auto camera_object = utility::re_component::get_game_object(camera);

    if (camera_object == nullptr || camera_object->transform == nullptr) {
        m_needs_audio_restore = false;
        return;
    }

    //camera_object->transform->worldTransform = m_original_camera_matrix;

    auto joint = utility::re_transform::get_joint(*camera_object->transform, 0);

    if (joint == nullptr) {
        m_needs_audio_restore = false;
        return;
    }

    sdk::set_joint_rotation(joint, m_original_audio_camera_rotation);
    sdk::set_joint_position(joint, m_original_audio_camera_position);

    m_needs_audio_restore = false;
}

void VR::restore_camera() {
    if (!m_needs_camera_restore) {
        return;
    }

#if defined(RE2) || defined(RE3)
    if (FirstPerson::get()->will_be_used()) {
        m_needs_camera_restore = false;
        return;
    }
#endif

    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        m_needs_camera_restore = false;
        return;
    }

    auto camera_object = utility::re_component::get_game_object(camera);

    if (camera_object == nullptr || camera_object->transform == nullptr) {
        m_needs_camera_restore = false;
        return;
    }

    //camera_object->transform->worldTransform = m_original_camera_matrix;

    auto joint = utility::re_transform::get_joint(*camera_object->transform, 0);

    if (joint == nullptr) {
        m_needs_camera_restore = false;
        return;
    }

    sdk::set_joint_rotation(joint, m_original_camera_rotation);
    sdk::set_joint_position(joint, m_original_camera_position);
    m_needs_camera_restore = false;
}

void VR::set_lens_distortion(bool value) {
    if (!m_force_lensdistortion_settings->value()) {
        return;
    }

#ifdef RE7
    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    static auto lens_distortion_tdef = sdk::find_type_definition(game_namespace("LensDistortionController"));
    static auto lens_distortion_t = lens_distortion_tdef->get_type();

    auto lens_distortion_component = utility::re_component::find(camera, lens_distortion_t);

    if (lens_distortion_component != nullptr) {
        // Get "LensDistortion" field
        auto lens_distortion = *sdk::get_object_field<REManagedObject*>(lens_distortion_component, "LensDistortion");

        if (lens_distortion != nullptr) {
            // Call "set_Enabled" method
            sdk::call_object_func<void*>(lens_distortion, "set_Enabled", sdk::get_thread_context(), lens_distortion, value);
        }
    }
#endif
}

void VR::disable_bad_effects() {
    auto context = sdk::get_thread_context();

    auto application = sdk::Application::get();

    // minor optimizations to prevent hashing and map lookups every frame
    static auto renderer_t = sdk::find_type_definition("via.render.Renderer");

    static auto get_render_config_method = renderer_t->get_method("get_RenderConfig");

    static auto render_config_t = sdk::find_type_definition("via.render.RenderConfig");

    static auto get_framerate_setting_method = render_config_t->get_method("get_FramerateSetting");
    static auto set_framerate_setting_method = render_config_t->get_method("set_FramerateSetting");

    static auto get_antialiasing_method = render_config_t->get_method("get_AntiAliasing");
    static auto set_antialiasing_method = render_config_t->get_method("set_AntiAliasing");

    static auto get_lens_distortion_setting_method = render_config_t->get_method("getLensDistortionSetting");
    static auto set_lens_distortion_setting_method = render_config_t->get_method("setLensDistortionSetting");

    static auto get_motion_blur_enable_method = render_config_t->get_method("get_MotionBlurEnable");
    static auto set_motion_blur_enable_method = render_config_t->get_method("set_MotionBlurEnable");

    static auto get_vsync_method = render_config_t->get_method("get_VSync");
    static auto set_vsync_method = render_config_t->get_method("set_VSync");

    static auto get_transparent_buffer_quality_method = render_config_t->get_method("get_TransparentBufferQuality");
    static auto set_transparent_buffer_quality_method = render_config_t->get_method("set_TransparentBufferQuality");

    static auto get_lensflare_enable_method = render_config_t->get_method("get_LensFlareEnable");
    static auto set_lensflare_enable_method = render_config_t->get_method("set_LensFlareEnable");

    static auto get_colorspace_method = render_config_t->get_method("get_ColorSpace");
    static auto set_colorspace_method = render_config_t->get_method("set_ColorSpace");

    static auto get_dynamic_shadow_enable_method = render_config_t->get_method("get_DynamicShadowEnable");
    static auto set_dynamic_shadow_enable_method = render_config_t->get_method("set_DynamicShadowEnable");

    static auto get_hdrmode_method = renderer_t->get_method("get_HDRMode");
    static auto set_hdrmode_method = renderer_t->get_method("set_HDRMode");

    static auto get_hdr_display_mode_enable_method = renderer_t->get_method("get_HDRDisplayModeEnable");
    static auto set_hdr_display_mode_enable_method = renderer_t->get_method("set_HDRDisplayModeEnable");

    auto renderer = renderer_t->get_instance();

    auto render_config = get_render_config_method->call<::REManagedObject*>(context, renderer);

    if (render_config == nullptr) {
        spdlog::info("No render config!");
        return;
    }

    if (m_force_fps_settings->value() && get_framerate_setting_method != nullptr && set_framerate_setting_method != nullptr) {
        const auto framerate_setting = get_framerate_setting_method->call<via::render::RenderConfig::FramerateType>(context, render_config);

        // Allow FPS to go above 60
        if (framerate_setting != via::render::RenderConfig::FramerateType::VARIABLE) {
            set_framerate_setting_method->call<void*>(context, render_config, via::render::RenderConfig::FramerateType::VARIABLE);
            spdlog::info("[VR] Set framerate to variable");
        }
    }
    
    // get_MaxFps on application
    if (m_force_fps_settings->value() && application->get_max_fps() < 600.0f) {
        application->set_max_fps(600.0f);
        spdlog::info("[VR] Max FPS set to 600");
    }

    if (m_force_aa_settings->value() && get_antialiasing_method != nullptr && set_antialiasing_method != nullptr) {
        const auto antialiasing = get_antialiasing_method->call<via::render::RenderConfig::AntiAliasingType>(context, render_config);

        // Disable TAA
        switch (antialiasing) {
            case via::render::RenderConfig::AntiAliasingType::TAA: [[fallthrough]];
            case via::render::RenderConfig::AntiAliasingType::FXAA_TAA:
                set_antialiasing_method->call<void*>(context, render_config, via::render::RenderConfig::AntiAliasingType::NONE);
                spdlog::info("[VR] TAA disabled");
                break;
            default:
                break;
        }
    }

    if (m_force_lensdistortion_settings->value() && get_lens_distortion_setting_method != nullptr && set_lens_distortion_setting_method != nullptr) {
        const auto lens_distortion_setting = get_lens_distortion_setting_method->call<via::render::RenderConfig::LensDistortionSetting>(context, render_config);

        // Disable lens distortion
        if (lens_distortion_setting != via::render::RenderConfig::LensDistortionSetting::OFF) {
            set_lens_distortion_setting_method->call<void*>(context, render_config, via::render::RenderConfig::LensDistortionSetting::OFF);
            spdlog::info("[VR] Lens distortion disabled");
        }
    }

    if (m_force_motionblur_settings->value() && get_motion_blur_enable_method != nullptr && set_motion_blur_enable_method != nullptr) {
        const auto is_motion_blur_enabled = get_motion_blur_enable_method->call<bool>(context, render_config);

        // Disable motion blur
        if (is_motion_blur_enabled) {
            set_motion_blur_enable_method->call<void*>(context, render_config, false);
            spdlog::info("[VR] Motion blur disabled");
        }
    }

    if (m_force_vsync_settings->value() && get_vsync_method != nullptr && set_vsync_method != nullptr) {
#ifndef MHRISE
        const auto vsync = get_vsync_method->call<bool>(context, render_config);

        // Disable vsync
        if (vsync) {
            set_vsync_method->call<void*>(context, render_config, false);
            spdlog::info("[VR] VSync disabled");
        }
#else
        // We are only calling set_vsync instead of checking with get_vsync in MHRise
        // because get_VSync has some insane code protection on it for some reason
        // which would increase frametimes to 15+
        set_vsync_method->call<void*>(context, render_config, false);
#endif
    }

    if (m_force_volumetrics_settings->value() && get_transparent_buffer_quality_method != nullptr && set_transparent_buffer_quality_method != nullptr) {
        const auto transparent_buffer_quality = get_transparent_buffer_quality_method->call<via::render::RenderConfig::Quality>(context, render_config);

        // Disable volumetrics
        if (transparent_buffer_quality != via::render::RenderConfig::Quality::NONE) {
            set_transparent_buffer_quality_method->call<void*>(context, render_config, via::render::RenderConfig::Quality::NONE);
            spdlog::info("[VR] Volumetrics disabled");
        }
    }

    if (m_force_lensflares_settings->value() && get_lensflare_enable_method != nullptr && set_lensflare_enable_method != nullptr) {
        const auto is_lensflare_enabled = get_lensflare_enable_method->call<bool>(context, render_config);

        // Disable lensflares
        if (is_lensflare_enabled) {
            set_lensflare_enable_method->call<void*>(context, render_config, false);
            spdlog::info("[VR] Lensflares disabled");
        }
    }

    if (get_hdrmode_method != nullptr && set_hdrmode_method != nullptr) {
        // static
        const auto is_hdr_enabled = get_hdrmode_method->call<bool>(context);

        // Disable HDR
        if (is_hdr_enabled) {
            set_hdrmode_method->call<void*>(context, false);
            
            if (set_hdr_display_mode_enable_method != nullptr) {
                set_hdr_display_mode_enable_method->call<void*>(context, false);
            }

            spdlog::info("[VR] HDR disabled");
        }
    }

    if (get_colorspace_method != nullptr && set_colorspace_method != nullptr) {
        const auto is_hdr_enabled = get_colorspace_method->call<via::render::ColorSpace>(context, render_config) == via::render::ColorSpace::HDR10;

        if (is_hdr_enabled) {
            set_colorspace_method->call<void*>(context, render_config, via::render::ColorSpace::HDTV);
            spdlog::info("[VR] HDR disabled (ColorSpace)");
        }
    }

    if (m_force_dynamic_shadows_settings->value() && get_dynamic_shadow_enable_method != nullptr && set_dynamic_shadow_enable_method != nullptr) {
        const auto is_dynamic_shadow_enabled = get_dynamic_shadow_enable_method->call<bool>(context, render_config);

        // Enable dynamic shadows
        if (!is_dynamic_shadow_enabled) {
            set_dynamic_shadow_enable_method->call<void*>(context, render_config, true);
            spdlog::info("[VR] Dynamic shadows enabled");
        }
    }

#ifdef RE7
    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    auto camera_game_object = utility::re_component::get_game_object(camera);

    if (camera_game_object == nullptr) {
        return;
    }
    
    auto camera_transform = camera_game_object->transform;

    if (camera_transform == nullptr) {
        return;
    }

    auto get_type = [](std::string name) {
        auto tdef = sdk::find_type_definition(name);
        return tdef->get_type();
    };

    static auto effect_player_t = get_type("via.effect.EffectPlayer");
    static auto hide_effect_for_vr_t = get_type(game_namespace("EPVStandard.HideEffectForVR"));

    // Do not draw the game object if it is hidden for VR (this was there for the original PSVR release i guess)
    for (auto child = camera_transform->child; child != nullptr; child = child->child) {
        if (utility::re_component::find(child, effect_player_t) != nullptr) {
            if (utility::re_component::find(child, hide_effect_for_vr_t) != nullptr) {
                auto game_object = child->ownerGameObject;

                game_object->shouldDraw = false;
                //sdk::call_object_func<void*>(game_object, "set_Draw", sdk::get_thread_context(), game_object, false);
            }
        }
    }
#endif
}

void VR::fix_temporal_effects() {
    // this is SO DUMB!!!!!
    const auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }
    
    static auto render_output_type = sdk::find_type_definition("via.render.RenderOutput")->get_type();
    auto render_output_component = utility::re_component::find(camera, render_output_type);

    if (render_output_component == nullptr) {
        return;
    }

    if (!m_is_hmd_active || m_disable_temporal_fix) {
        sdk::call_object_func_easy<void*>(render_output_component, "set_DistortionType", 0); // None
        return;
    }

    if (m_frame_count % 2 == m_left_eye_interval) {
        sdk::call_object_func_easy<void*>(render_output_component, "set_DistortionType", 1); // left
    } else {
        sdk::call_object_func_easy<void*>(render_output_component, "set_DistortionType", 2); // right
    }
}

int32_t VR::get_frame_count() const {
    return get_game_frame_count();
}

int32_t VR::get_game_frame_count() const {
    static auto renderer_type = sdk::find_type_definition("via.render.Renderer");

    if (renderer_type == nullptr) {
        renderer_type = sdk::find_type_definition("via.render.Renderer");
        spdlog::warn("VR: Failed to find renderer type, trying again next time");
        return 0;
    }

    auto renderer = renderer_type->get_instance();

    if (renderer == nullptr) {
        return 0;
    }

    static auto get_render_frame_method = renderer_type->get_method("get_RenderFrame");

    return get_render_frame_method->call<int32_t>(sdk::get_thread_context(), renderer);
}

float VR::get_standing_height() {
    std::shared_lock _{ m_pose_mtx };

    return m_standing_origin.y;
}

Vector4f VR::get_standing_origin() {
    std::shared_lock _{ m_pose_mtx };

    return m_standing_origin;
}

void VR::set_standing_origin(const Vector4f& origin) {
    std::unique_lock _{ m_pose_mtx };
    
    m_standing_origin = origin;
}

glm::quat VR::get_rotation_offset() {
    std::shared_lock _{ m_rotation_mtx };

    return m_rotation_offset;
}

void VR::set_rotation_offset(const glm::quat& offset) {
    std::unique_lock _{ m_rotation_mtx };

    m_rotation_offset = offset;
}

void VR::recenter_view() {
    const auto new_rotation_offset = glm::normalize(glm::inverse(utility::math::flatten(glm::quat{get_rotation(0)})));

    set_rotation_offset(new_rotation_offset);
}

glm::quat VR::get_gui_rotation_offset() {
    std::shared_lock _{ m_gui_mtx };

    return m_gui_rotation_offset;
}

void VR::set_gui_rotation_offset(const glm::quat& offset) {
    std::unique_lock _{ m_gui_mtx };

    m_gui_rotation_offset = offset;
}

void VR::recenter_gui(const glm::quat& from) {
    const auto new_gui_offset = glm::normalize(glm::inverse(utility::math::flatten(from)));
    set_gui_rotation_offset(new_gui_offset);
}

Vector4f VR::get_current_offset() {
    if (!m_is_hmd_active) {
        return Vector4f{};
    }

    std::shared_lock _{ m_eyes_mtx };

    if (m_frame_count % 2 == m_left_eye_interval) {
        //return Vector4f{m_eye_distance * -1.0f, 0.0f, 0.0f, 0.0f};
        return m_eyes[vr::Eye_Left][3];
    }
    
    return m_eyes[vr::Eye_Right][3];
    //return Vector4f{m_eye_distance, 0.0f, 0.0f, 0.0f};
}

Matrix4x4f VR::get_current_eye_transform(bool flip) {
    if (!m_is_hmd_active) {
        return glm::identity<Matrix4x4f>();
    }

    std::shared_lock _{m_eyes_mtx};

    auto mod_count = flip ? m_right_eye_interval : m_left_eye_interval;

    if (m_frame_count % 2 == mod_count) {
        return m_eyes[vr::Eye_Left];
    }

    return m_eyes[vr::Eye_Right];
}

Matrix4x4f VR::get_current_projection_matrix(bool flip) {
    if (!m_is_hmd_active) {
        return glm::identity<Matrix4x4f>();
    }

    std::shared_lock _{m_eyes_mtx};

    auto mod_count = flip ? m_right_eye_interval : m_left_eye_interval;

    if (m_frame_count % 2 == mod_count) {
        return m_projections[vr::Eye_Left];
    }

    return m_projections[vr::Eye_Right];
}

void VR::on_pre_imgui_frame() {
    if (!m_openvr_loaded || !m_is_hmd_active || !m_wgp_initialized) {
        return;
    }

    m_overlay_component.on_pre_imgui_frame();
}

void VR::on_present() {
    if (!m_openvr_loaded) {
        return;
    }

    if (m_wgp_initialized) {
        const auto hmd_activity = m_hmd->GetTrackedDeviceActivityLevel(vr::k_unTrackedDeviceIndex_Hmd);
        m_is_hmd_active = hmd_activity == vr::k_EDeviceActivityLevel_UserInteraction || hmd_activity == vr::k_EDeviceActivityLevel_UserInteraction_Timeout;

        // upon headset re-entry, reinitialize OpenVR
        if (m_is_hmd_active && !m_was_hmd_active) {
            m_request_reinitialize_openvr = true;
        }

        m_was_hmd_active = m_is_hmd_active;

        if (!m_is_hmd_active) {
            return;
        }
    } else {
        m_is_hmd_active = true; // We need to force out an initial WaitGetPoses call
        m_was_hmd_active = true;
    }

    if (m_frame_count == m_last_frame_count) {
        return;
    }

    m_submitted = false;

    const auto renderer = g_framework->get_renderer_type();

    // attempt to fix crash when reinitializing openvr
    std::scoped_lock _{m_openvr_mtx};

    vr::EVRCompositorError e = vr::EVRCompositorError::VRCompositorError_None;

    if (renderer == REFramework::RendererType::D3D11) {
        e = m_d3d11.on_frame(this);
    } else if (renderer == REFramework::RendererType::D3D12) {
        m_is_d3d12 = true;
        e = m_d3d12.on_frame(this);
    }

    // force a waitgetposes call to fix this...
    if (e == vr::EVRCompositorError::VRCompositorError_AlreadySubmitted) {
        m_wgp_initialized = false;
        m_needs_wgp_update = true;
    }
}

void VR::on_post_present() {
    if (!m_is_hmd_active || !m_openvr_loaded) {
        return;
    }

    // Unlock the m_present_finished conditional variable
    // Which will synchronize WaitGetPoses() properly
    // inside on_pre_wait_rendering()
    // we can't do it here because the game logic thread executes out of sync otherwise
    // causing jittery HMD rotation
    auto unlock_present = [&]() {
        {
            std::lock_guard _{m_present_finished_mtx};
            m_present_finished = true;
        }

        m_present_finished_cv.notify_all();
    };
    
    if (m_frame_count == m_last_frame_count) {
        return;
    }

    std::scoped_lock _{m_openvr_mtx};

    m_last_frame_count = m_frame_count;

    if (m_submitted || m_needs_wgp_update) {
        if (m_submitted) {
            m_overlay_component.on_post_compositor_submit();
            vr::VRCompositor()->PostPresentHandoff();
        }

        m_needs_wgp_update = true;
        m_submitted = false;
        unlock_present();
    } else { // always unlocks every frame so we don't cause a deadlock on AFR
        unlock_present();
    }
}

void VR::on_update_transform(RETransform* transform) {
    
}

void VR::on_update_camera_controller(RopewayPlayerCameraController* controller) {
    // get headset rotation
    /*const auto& headset_pose = m_game_poses[0];

    if (!headset_pose.bPoseIsValid) {
        return;
    }

    auto headset_matrix = Matrix4x4f{ *(Matrix3x4f*)&headset_pose.mDeviceToAbsoluteTracking };
    auto headset_rotation = glm::extractMatrixRotation(glm::rowMajor4(headset_matrix));

    headset_rotation *= get_current_rotation_offset();

    *(glm::quat*)&controller->worldRotation = glm::quat{ headset_rotation  };

    controller->worldPosition += get_current_offset();*/
}

struct GUIRestoreData {
    REComponent* element{nullptr};
    REComponent* view{nullptr};
    Vector4f original_position{ 0.0f, 0.0f, 0.0f, 1.0f };
    via::gui::ViewType view_type{ via::gui::ViewType::Screen };
    bool overlay{false};
    bool detonemap{true};
};

thread_local std::vector<std::unique_ptr<GUIRestoreData>> g_elements_to_reset{};

bool VR::on_pre_gui_draw_element(REComponent* gui_element, void* primitive_context) {
    inside_gui_draw = true;

    if (!m_is_hmd_active || !m_openvr_loaded) {
        return true;
    }

    auto game_object = utility::re_component::get_game_object(gui_element);

    if (game_object != nullptr && game_object->transform != nullptr) {
        auto context = sdk::get_thread_context();

        const auto name = utility::re_string::get_string(game_object->name);
        const auto name_hash = utility::hash(name);

        switch (name_hash) {
        // Don't mess with this, causes weird black boxes on the sides of the screen
        case "GUI_PillarBox"_fnv:
        // These allow the cutscene transitions to display (fade to black)
        case "BlackFade"_fnv:
        case "WhiteFade"_fnv:
        case "Fade_In_Out_Black"_fnv:
        case "Fade_In_Out_White"_fnv:
        case "FadeInOutBlack"_fnv:
        case "FadeInOutWhite"_fnv:
        case "GUIBlackMask"_fnv:
        case "GenomeCodexGUI"_fnv:
        case "sm42_020_keystrokeDevice01A_gimmick"_fnv: // this one is the keypad in the locker room...
            return true;

#if defined(RE2) || defined(RE3)
        // the weird buggy overlay in the inventory
        case "GuiBack"_fnv:
            return false;
#endif

        default:
            break;
        };

        // Certain UI elements we want to remove when in VR (FirstPerson enabled)
#if defined(RE2) || defined(RE3)
        auto& fp = FirstPerson::get();

        if (fp->is_enabled() && fp->will_be_used()) {
            const auto has_motion_controls = this->is_using_controllers();

            switch(name_hash) {
            case "GUI_Reticle"_fnv: // Crosshair
                if (has_motion_controls) {
                    return false;
                }
                
                break;
            default:
                break;
            }
        }
#endif

#ifdef RE7
        if (name_hash == "HUD"_fnv) { // not a hero
            game_object->transform->worldTransform = Matrix4x4f{ 
                3.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 3.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 3.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };

            return true;
        }
#endif

        //spdlog::info("VR: on_pre_gui_draw_element: {}", name);
        //spdlog::info("VR: on_pre_gui_draw_element: {} {:x}", name, (uintptr_t)game_object);

        auto view = sdk::call_object_func<REComponent*>(gui_element, "get_View", context, gui_element);

        if (view != nullptr) {
            const auto current_view_type = sdk::call_object_func<uint32_t>(view, "get_ViewType", context, view);

            if (current_view_type == (uint32_t)via::gui::ViewType::Screen) {
                static sdk::RETypeDefinition* via_render_mesh_typedef = nullptr;

                if (via_render_mesh_typedef == nullptr) {
                    via_render_mesh_typedef = sdk::find_type_definition("via.render.Mesh");

                    // wait
                    if (via_render_mesh_typedef == nullptr) {
                        return true;
                    }
                }

                // we don't want to mess with any game object that has a mesh
                // because it might be something physical in the game world
                // that the player can interact with
                if (utility::re_component::find(game_object->transform, via_render_mesh_typedef->get_type()) != nullptr) {
                    return true;
                }

                auto ui_scale = m_ui_scale_option->value();
                const auto world_ui_scale = m_world_ui_scale_option->value();

                auto& restore_data = g_elements_to_reset.emplace_back(std::make_unique<GUIRestoreData>());
                auto original_game_object_pos = sdk::get_transform_position(game_object->transform);

                restore_data->element = gui_element;
                restore_data->view = view;
                restore_data->original_position = original_game_object_pos;
                restore_data->overlay = sdk::call_object_func<bool>(view, "get_Overlay", context, view);
                restore_data->detonemap = sdk::call_object_func<bool>(view, "get_Detonemap", context, view);
                restore_data->view_type = (via::gui::ViewType)current_view_type;

                original_game_object_pos.w = 0.0f;

                // Set view type to world
                sdk::call_object_func<void*>(view, "set_ViewType", context, view, (uint32_t)via::gui::ViewType::World);

                // Set overlay = true (fixes double vision in VR)
                sdk::call_object_func<void*>(view, "set_Overlay", context, view, true);

                // Set detonemap = true (fixes weird tint)
                sdk::call_object_func<void*>(view, "set_Detonemap", context, view, true);

                // Go through the children until we hit a blur filter
                // And then remove it
                /*for (auto child = sdk::call_object_func<REComponent*>(view, "get_Child", sdk::get_thread_context(), view); child != nullptr; child = sdk::call_object_func<REComponent*>(child, "get_Child", sdk::get_thread_context(), child)) {
                    if (utility::re_managed_object::is_a(child, "via.gui.BlurFilter")) {
                        // Call remove()
                        sdk::call_object_func<void*>(child, "remove", sdk::get_thread_context(), child);
                        break;
                    }
                }*/

                auto camera = sdk::get_primary_camera();

                // Set the gui element's position to be in front of the camera
                if (camera != nullptr) {
                    auto camera_object = utility::re_component::get_game_object(camera);

                    if (camera_object != nullptr && camera_object->transform != nullptr) {
                        auto& gui_matrix = game_object->transform->worldTransform;
                        auto child = sdk::call_object_func<REManagedObject*>(view, "get_Child", context, view);

                        auto fix_2d_position = [&](const Vector4f& target_position, 
                                                    bool screen_correction = true,
                                                    std::optional<float> custom_ui_scale = std::nullopt)
                        {
                            if (!custom_ui_scale) {
                                custom_ui_scale = world_ui_scale;
                            }

                            auto delta = target_position - m_render_camera_matrix[3];
                            delta.w = 0.0f;

                            auto dir = glm::normalize(delta);
                            dir.w = 0.0f;
                            
                            // make matrix from dir
                            const auto look_mat = glm::rowMajor4(glm::lookAtLH(Vector3f{}, Vector3f{ dir }, Vector3f(0.0f, 1.0f, 0.0f)));
                            const auto look_rot = glm::quat{look_mat};

                            auto new_pos = target_position;
                            new_pos.w = 1.0f;

                            gui_matrix = look_mat;
                            gui_matrix[3] = new_pos;

                            // LESSON: DO NOT CALL THESE METHODS ON THE TRANSFORM!
                            // THEY CAUSE SOME STRANGE BUGS WHEN THE GUI ELEMENT HAS A PARENT TRANSFORM!
                            // THE GUI RENDERING FUNCTIONS PERFORM ON THE WORLD MATRIX, SO THIS IS NOT NECESSARY.
                            //sdk::set_transform_position(game_object->transform, new_pos);
                            //sdk::set_transform_rotation(game_object->transform, look_rot);
                            
                            const auto scaled_ui_scale = *custom_ui_scale * 0.01f;
                            const auto distance = glm::length(delta);
                            const auto scale = std::clamp<float>(distance * scaled_ui_scale, 0.1f, 100.0f);

                            regenny::via::Size gui_size{};
                            sdk::call_object_func<void*>(view, "get_ScreenSize", &gui_size, context, view);
                            
                            auto fix_transform_object = [&](::REManagedObject* object) {
                                static auto transform_object_type = sdk::find_type_definition("via.gui.TransformObject");

                                if (object == nullptr) {
                                    return;
                                }

                                const auto t = utility::re_managed_object::get_type_definition(object);

                                if (t == nullptr || !t->is_a(transform_object_type)) {
                                    return;
                                }

                                //sdk::call_object_func<void*>(object, "set_ResolutionAdjust", context, object, true);

                                if (screen_correction) {
                                    Vector3f half_size{ gui_size.w / 2.0f, gui_size.h / 2.0f, 0.0f };
                                    sdk::call_object_func<void*>(object, "set_Position", context, object, &half_size);
                                }

                                //Vector4f new_scale{ scale, scale, scale, 1.0f };
                                const auto old_scale = sdk::call_object_func_easy<Vector4f>(object, "get_Scale");
                                Vector4f new_scale{ old_scale.y, old_scale.y, old_scale.z, 1.0f };
                                sdk::call_object_func<void*>(object, "set_Scale", context, object, &new_scale);
                            };

                            for (auto c = child; c != nullptr; c = sdk::call_object_func<REManagedObject*>(c, "get_Next", context, c)) {
                                fix_transform_object(c);
                            }

                            gui_matrix = glm::scale(gui_matrix, Vector3f{ scale, scale, scale });
                        };

                        auto camera_transform = camera_object->transform;

                        const auto& camera_matrix = m_original_camera_matrix;
                        const auto& camera_position = camera_matrix[3];

                        glm::quat wanted_rotation{};
                        
                        bool wants_face_glue = false;
                        auto ui_distance = m_ui_distance_option->value();

                        switch (name_hash) {
                        case "damage_ui2102"_fnv:
                        case "NightVision_Filter"_fnv:
                            wants_face_glue = true;
                            break;
                        default:
                            break;
                        };

                        float right_world_adjust{};

                        switch (name_hash) {
                        case "GuiFront"_fnv:
                        case "GUI_Pause"_fnv:
                        case "GUIPause"_fnv:
                        case "GUIMapBg"_fnv:
                        case "BG"_fnv:
                            ui_distance += 0.1f; // give everything a little pop in the inventory.
                            break;
                        case "GUI_MapBG"_fnv:
                        case "GUI_Map"_fnv:
                        case "GUI_MapGrid"_fnv:
                        case "GUIMap"_fnv:
                        case "GUIMapIcon"_fnv:
                            ui_distance -= 0.025f; // give everything a little pop in the inventory.
                            break;
                        case "GuiCaption"_fnv:
                        case "GUIInventory"_fnv:
                        case "GUIInventoryCraft"_fnv:
                        case "GUIInventoryKeyItem"_fnv:
                        case "GUIInventoryTreasure"_fnv:
                        case "GUIBinder"_fnv:
                        case "GUIEquip"_fnv:
                            ui_distance -= 0.1f; // give everything a little pop in the inventory.
                            break;
                        case "GUI_RemainingBullet"_fnv:
                            right_world_adjust = 0.15f;
                            break;
                        case "GUI_Reticle"_fnv:
                        case "ReticleGUI"_fnv:
                        case "GUIReticle"_fnv:
                            ui_distance = 5.0f;
                            ui_scale = 2.0f;
                            break;
                        default:
                            break;
                        }

                        if (ui_distance < 0.0f) {
                            ui_distance = 0.0f;
                        }

                        // glues the GUI to the camera rotation and position
                        if (wants_face_glue) {
                            ui_distance = 5.0f;

                            wanted_rotation = glm::extractMatrixRotation(m_render_camera_matrix) * Matrix4x4f{
                                -1, 0, 0, 0,
                                0, 1, 0, 0,
                                0, 0, -1, 0,
                                0, 0, 0, 1
                            };
                        } else {
                            const auto gui_rotation_offset = get_gui_rotation_offset();

                            wanted_rotation = glm::extractMatrixRotation(camera_matrix) * Matrix4x4f{
                                -1, 0, 0, 0,
                                0, 1, 0, 0,
                                0, 0, -1, 0,
                                0, 0, 0, 1
                            };

                            if (m_decoupled_pitch->value()) {
                                bool is_exception = false;

                                switch (name_hash) {
                                    case "GUIReticle"_fnv:[[fallthrough]];
                                    case "ReticleGUI"_fnv:[[fallthrough]];
                                    case "GUI_Reticle"_fnv:
                                        is_exception = true;
                                        break;
                                    default:
                                        break;
                                }

                                if (!is_exception) {
                                    wanted_rotation = utility::math::flatten(wanted_rotation);
                                }
                            }

                            wanted_rotation = gui_rotation_offset * wanted_rotation;
                        }

                        const auto wanted_rotation_mat = Matrix4x4f{wanted_rotation};

                        gui_matrix = wanted_rotation_mat;
                        gui_matrix[3] = camera_position + (wanted_rotation_mat[2] * ui_distance) + (wanted_rotation_mat[0] * right_world_adjust);
                        gui_matrix[3].w = 1.0f;

                        // Scales the GUI so it's not massive.
                        if (!wants_face_glue) {
                            const auto scale = 1.0f / ui_scale;
                            gui_matrix = glm::scale(gui_matrix, Vector3f{ scale, scale, scale });
                        }

                        static auto gui_driver_typedef = sdk::find_type_definition(game_namespace("GUIDriver"));
                        static auto mhrise_npc_head_message_typedef = sdk::find_type_definition(game_namespace("gui.GuiCommonNpcHeadMessage"));
                        static auto mhrise_speech_balloon_typedef = sdk::find_type_definition(game_namespace("gui.GuiCommonNpcSpeechBalloon"));
                        static auto mhrise_head_message_typedef = sdk::find_type_definition(game_namespace("gui.GuiCommonHeadMessage"));
                        static auto mhrise_otomo_head_message_typedef = sdk::find_type_definition(game_namespace("gui.GuiCommonOtomoHeadMessage"));

                        static auto gameobject_elements_list = {
                            mhrise_npc_head_message_typedef,
                            mhrise_speech_balloon_typedef,
                            mhrise_head_message_typedef,
                            mhrise_otomo_head_message_typedef
                        };
                        
                        // Fix position of interaction icons
                        if (name_hash == "GUI_FloatIcon"_fnv || name_hash == "RogueFloatIcon"_fnv) { // RE2, RE3
                            if (name_hash == "GUI_FloatIcon"_fnv) {
                                m_last_interaction_display = std::chrono::steady_clock::now();
                            }
                        
                            fix_2d_position(original_game_object_pos);
                        } else if(gui_driver_typedef != nullptr) { // RE8
                            auto interact_icon_comp = utility::re_component::find(game_object->transform, gui_driver_typedef->get_type());

                            if (interact_icon_comp != nullptr) {
                                auto interact_icon_object = sdk::call_object_func<REGameObject*>(interact_icon_comp, "get_attachTarget", context, interact_icon_comp);

                                if (interact_icon_object != nullptr && interact_icon_object->transform != nullptr) {
                                    // call get_Position on the object
                                    Vector4f interact_icon_position{};
                                    sdk::call_object_func<Vector4f*>(interact_icon_object->transform, "get_Position", &interact_icon_position, context, interact_icon_object->transform);

                                    fix_2d_position(interact_icon_position);
                                }
                            }
                        } else {
                            // MHRise
                            for (auto element_type : gameobject_elements_list) {
                                if (element_type == nullptr) {
                                    continue;
                                }

                                auto element_comp = utility::re_component::find(game_object->transform, element_type->get_type());

                                if (element_comp == nullptr) {
                                    continue;
                                }

                                static auto get_parent_method = sdk::get_object_method(game_object->transform, "get_Parent");
                                auto parent = get_parent_method->call<::RETransform*>(context, game_object->transform);

                                if (parent != nullptr) {
                                    Vector4f offset{};

                                    if (element_type == mhrise_speech_balloon_typedef) {
                                        static auto pos_data_field = mhrise_speech_balloon_typedef->get_field("posData");
                                        static auto npc_message_pos = pos_data_field->get_type()->get_field("NpcMessagePos");

                                        auto pos_data = pos_data_field->get_data<::REManagedObject*>(element_comp);

                                        if (pos_data != nullptr) {
                                            const auto y_offset = npc_message_pos->get_data<float>(pos_data);
                                            offset = Vector4f{0.0f, y_offset, 0.0f, 0.0f};
                                        } else {
                                            offset = Vector4f{0.0f, 1.0f, 0.0f, 0.0f};
                                        }
                                    } else if (element_type == mhrise_npc_head_message_typedef) {
                                        static auto pos_data_field = mhrise_npc_head_message_typedef->get_field("posData");
                                        static auto npc_message_pos = pos_data_field->get_type()->get_field("NpcMessagePos");
                                        
                                        auto pos_data = pos_data_field->get_data<::REManagedObject*>(element_comp);

                                        if (pos_data != nullptr) {
                                            const auto y_offset = npc_message_pos->get_data<float>(pos_data);
                                            offset = Vector4f{0.0f, y_offset, 0.0f, 0.0f};
                                        } else {
                                            offset = Vector4f{0.0f, 1.0f, 0.0f, 0.0f};
                                        }
                                    } else if (element_type == mhrise_head_message_typedef) {
                                        static auto message_pos_y_field = mhrise_head_message_typedef->get_field("MessagePosY");

                                        const auto y_offset = message_pos_y_field->get_data<float>(element_comp);
                                        offset = Vector4f{0.0f, y_offset, 0.0f, 0.0f};
                                    } else if (element_type == mhrise_otomo_head_message_typedef) { //airou and dog
                                        offset = Vector4f{0.0, 1.0f, 0.0f, 0.0f};
                                    }

                                    fix_2d_position(sdk::get_transform_position(parent) + offset);
                                } else {
                                    fix_2d_position(original_game_object_pos);
                                }
                            }
                        }

                        // ... RE7
                        if (child != nullptr && utility::re_managed_object::get_field<wchar_t*>(child, "Name") == std::wstring_view(L"c_interact")) {
                            static auto ui_world_pos_attach_typedef = sdk::find_type_definition("app.UIWorldPosAttach");
                            auto world_pos_attach_comp = utility::re_component::find(game_object->transform, ui_world_pos_attach_typedef->get_type());

                            // Fix the world position of the gui element
                            if (world_pos_attach_comp != nullptr) {
                                const auto& target_pos = *sdk::get_object_field<Vector4f>(world_pos_attach_comp, "_NowTargetPos");

                                fix_2d_position(target_pos);
                            }
                        }
                    }
                }
            }
        }
    } else {
        spdlog::info("VR: on_pre_gui_draw_element: nullptr gameobject");
    }

    return true;
}

void VR::on_gui_draw_element(REComponent* gui_element, void* primitive_context) {
    //spdlog::info("VR: on_gui_draw_element");

    auto context = sdk::get_thread_context();

    // Restore elements back to original states
    for (auto& data : g_elements_to_reset) {
        sdk::call_object_func<void*>(data->view, "set_ViewType", context, data->view, (uint32_t)via::gui::ViewType::Screen);
        sdk::call_object_func<void*>(data->view, "set_Overlay", context, data->view, data->overlay);
        sdk::call_object_func<void*>(data->view, "set_Detonemap", context, data->view, data->detonemap);
        
        auto game_object = utility::re_component::get_game_object(data->element);

        if (game_object != nullptr && game_object->transform != nullptr) {
            //sdk::set_transform_position(game_object->transform, data->original_position);

            auto& gui_matrix = game_object->transform->worldTransform;
            gui_matrix[3] = data->original_position;
        }
    }

    g_elements_to_reset.clear();
    inside_gui_draw = false;
}

void VR::on_pre_update_before_lock_scene(void* ctx) {
    /*auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    auto projection_matrix = get_current_projection_matrix(true);

    // Steps towards getting lens flares and volumetric lighting working
    // Get the FOV from the projection matrix
    const auto vfov = glm::degrees(2.0f * std::atan(1.0f / projection_matrix[1][1]));
    const auto aspect = projection_matrix[1][1] / projection_matrix[0][0];
    const auto hfov = vfov * aspect;
    
    spdlog::info("vFOV: {}", vfov);
    spdlog::info("Aspect: {}", aspect);

    sdk::call_object_func<void*>(camera, "set_FOV", sdk::get_thread_context(), camera, vfov);
    sdk::call_object_func<void*>(camera, "set_VerticalEnable", sdk::get_thread_context(), camera, true);
    sdk::call_object_func<void*>(camera, "set_AspectRatio", sdk::get_thread_context(), camera, aspect);*/
}

void VR::on_pre_lightshaft_draw(void* shaft, void* render_context) {
    m_in_lightshaft = true;

    /*static auto transparent_layer_t = sdk::find_type_definition("via.render.layer.Transparent");
    auto transparent_layer = sdk::renderer::find_layer(transparent_layer_t->type);

    spdlog::info("transparent layer: {:x}", (uintptr_t)transparent_layer);

    if (transparent_layer == nullptr) {
        return;
    }

    static auto scene_layer_t = sdk::find_type_definition("via.render.layer.Scene");
    auto scene_layer = transparent_layer->find_parent(scene_layer_t->type);

    spdlog::info("scene layer: {:x}", (uintptr_t)scene_layer);

    if (scene_layer == nullptr) {
        return;
    }

    scene_layer->update();

    spdlog::info("scene layer update: {:x}", (uintptr_t)(*(void***)scene_layer)[sdk::renderer::RenderLayer::UPDATE_VTABLE_INDEX]);*/
}

void VR::on_lightshaft_draw(void* shaft, void* render_context) {
    m_in_lightshaft = false;
}

thread_local bool timed_out = false;

void VR::on_pre_begin_rendering(void* entry) {
    if (!m_openvr_loaded) {
        return;
    }

    m_in_render = true;

    // Use the gamepad/motion controller sticks to lerp the standing origin back to the center
    if (m_via_hid_gamepad.update()) {
        auto pad = sdk::call_native_func_easy<REManagedObject*>(m_via_hid_gamepad.object, m_via_hid_gamepad.t, "get_LastInputDevice");

        if (pad != nullptr) {
            // Move direction
            // It's not a Vector2f because via.vec2 is not actually 8 bytes, we don't want stack corruption to occur.
            const auto axis_l = (Vector2f)*utility::re_managed_object::get_field<Vector3f*>(pad, "AxisL");
            const auto axis_r = (Vector2f)*utility::re_managed_object::get_field<Vector3f*>(pad, "AxisR");

            // Lerp the standing origin back to HMD position
            // if the user is moving
            if (glm::length(axis_l) > 0.0f || glm::length(axis_r) > 0.0f) {
                const auto highest_length = std::max<float>(glm::length(axis_l), glm::length(axis_r));
                auto new_pos = get_position(vr::k_unTrackedDeviceIndex_Hmd);

                const auto delta = sdk::Application::get()->get_delta_time();

                new_pos.y = m_standing_origin.y;
                // Don't set the Y because it would look really strange
                m_standing_origin = glm::lerp(m_standing_origin, new_pos, ((float)highest_length * delta) * 0.01f);
            }
        }

        // TODO: do the same thing for the keyboard
        // will probably require some game-specific code
    }

    if (!inside_on_end && m_request_reinitialize_openvr) {
        std::scoped_lock _{m_openvr_mtx};

        m_request_reinitialize_openvr = false;
        reinitialize_openvr();
    }

    detect_controllers();

    actual_frame_count = get_game_frame_count();
    m_frame_count = actual_frame_count;

    /*if (!inside_on_end) {
        spdlog::info("VR: frame count: {}", m_frame_count);
    } else {
        spdlog::info("VR: frame count: {} (inside on_end)", m_frame_count);
    }*/

    // if we timed out, just return. we're assuming that the rendering will go on as normal
    if (timed_out) {
        if (inside_on_end) {
            spdlog::warn("VR: on_pre_wait_rendering: timed out inside_on_end");
        } else {
            spdlog::warn("VR: on_pre_wait_rendering: timed out");
        }

        return;
    }

    if (m_needs_wgp_update && inside_on_end) {
        spdlog::info("VR: on_pre_wait_rendering: inside on end!");
    }

    // Call WaitGetPoses
    if (m_needs_wgp_update && !inside_on_end) {
        m_needs_wgp_update = false;
        update_hmd_state();
    }

    if (m_needs_wgp_update) {
        return;
    }

    const auto should_update_camera = (m_frame_count % 2 == m_left_eye_interval) || is_using_afr();

    if (!inside_on_end && should_update_camera) {
        update_camera();
    } else if (inside_on_end) {
        update_camera_origin();
    }

    // update our internally stored render matrix
    update_render_matrix();
    //fix_temporal_effects(); // BAD way to do it!
}

void VR::on_begin_rendering(void* entry) {
    //spdlog::info("BeginRendering");
}

void VR::on_pre_end_rendering(void* entry) {

    //spdlog::info("EndRendering");
}

void VR::on_end_rendering(void* entry) {
    if (!m_openvr_loaded) {
        return;
    }

    if ((!m_is_hmd_active || m_needs_wgp_update) && !inside_on_end) {
        restore_camera();

        inside_on_end = false;
        m_in_render = false;
        return;
    }

    if (is_using_afr() || inside_on_end) {
        if (is_using_afr()) {
            restore_camera();
            m_in_render = false;
        }

        return;
    }

    // Only render again on even (left eye) frames
    // We're checking == 1 because at this point, the frame has finished.
    // Meaning the previous frame was a left eye frame.
    if (!inside_on_end && m_frame_count % 2 == m_left_eye_interval) {
        inside_on_end = true;
        
        // Try to render again for the right eye
        auto app = sdk::Application::get();

        static auto app_type = sdk::find_type_definition("via.Application");
        static auto set_max_delta_time_fn = app_type->get_method("set_MaxDeltaTime");

        // RE8 and onwards...
        // defaults to 2, and will slow the game down if frame rate is too low
        if (set_max_delta_time_fn != nullptr) {
            // static func, no need for app
            set_max_delta_time_fn->call<void*>(sdk::get_thread_context(), 10.0f);
        }

        static auto chain = app->generate_chain("WaitRendering", "EndRendering");
        static bool do_once = true;

        if (do_once) {
            do_once = false;

            // Remove these from the chain (std::vector)
            auto entries_to_remove = std::vector<std::string> {
                "UpdatePhysicsCharacterController",
                "UpdateTelemetry",
                "UpdateMovie", // Causes movies to play twice as fast if ran again
                "UpdateSpeedTree",
                "UpdateHansoft",
                // The dynamics stuff causes a cloth physics step in the right eye
                "BeginRenderingDynamics",
                "BeginDynamics",
                "EndRenderingDynamics",
                "EndDynamics",
                "EndPhysics",
                "RenderDynamics",
                "RenderLandscape",
                "DevelopRenderer",
                "DrawWidget"
            };

            for (auto& entry : entries_to_remove) {
                chain.erase(std::remove_if(chain.begin(), chain.end(), [&](auto& func) {
                    return entry == func->description;
                }), chain.end());
            }
        }

        sdk::renderer::begin_update_primitive();

        //static auto update_geometry = app->get_function("UpdateGeometry");
        static auto begin_update_effect = app->get_function("BeginUpdateEffect");
        static auto update_effect = app->get_function("UpdateEffect");
        static auto end_update_effect = app->get_function("EndUpdateEffect");
        static auto prerender_gui = app->get_function("PrerenderGUI");
        static auto begin_update_primitive = app->get_function("BeginUpdatePrimitive");

        // SO. Let me explain what's happening here.
        // If we try and just render a frame in this naive way in this order:
        // BeginUpdatePrimitive,
        // WaitRendering,
        // BeginRendering,
        // UpdatePrimitive,
        // EndPrimitive,
        // EndRendering,
        // This will end up having a chance to crash when rendering fluid effects for some reason when calling UpdatePrimitive.
        // The crash happens because some pipeline state inside the fluid simulator gets set to null.
        // So, we manually call BeginEffect, and then EndUpdateEffect
        // Which somehow solves the crash.
        // We don't call UpdateEffect because it will make effects appear to run at a higher framerate
        if (begin_update_primitive != nullptr) {
            begin_update_primitive->func(begin_update_primitive->entry);
        }

        if (begin_update_effect != nullptr) {
            begin_update_effect->func(begin_update_effect->entry);
        }

        /*if (update_effect != nullptr) {
            update_effect->func(update_effect->entry);
        }*/

        if (end_update_effect != nullptr) {
            end_update_effect->func(end_update_effect->entry);
        }

        if (prerender_gui != nullptr) {
            prerender_gui->func(prerender_gui->entry);
        }

        for (auto func : chain) {
            //spdlog::info("Calling {}", func->description);

            func->func(func->entry);
        }

        restore_camera();

        m_in_render = false;
        inside_on_end = false;
    }
}

void VR::on_pre_wait_rendering(void* entry) {
    if (!m_openvr_loaded) {
        return;
    }

    timed_out = false;

    if (!m_is_hmd_active) {
        return;
    }

    // wait for m_present_finished (std::condition_variable)
    // to be signaled
    {
        std::unique_lock lock{m_present_finished_mtx};
        const auto now = std::chrono::steady_clock::now();
        
        if (!m_present_finished_cv.wait_until(lock, now + std::chrono::milliseconds(333), [&]() { return m_present_finished; })) {
            timed_out = true;
        }

        m_present_finished = false;
    }
}

void VR::on_wait_rendering(void* entry) {
}

void VR::on_pre_application_entry(void* entry, const char* name, size_t hash) {
    if (!m_openvr_loaded) {
        return;
    }

    switch (hash) {
        case "UpdateHID"_fnv:
            on_pre_update_hid(entry);
            break;
        case "WaitRendering"_fnv:
            on_pre_wait_rendering(entry);
            break;
        case "BeginRendering"_fnv:
            on_pre_begin_rendering(entry);
            break;
        case "EndRendering"_fnv:
            on_pre_end_rendering(entry);
            break;
        default:
            break;
    }
}

void VR::on_application_entry(void* entry, const char* name, size_t hash) {
    if (!m_openvr_loaded) {
        return;
    }

    switch (hash) {
        case "UpdateHID"_fnv:
            on_update_hid(entry);
            break;
        case "WaitRendering"_fnv:
            on_wait_rendering(entry);
            break;
        case "BeginRendering"_fnv:
            on_begin_rendering(entry);
            break;
        case "EndRendering"_fnv:
            on_end_rendering(entry);
            break;
        default:
            break;
    }
}

void VR::on_pre_update_hid(void* entry) {
    if (!m_openvr_loaded || !m_is_hmd_active) {
        return;
    }

    update_action_states();
}

void VR::on_update_hid(void* entry) {
    if (!m_openvr_loaded || !m_is_hmd_active) {
        return;
    }

#if not defined(RE2) and not defined(RE3)
    this->openvr_input_to_re_engine();
#endif
}

void VR::openvr_input_to_re2_re3(REManagedObject* input_system) {
    if (!m_openvr_loaded) {
        return;
    }

    // Get OpenVR input system
    auto openvr_input = vr::VRInput();

    if (openvr_input == nullptr) {
        spdlog::error("[VR] Failed to get OpenVR input system.");
        return;
    }

    static auto gui_master_type = sdk::find_type_definition(game_namespace("gui.GUIMaster"));
    static auto gui_master_get_instance = gui_master_type->get_method("get_Instance");
    static auto gui_master_get_input = gui_master_type->get_method("get_Input");
    static auto gui_master_input_type = gui_master_get_input->get_return_type();

    // ??????
#ifdef RE2
    static auto input_set_is_trigger_move_up_d = gui_master_input_type->get_method("set_IsTriggerMoveUpD");
    static auto input_set_is_trigger_move_down_d = gui_master_input_type->get_method("set_IsTriggerMoveDownD");
#endif

    auto gui_master = gui_master_get_instance->call<::REManagedObject*>(sdk::get_thread_context());
    auto gui_input = gui_master != nullptr ? 
                     gui_master_get_input->call<::REManagedObject*>(sdk::get_thread_context(), gui_master) : 
                     (::REManagedObject*)nullptr;

    auto ctx = sdk::get_thread_context();

    static auto get_lstick_method = sdk::get_object_method(input_system, "get_LStick");
    static auto get_rstick_method = sdk::get_object_method(input_system, "get_RStick");
    auto lstick = get_lstick_method->call<::REManagedObject*>(ctx);
    auto rstick = get_rstick_method->call<::REManagedObject*>(ctx);

    if (lstick == nullptr || rstick == nullptr) {
        return;
    }

    static auto get_button_bits_method = sdk::get_object_method(input_system, "get_ButtonBits");
    auto button_bits_obj = get_button_bits_method->call<::REManagedObject*>(ctx, input_system);

    if (button_bits_obj == nullptr) {
        return;
    }

    static auto get_active_user_input_unit_method = sdk::get_object_method(input_system, "getActiveUserInputUnit");
    auto input_unit_obj = get_active_user_input_unit_method->call<::REManagedObject*>(ctx, input_system);

    if (input_unit_obj == nullptr) {
        return;
    }

    const auto now = std::chrono::steady_clock::now();
    auto is_using_controller = (now - get_last_controller_update()) <= std::chrono::seconds(10);

    const auto is_grip_down = is_action_active(m_action_grip, m_right_joystick);
    const auto is_trigger_down = is_action_active(m_action_trigger, m_right_joystick);
    const auto is_left_grip_down = is_action_active(m_action_grip, m_left_joystick);
    const auto is_left_trigger_down = is_action_active(m_action_trigger, m_left_joystick);
    const auto is_left_joystick_click_down = is_action_active(m_action_joystick_click, m_left_joystick);
    const auto is_right_joystick_click_down = is_action_active(m_action_joystick_click, m_right_joystick);

    const auto is_left_a_button_down = is_action_active(m_action_a_button, m_left_joystick);
    const auto is_left_b_button_down = is_action_active(m_action_b_button, m_left_joystick);
    const auto is_right_a_button_down = is_action_active(m_action_a_button, m_right_joystick);
    const auto is_right_b_button_down = is_action_active(m_action_b_button, m_right_joystick);

    const auto is_dpad_up_down = is_action_active(m_action_dpad_up, m_left_joystick) || is_action_active(m_action_dpad_up, m_right_joystick);
    const auto is_dpad_right_down = is_action_active(m_action_dpad_right, m_left_joystick) || is_action_active(m_action_dpad_right, m_right_joystick);
    const auto is_dpad_down_down = is_action_active(m_action_dpad_down, m_left_joystick) || is_action_active(m_action_dpad_down, m_right_joystick);
    const auto is_dpad_left_down = is_action_active(m_action_dpad_left, m_left_joystick) || is_action_active(m_action_dpad_left, m_right_joystick);

    const auto is_weapon_dial_down = is_action_active(m_action_weapon_dial, m_left_joystick) || is_action_active(m_action_weapon_dial, m_right_joystick);
    const auto is_re3_dodge_down = is_action_active(m_action_re3_dodge, m_left_joystick) || is_action_active(m_action_re3_dodge, m_right_joystick);
    const auto is_quickturn_down = is_action_active(m_action_re2_quickturn, m_left_joystick) || is_action_active(m_action_re2_quickturn, m_right_joystick);
    const auto is_reset_view_down = is_action_active(m_action_re2_reset_view, m_left_joystick) || is_action_active(m_action_re2_reset_view, m_right_joystick);
    const auto is_change_ammo_down = is_action_active(m_action_re2_change_ammo, m_left_joystick) || is_action_active(m_action_re2_change_ammo, m_right_joystick);
    const auto is_minimap_down = is_action_active(m_action_minimap, m_left_joystick) || is_action_active(m_action_minimap, m_right_joystick);
	const auto is_toggle_flashlight_down = is_action_active(m_action_re2_toggle_flashlight, m_left_joystick);

    const auto is_left_system_button_down = is_action_active(m_action_system_button, m_left_joystick);
    const auto is_right_system_button_down = is_action_active(m_action_system_button, m_right_joystick);

    
    
#if defined(RE2) || defined(RE3) || defined(RE8)
    if (is_toggle_flashlight_down && !m_was_flashlight_toggle_down) {            
        ManualFlashlight::g_manual_flashlight-> toggle_flashlight();
    }    

    m_was_flashlight_toggle_down = is_toggle_flashlight_down;
#endif


#if defined(RE2) || defined(RE3)
    const auto is_firstperson_toggle_down = is_action_active(m_action_re2_firstperson_toggle, m_left_joystick) || is_action_active(m_action_re2_firstperson_toggle, m_right_joystick);

    if (is_firstperson_toggle_down && !m_was_firstperson_toggle_down) {
        FirstPerson::get()->toggle();
    }

    m_was_firstperson_toggle_down = is_firstperson_toggle_down;
#endif

#if defined(RE2) || defined(RE3)
    const auto is_gripping_weapon = FirstPerson::get()->was_gripping_weapon();
#else
    const auto is_gripping_weapon = false;
#endif

    // Current actual button bits used by the game
    auto& button_bits_down = *sdk::get_object_field<uint64_t>(button_bits_obj, "Down");
    auto& button_bits_on = *sdk::get_object_field<uint64_t>(button_bits_obj, "On");
    auto& button_bits_up = *sdk::get_object_field<uint64_t>(button_bits_obj, "Up");

    // Set button state based on our own history we keep that doesn't get overwritten by the game
    auto set_button_state = [&](app::ropeway::InputDefine::Kind kind, bool state) {
        const auto kind_uint64 = (uint64_t)kind;

        if (state) {
            m_last_controller_update = now;
            is_using_controller = true;

            button_bits_up &= ~kind_uint64;
            m_button_states_up &= ~kind_uint64;

            // if "On" state is not set
            if ((m_button_states_on.to_ullong() & kind_uint64) == 0) {
                if (m_button_states_down.to_ullong() & kind_uint64) {
                    m_button_states_on |= kind_uint64;
                    m_button_states_down &= ~kind_uint64;

                    button_bits_on |= kind_uint64;
                    button_bits_down &= ~kind_uint64;
                } else {
                    m_button_states_on &= ~kind_uint64;
                    m_button_states_down |= kind_uint64;

                    button_bits_on &= ~kind_uint64;
                    button_bits_down |= kind_uint64;
                }
            } else {
                m_button_states_down &= ~kind_uint64;
                button_bits_down &= ~kind_uint64;

                m_button_states_on |= kind_uint64;
                button_bits_on |= kind_uint64;
            }
        } else {
            if (m_button_states_down.to_ullong() & kind_uint64 || m_button_states_on.to_ullong() & kind_uint64) {
                m_button_states_up |= kind_uint64;
                button_bits_up |= kind_uint64;

                m_last_controller_update = now;
                is_using_controller = true;
            } else if (is_using_controller) {
                m_button_states_up &= ~kind_uint64;
                button_bits_up &= ~kind_uint64;
            }

            // Don't want to screw with the user's input if they aren't actively
            // Using their VR controllers
            if (is_using_controller) {
                button_bits_down &= ~kind_uint64;
                m_button_states_down &= ~kind_uint64;
                
                button_bits_on &= ~kind_uint64;
                m_button_states_on &= ~kind_uint64;
            }
        }
    };

    // Right Grip: Aim, UI Right (RB)
    set_button_state(app::ropeway::InputDefine::Kind::HOLD, is_grip_down);
    set_button_state(app::ropeway::InputDefine::Kind::UI_SHIFT_RIGHT, is_grip_down);

    // Left Grip: Alternate aim (grenades, knives, etc), UI left (LB)
    set_button_state(app::ropeway::InputDefine::Kind::SUPPORT_HOLD, is_left_grip_down && !is_gripping_weapon);   
    set_button_state(app::ropeway::InputDefine::Kind::UI_SHIFT_LEFT, is_left_grip_down);

    // Right Trigger (RB): Attack, Alternate UI right (RT), GE_RTrigBottom (quick time event), GE_RTrigTop (another quick time event)
    set_button_state(app::ropeway::InputDefine::Kind::ATTACK, is_trigger_down);
    set_button_state(app::ropeway::InputDefine::Kind::UI_SHIFT_RIGHT_2, is_trigger_down);
    set_button_state((app::ropeway::InputDefine::Kind)18014398509481984, is_trigger_down);
    set_button_state((app::ropeway::InputDefine::Kind)9007199254740992, is_trigger_down);
    //set_button_state((app::ropeway::InputDefine::Kind)4503599627370496, is_trigger_down);

    // Left Trigger (LB): Alternate UI left (LT), DEFENSE (LB)
    set_button_state(app::ropeway::InputDefine::Kind::UI_SHIFT_LEFT_2, is_left_trigger_down);
    set_button_state(app::ropeway::InputDefine::Kind::DEFENSE, is_left_trigger_down);

    // L3: Sprint
    set_button_state(app::ropeway::InputDefine::Kind::JOG1, is_left_joystick_click_down);

    // R3: Reset camera
    set_button_state(app::ropeway::InputDefine::Kind::RESET_CAMERA, is_reset_view_down);

    // Left B: Inventory, PRESS_START
    set_button_state(app::ropeway::InputDefine::Kind::INVENTORY, is_left_b_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::PRESS_START, is_left_b_button_down);

    // Left A: QUICK_TURN, PRESS_START, CANCEL, DIALOG_CANCEL
    set_button_state(app::ropeway::InputDefine::Kind::QUICK_TURN, is_quickturn_down); // unique, unbound by default as it causes issues
    set_button_state(app::ropeway::InputDefine::Kind::PRESS_START, is_left_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::CANCEL, is_left_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::DIALOG_CANCEL, is_left_a_button_down);
    
    // Right A: Action, ITEM, PRESS_START, DECIDE, DIALOG_DECIDE, (1 << 51)
    set_button_state(app::ropeway::InputDefine::Kind::ACTION, is_right_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::ITEM, is_right_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::PRESS_START, is_right_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::DECIDE, is_right_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::DIALOG_DECIDE, is_right_a_button_down);

    // only allow dodging if there's nothing to interact with nearby
    if (!is_right_a_button_down || (now - m_last_interaction_display) >= std::chrono::milliseconds(200)) {
        set_button_state((app::ropeway::InputDefine::Kind)((uint64_t)1 << 51), is_re3_dodge_down); // RE3 dodge
    } else {
        set_button_state((app::ropeway::InputDefine::Kind)((uint64_t)1 << 51), false); // RE3 dodge
    }
    
    // Right B: Reload, Skip Event, UI_EXCHANGE, UI_RESET, (1 << 52) (that one is RE3 only? don't see it in the enum)
    set_button_state(app::ropeway::InputDefine::Kind::RELOAD, is_right_b_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::SKIP_EVENT, is_right_b_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::UI_EXCHANGE, is_right_b_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::UI_RESET, is_right_b_button_down);
    set_button_state((app::ropeway::InputDefine::Kind)((uint64_t)1 << 52), is_right_b_button_down);

    const auto left_axis = get_left_stick_axis();
    const auto right_axis = get_right_stick_axis();
    const auto left_axis_len = glm::length(left_axis);
    const auto right_axis_len = glm::length(right_axis);

    if (!is_weapon_dial_down) {
        // DPad Up: Shortcut Up
        set_button_state(app::ropeway::InputDefine::Kind::SHORTCUT_UP, is_dpad_up_down);
        set_button_state(app::ropeway::InputDefine::Kind::UI_MAP_UP, is_dpad_up_down);
        set_button_state(app::ropeway::InputDefine::Kind::UI_UP, is_dpad_up_down); // IsTriggerMoveUpD in RE3

        // DPad Right: Shortcut Right
        set_button_state(app::ropeway::InputDefine::Kind::SHORTCUT_RIGHT, is_dpad_right_down);

        // DPad Down: Shortcut Down
        set_button_state(app::ropeway::InputDefine::Kind::SHORTCUT_DOWN, is_dpad_down_down);
        set_button_state(app::ropeway::InputDefine::Kind::UI_MAP_DOWN, is_dpad_down_down);
        set_button_state(app::ropeway::InputDefine::Kind::UI_DOWN, is_dpad_down_down); // IsTriggerMoveDownD in RE3

        // DPad Left: Shortcut Left
        set_button_state(app::ropeway::InputDefine::Kind::SHORTCUT_LEFT, is_dpad_left_down);

        // well this was really annoying to figure out
#ifdef RE2
        if (is_dpad_up_down && gui_input != nullptr) {
            input_set_is_trigger_move_up_d->call<void*>(ctx, gui_input, true);
        }

        if (is_dpad_down_down && gui_input != nullptr) {
            input_set_is_trigger_move_down_d->call<void*>(ctx, gui_input, true);
        }
#endif
    } else {
        set_button_state(app::ropeway::InputDefine::Kind::SHORTCUT_UP, left_axis.y > 0.9f);
        set_button_state(app::ropeway::InputDefine::Kind::SHORTCUT_RIGHT, left_axis.x > 0.9f);
        set_button_state(app::ropeway::InputDefine::Kind::SHORTCUT_DOWN, left_axis.y < -0.9f);
        set_button_state(app::ropeway::InputDefine::Kind::SHORTCUT_LEFT, left_axis.x < -0.9f);
    }

    // Change Ammo
    set_button_state(app::ropeway::InputDefine::Kind::CHANGE_BULLET, is_change_ammo_down);

    // MiniMap
    set_button_state(app::ropeway::InputDefine::Kind::MINIMAP, is_minimap_down);

    // Left or Right System Button: Pause
    set_button_state(app::ropeway::InputDefine::Kind::PAUSE, is_left_system_button_down || is_right_system_button_down || m_handle_pause);
    m_handle_pause = false;

    // Fixes QTE bound to triggers
    if (is_using_controller) {
        const auto left_trigger_state = is_left_trigger_down ? 1.0f : 0.0f;
        const auto right_trigger_state = is_trigger_down ? 1.0f : 0.0f;

        static auto set__analog_l_method = sdk::get_object_method(input_unit_obj, "set__AnalogL");
        static auto set__analog_r_method = sdk::get_object_method(input_unit_obj, "set__AnalogR");
        set__analog_l_method->call<void*>(ctx, input_unit_obj, left_trigger_state);
        set__analog_r_method->call<void*>(ctx, input_unit_obj, right_trigger_state);
    }

    bool moved_sticks = false;
    const auto deadzone = m_joystick_deadzone->value();

    if (left_axis_len > deadzone) {
        moved_sticks = true;

        // Override the left stick's axis values to the VR controller's values
        Vector3f axis{ left_axis.x, left_axis.y, 0.0f };

        static auto update_method = sdk::get_object_method(lstick, "update");
        update_method->call<void*>(ctx, lstick, &axis, &axis);
    }

    if (right_axis_len > deadzone) {
        moved_sticks = true;

        // Override the right stick's axis values to the VR controller's values
        Vector3f axis{ right_axis.x, right_axis.y, 0.0f };

        static auto update_method = sdk::get_object_method(rstick, "update");
        update_method->call<void*>(ctx, rstick, &axis, &axis);
    }

    if (moved_sticks) {
        auto new_pos = get_position(vr::k_unTrackedDeviceIndex_Hmd);

        const auto delta = sdk::Application::get()->get_delta_time();
        const auto highest_length = std::max<float>(glm::length(right_axis), glm::length(left_axis));

        new_pos.y = m_standing_origin.y;
        // Don't set the Y because it would look really strange
        m_standing_origin = glm::lerp(m_standing_origin, new_pos, ((float)highest_length * delta) * 0.01f);
    }

    set_button_state(app::ropeway::InputDefine::Kind::MOVE, left_axis_len > deadzone);
    set_button_state(app::ropeway::InputDefine::Kind::UI_L_STICK, left_axis_len > deadzone);

    set_button_state(app::ropeway::InputDefine::Kind::WATCH, right_axis_len > deadzone);
    set_button_state(app::ropeway::InputDefine::Kind::UI_R_STICK, right_axis_len > deadzone);
    //set_button_state(app::ropeway::InputDefine::Kind::RUN, right_axis_len > 0.01f);

    // Causes the right stick to take effect properly
    if (is_using_controller) {
        static auto set_input_mode_method = sdk::get_object_method(input_system, "set_InputMode");
        set_input_mode_method->call<void*>(ctx, input_system, app::ropeway::InputDefine::InputMode::Pad);
    }
}

void VR::openvr_input_to_re_engine() {
    // TODO: Get the "merged pad" and actually modify some inputs!

    const auto left_axis = get_left_stick_axis();
    const auto right_axis = get_right_stick_axis();
    const auto left_axis_len = glm::length(left_axis);
    const auto right_axis_len = glm::length(right_axis);
    const auto now = std::chrono::steady_clock::now();

    bool moved_sticks = false;

    if (left_axis_len > m_joystick_deadzone->value()) {
        moved_sticks = true;
    }

    if (right_axis_len > m_joystick_deadzone->value()) {
        moved_sticks = true;
    }

    if (moved_sticks) {
        auto new_pos = get_position(vr::k_unTrackedDeviceIndex_Hmd);

        const auto delta = sdk::Application::get()->get_delta_time();
        const auto highest_length = std::max<float>(glm::length(right_axis), glm::length(left_axis));

        new_pos.y = m_standing_origin.y;
        // Don't set the Y because it would look really strange
        m_standing_origin = glm::lerp(m_standing_origin, new_pos, ((float)highest_length * delta) * 0.01f);
        m_last_controller_update = now;
    }

    for (auto& it : m_action_handles) {
        if (is_action_active(it.second, m_left_joystick) || is_action_active(it.second, m_right_joystick)) {
            m_last_controller_update = now;
        }
    }

#ifdef RE7
    if (m_handle_pause) {
        auto menu_manager = sdk::get_managed_singleton<::REManagedObject>("app.MenuManager");

        if (menu_manager != nullptr) {
            sdk::call_object_func<void*>(menu_manager, "openPauseMenu", sdk::get_thread_context(), menu_manager, 0);
            m_handle_pause = false;
        }
    }
#endif

#ifdef RE8
    if (m_handle_pause) {
        auto gui_manager = sdk::get_managed_singleton<::REManagedObject>("app.GUIManager");

        if (gui_manager != nullptr) {
            constexpr uint32_t PAUSE_ALIAS = 0x7DEE1056;
            const auto gui_handle = sdk::call_object_func<::REManagedObject*>(gui_manager, "findGUI(System.UInt32)", sdk::get_thread_context(), gui_manager, PAUSE_ALIAS);

            if (gui_handle != nullptr) {
                auto driver = sdk::call_object_func<::REManagedObject*>(gui_handle, "get_driver", sdk::get_thread_context(), gui_handle);

                if (driver != nullptr) {
                    const auto fake_param = sdk::create_instance("app.GUIRequest.Parameter");

                    if (fake_param != nullptr) {
                        sdk::call_object_func<void*>(driver, "show", sdk::get_thread_context(), driver, fake_param);
                    } else {
                        spdlog::error("Failed to create fake app.GUIRequest.Parameter, cannot pause!");
                    }
                }
            }
        }

        m_handle_pause = false;
    }
#endif
}

void VR::on_draw_ui() {
    // create VR tree entry in menu (imgui)
    if (m_openvr_loaded) {
        ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);
    } else {
        if (m_openvr_error && !m_openvr_dll_missing) {
            ImGui::SetNextTreeNodeOpen(true, ImGuiCond_::ImGuiCond_FirstUseEver);
        } else {
            ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);
        }
    }

    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    if (m_openxr.error) {
        ImGui::Text("OpenXR Error: %s", m_openxr.error->c_str());
    }

    if (!m_openvr_loaded) {
        if (m_openvr_error && m_openvr_dll_missing) {
            ImGui::Text("VR not loaded: openvr_api.dll not found");
            ImGui::Text("Please drop the openvr_api.dll file into the game's directory if you want to use VR");
        } else if (m_openvr_error) {
            ImGui::Text("VR not loaded: %s", m_openvr_error->c_str());
        } else {
            ImGui::Text("VR not loaded: Unknown error");
        }

        return;
    }

    // draw VR tree entry in menu (imgui)
    ImGui::Text("Render Resolution: %d x %d", m_w, m_h);
    ImGui::Text("Resolution can be changed in SteamVR");
    ImGui::Separator();

    if (ImGui::Button("Set Standing Height")) {
        m_standing_origin.y = get_position(0).y;
    }

    if (ImGui::Button("Set Standing Origin") || m_set_standing_key->is_key_down_once()) {
        m_standing_origin = get_position(0);
    }

    if (ImGui::Button("Recenter View") || m_recenter_view_key->is_key_down_once()) {
        recenter_view();
    }

    if (ImGui::Button("Reinitialize OpenVR")) {
        m_request_reinitialize_openvr = true;
    }

    //ImGui::DragFloat4("Right Bounds", (float*)&m_right_bounds, 0.005f, -2.0f, 2.0f);
    //ImGui::DragFloat4("Left Bounds", (float*)&m_left_bounds, 0.005f, -2.0f, 2.0f);

    ImGui::Separator();

    m_set_standing_key->draw("Set Standing Origin Key");
    m_recenter_view_key->draw("Recenter View Key");

    ImGui::Separator();

    m_use_afr->draw("Use AFR");
    m_decoupled_pitch->draw("Decoupled Camera Pitch");

    if (ImGui::Checkbox("Positional Tracking", &m_positional_tracking)) {
    }

    /*if (ImGui::Checkbox("Depth Aided Reprojection", &m_depth_aided_reprojection)) {
    }*/

    m_hmd_oriented_audio->draw("Head Oriented Audio");
    m_use_custom_view_distance->draw("Use Custom View Distance");
    m_view_distance->draw("View Distance/FarZ");
    m_motion_controls_inactivity_timer->draw("Inactivity Timer");
    m_joystick_deadzone->draw("Joystick Deadzone");

    m_ui_scale_option->draw("2D UI Scale");
    m_ui_distance_option->draw("2D UI Distance");
    m_world_ui_scale_option->draw("World-Space UI Scale");

    ImGui::DragFloat3("Overlay Rotation", (float*)&m_overlay_rotation, 0.01f, -360.0f, 360.0f);
    ImGui::DragFloat3("Overlay Position", (float*)&m_overlay_position, 0.01f, -100.0f, 100.0f);

    ImGui::Separator();
    ImGui::Text("Graphical Options");

    m_force_fps_settings->draw("Force Uncap FPS");
    m_force_aa_settings->draw("Force Disable TAA");
    m_force_motionblur_settings->draw("Force Disable Motion Blur");
    m_force_vsync_settings->draw("Force Disable V-Sync");
    m_force_lensdistortion_settings->draw("Force Disable Lens Distortion");
    m_force_volumetrics_settings->draw("Force Disable Volumetrics");
    m_force_lensflares_settings->draw("Force Disable Lens Flares");
    m_force_dynamic_shadows_settings->draw("Force Enable Dynamic Shadows");
    m_allow_engine_overlays->draw("Allow Engine Overlays");

    ImGui::Separator();
    ImGui::Text("Debug info");
    ImGui::Checkbox("Disable Projection Matrix Override", &m_disable_projection_matrix_override);
    ImGui::Checkbox("Disable GUI Projection Matrix Override", &m_disable_gui_camera_projection_matrix_override);
    ImGui::Checkbox("Disable View Matrix Override", &m_disable_view_matrix_override);
    ImGui::Checkbox("Disable Backbuffer Size Override", &m_disable_backbuffer_size_override);
    ImGui::Checkbox("Disable Temporal Fix", &m_disable_temporal_fix);
    ImGui::Checkbox("Disable Post Effect Fix", &m_disable_post_effect_fix);

    ImGui::DragFloat4("Raw Left", (float*)&m_raw_projections[0], 0.01f, -100.0f, 100.0f);
    ImGui::DragFloat4("Raw Right", (float*)&m_raw_projections[1], 0.01f, -100.0f, 100.0f);

    // convert m_avg_input_delay (std::chrono::nanoseconds) to milliseconds (float)
    auto duration_float = std::chrono::duration<float, std::milli>(m_avg_input_delay).count();

    ImGui::DragFloat("Avg Input Processing Delay (MS)", &duration_float, 0.00001f);
}

void VR::on_device_reset() {
    spdlog::info("VR: on_device_reset");
    m_backbuffer_inconsistency = false;
    m_d3d11.on_reset(this);
    m_d3d12.on_reset(this);
    m_overlay_component.on_reset();

    // so i guess device resets can happen between begin and end rendering...
    if (m_in_render) {
        spdlog::info("VR: on_device_reset: in_render");

        // what the fuck
        if (m_needs_camera_restore) {
            spdlog::info("VR: on_device_reset: needs_camera_restore");
            restore_camera();
        }
    }

    if (inside_on_end) {
        spdlog::info("VR: on_device_reset: inside_on_end");
    }
}

void VR::on_config_load(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }
}

void VR::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}

Vector4f VR::get_position(uint32_t index) {
    if (index >= vr::k_unMaxTrackedDeviceCount) {
        return Vector4f{};
    }

    std::shared_lock _{ m_pose_mtx };

    return get_position_unsafe(index);
}

Vector4f VR::get_velocity(uint32_t index) {
    if (index >= vr::k_unMaxTrackedDeviceCount) {
        return Vector4f{};
    }

    std::shared_lock _{ m_pose_mtx };

    return get_velocity_unsafe(index);
}

Vector4f VR::get_angular_velocity(uint32_t index) {
    if (index >= vr::k_unMaxTrackedDeviceCount) {
        return Vector4f{};
    }

    std::shared_lock _{ m_pose_mtx };

    return get_angular_velocity_unsafe(index);
}

Vector4f VR::get_position_unsafe(uint32_t index) {
    if (index >= vr::k_unMaxTrackedDeviceCount) {
        return Vector4f{};
    }

    auto& pose = get_poses()[index];
    auto matrix = Matrix4x4f{ *(Matrix3x4f*)&pose.mDeviceToAbsoluteTracking };
    auto result = glm::rowMajor4(matrix)[3];
    result.w = 1.0f;

    return result;
}

Vector4f VR::get_velocity_unsafe(uint32_t index) {
    if (index >= vr::k_unMaxTrackedDeviceCount) {
        return Vector4f{};
    }

    const auto& pose = get_poses()[index];
    const auto& velocity = pose.vVelocity;

    return Vector4f{ velocity.v[0], velocity.v[1], velocity.v[2], 0.0f };
}

Vector4f VR::get_angular_velocity_unsafe(uint32_t index) {
    if (index >= vr::k_unMaxTrackedDeviceCount) {
        return Vector4f{};
    }

    const auto& pose = get_poses()[index];
    const auto& angular_velocity = pose.vAngularVelocity;

    return Vector4f{ angular_velocity.v[0], angular_velocity.v[1], angular_velocity.v[2], 0.0f };
}

Matrix4x4f VR::get_rotation(uint32_t index) {
    if (index >= vr::k_unMaxTrackedDeviceCount) {
        return glm::identity<Matrix4x4f>();
    }

    std::shared_lock _{ m_pose_mtx };

    auto& pose = get_poses()[index];
    auto matrix = Matrix4x4f{ *(Matrix3x4f*)&pose.mDeviceToAbsoluteTracking };
    return glm::extractMatrixRotation(glm::rowMajor4(matrix));
}

Matrix4x4f VR::get_transform(uint32_t index) {
    if (index >= vr::k_unMaxTrackedDeviceCount) {
        return glm::identity<Matrix4x4f>();
    }

    std::shared_lock _{ m_pose_mtx };

    auto& pose = get_poses()[index];
    auto matrix = Matrix4x4f{ *(Matrix3x4f*)&pose.mDeviceToAbsoluteTracking };
    return glm::rowMajor4(matrix);
}

vr::HmdMatrix34_t VR::get_raw_transform(uint32_t index) {
    if (index >= vr::k_unMaxTrackedDeviceCount) {
        return vr::HmdMatrix34_t{};
    }

    std::shared_lock _{ m_pose_mtx };

    auto& pose = get_poses()[index];
    return pose.mDeviceToAbsoluteTracking;
}

bool VR::is_action_active(vr::VRActionHandle_t action, vr::VRInputValueHandle_t source) const {
    if (!m_openvr_loaded) {
        return false;
    }

    vr::InputDigitalActionData_t data{};
	vr::VRInput()->GetDigitalActionData(action, &data, sizeof(data), source);

    return data.bActive && data.bState;
}

Vector2f VR::get_joystick_axis(vr::VRInputValueHandle_t handle) const {
    if (!m_openvr_loaded) {
        return Vector2f{};
    }

    vr::InputAnalogActionData_t data{};
    vr::VRInput()->GetAnalogActionData(m_action_joystick, &data, sizeof(data), handle);

    const auto deadzone = m_joystick_deadzone->value();
    const auto out = Vector2f{ data.x, data.y };

    return glm::length(out) > deadzone ? out : Vector2f{};
}

Vector2f VR::get_left_stick_axis() const {
    return get_joystick_axis(m_left_joystick);
}

Vector2f VR::get_right_stick_axis() const {
    return get_joystick_axis(m_right_joystick);
}

void VR::trigger_haptic_vibration(float seconds_from_now, float duration, float frequency, float amplitude, vr::VRInputValueHandle_t source) {
    if (!m_openvr_loaded || !is_using_controllers()) {
        return;
    }

    vr::VRInput()->TriggerHapticVibrationAction(m_action_haptic, seconds_from_now, duration, frequency, amplitude, source);
}
