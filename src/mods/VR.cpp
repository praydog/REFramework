#include <fstream>

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

#include "utility/Scan.hpp"
#include "utility/FunctionHook.hpp"
#include "utility/Module.hpp"

#include "FirstPerson.hpp"

#include "VR.hpp"

constexpr auto CONTROLLER_DEADZONE = 0.1f;

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
std::unique_ptr<FunctionHook> g_view_matrix_hook{};
std::unique_ptr<FunctionHook> g_overlay_draw_hook{};
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
        } else {
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

    if (result == nullptr || !g_framework->is_ready() || !vr->m_is_hmd_active) {
        return original_func(camera, result);
    }

    if (camera != sdk::get_primary_camera()) {
        return original_func(camera, result);
    }

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

Matrix4x4f* VR::camera_get_view_matrix_hook(REManagedObject* camera, Matrix4x4f* result) {
    auto original_func = g_view_matrix_hook->get_original<decltype(VR::camera_get_view_matrix_hook)>();

    if (result == nullptr || !g_framework->is_ready()) {
        return original_func(camera, result);
    }

    auto vr = VR::get();

    if (!vr->m_is_hmd_active) {
        return original_func(camera, result);
    }

    if (camera != sdk::get_primary_camera()) {
        return original_func(camera, result);
    }

    original_func(camera, result);

    auto& mtx = *result;

    const auto current_eye_transform = vr->get_current_eye_transform();
    //auto current_head_pos = -(glm::inverse(vr->get_rotation(0)) * ((vr->get_position(0)) - vr->m_standing_origin));
    //current_head_pos.w = 0.0f;

    /*if (vr->m_use_rotation) {
        if (vr->m_invert) {
            mtx *= glm::inverse(glm::extractMatrixRotation(current_eye_transform));
        } else {
            mtx *= glm::extractMatrixRotation(current_eye_transform);
        }
    }*/

    // Adjust the view matrix origin
    *(Vector3f*)&mtx[3] -= Vector3f { current_eye_transform[3] };

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

    if (left_axis_len > CONTROLLER_DEADZONE) {
        mod->m_last_controller_update = now;
        is_using_controller = true;

        // Override the left stick's axis values to the VR controller's values
        Vector3f axis{ left_axis.x, left_axis.y, 0.0f };
        sdk::call_object_func<void*>(lstick, "update", sdk::get_thread_context(), lstick, &axis, &axis);

        keep_button_down(app::ropeway::InputDefine::Kind::UI_L_STICK);
    }

    if (right_axis_len > CONTROLLER_DEADZONE) {
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

    mod->openvr_input_to_game(input_system);
}

void VR::overlay_draw_hook(void* layer, void* render_ctx) {
    auto original_func = g_overlay_draw_hook->get_original<decltype(VR::overlay_draw_hook)>();

    if (!g_framework->is_ready()) {
        original_func(layer, render_ctx);
        return;
    }

    // just don't render anything at all.
    // overlays just seem to break stuff in VR.
    auto mod = VR::get();

    if (!mod->m_is_hmd_active) {
        original_func(layer, render_ctx);
        return;
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

    if (openvr_error) {
        return openvr_error;
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

    hijack_error = hijack_overlay_renderer();

    if (hijack_error) {
        return hijack_error;
    }

    // all OK
    return Mod::on_initialize();
}

std::optional<std::string> VR::initialize_openvr() {
    auto error = vr::VRInitError_None;
	m_hmd = vr::VR_Init(&error, vr::VRApplication_Scene);

    // check if error
    if (error != vr::VRInitError_None) {
        return "VR_Init failed: " + std::string{vr::VR_GetVRInitErrorAsEnglishDescription(error)};
    }

    if (m_hmd == nullptr) {
        return "VR_Init failed: HMD is null";
    }

    // get render target size
    m_hmd->GetRecommendedRenderTargetSize(&m_w, &m_h);

    if (!vr::VRCompositor()) {
        return "VRCompositor failed to initialize.";
    }

    auto input_error = initialize_openvr_input();

    if (input_error) {
        return input_error;
    }

    return std::nullopt;
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

    // get action handles
    auto trigger_error = vr::VRInput()->GetActionHandle("/actions/default/in/Trigger", &m_action_trigger);
    auto grip_error = vr::VRInput()->GetActionHandle("/actions/default/in/Grip", &m_action_grip);

    if (trigger_error != vr::VRInputError_None || grip_error != vr::VRInputError_None) {
        return "VRInput failed to get action handles: " + std::to_string((uint32_t)trigger_error) + " " + std::to_string((uint32_t)grip_error);
    }

    if (m_action_trigger == vr::k_ulInvalidActionHandle || m_action_grip == vr::k_ulInvalidActionHandle) {
        return "VRInput failed to get action handles.";
    }

    auto joystick_error = vr::VRInput()->GetActionHandle("/actions/default/in/Joystick", &m_action_joystick);

    if (joystick_error != vr::VRInputError_None) {
        return "VRInput failed to get action handles (Joystick): " + std::to_string((uint32_t)joystick_error);
    }

    auto joystick_click_error = vr::VRInput()->GetActionHandle("/actions/default/in/JoystickClick", &m_action_joystick_click);

    if (joystick_click_error != vr::VRInputError_None) {
        return "VRInput failed to get action handles (JoystickClick): " + std::to_string((uint32_t)joystick_click_error);
    }

    auto a_button_error = vr::VRInput()->GetActionHandle("/actions/default/in/AButton", &m_action_a_button);

    if (a_button_error != vr::VRInputError_None) {
        return "VRInput failed to get action handles (AButton): " + std::to_string((uint32_t)a_button_error);
    }

    auto b_button_error = vr::VRInput()->GetActionHandle("/actions/default/in/BButton", &m_action_b_button);

    if (b_button_error != vr::VRInputError_None) {
        return "VRInput failed to get action handles (BButton): " + std::to_string((uint32_t)b_button_error);
    }

    m_active_action_set.ulActionSet = m_action_set;
    m_active_action_set.ulRestrictedToDevice = vr::k_ulInvalidInputValueHandle;
    m_active_action_set.nPriority = 0;

    detect_controllers();

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

std::optional<std::string> VR::hijack_overlay_renderer() {
    // We're going to make via.render.layer.Overlay.Draw() return early
    // For some reason this fixes 3D GUI rendering in RE3 in VR
    auto t = sdk::RETypeDB::get()->find_type("via.render.layer.Overlay");

    if (t == nullptr) {
        return "VR init failed: via.render.layer.Overlay type not found.";
    }

    void* fake_obj = t->create_instance();

    if (fake_obj == nullptr) {
        return "VR init failed: Failed to create fake via.render.layer.Overlay instance.";
    }

    auto obj_vtable = *(uintptr_t**)fake_obj;

    if (obj_vtable == nullptr) {
        return "VR init failed: via.render.layer.Overlay vtable not found.";
    }

    spdlog::info("via.render.layer.Overlay vtable: {:x}", (uintptr_t)obj_vtable - g_framework->get_module());

    auto draw_native = obj_vtable[sdk::renderer::RenderLayer::DRAW_VTABLE_INDEX];

    if (draw_native == 0) {
        return "VR init failed: via.render.layer.Overlay draw native not found.";
    }

    spdlog::info("via.render.layer.Overlay.Draw: {:x}", (uintptr_t)draw_native);

    // Set the first byte to the ret instruction
    //m_overlay_draw_patch = Patch::create(draw_native, { 0xC3 });

    g_overlay_draw_hook = std::make_unique<FunctionHook>(draw_native, overlay_draw_hook);

    if (!g_overlay_draw_hook->create()) {
        return "VR init failed: via.render.layer.Overlay draw native function hook failed.";
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
    /*t = sdk::RETypeDB::get()->find_type("via.render.layer.PostShadowCast");

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

void VR::update_hmd_state() {
    const auto start_time = std::chrono::high_resolution_clock::now();

    auto error = vr::VRInput()->UpdateActionState(&m_active_action_set, sizeof(m_active_action_set), 1);

    if (error != vr::VRInputError_None) {
        spdlog::error("VRInput failed to update action state: {}", (uint32_t)error);
    }

    const auto end_time = std::chrono::high_resolution_clock::now();

    if ((end_time - start_time) >= std::chrono::milliseconds(30)) {
        spdlog::warn("VRInput update action state took too long: {}ms", std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count());
        spdlog::info("Reinitializing openvr input");

        // Reinitialize openvr input, hopefully this fixes the issue
        m_controllers.clear();
        m_controllers_set.clear();

        auto input_error = initialize_openvr_input();

        if (input_error) {
            spdlog::error("Failed to reinitialize openvr input: {}", *input_error);
        }
    }

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

void VR::update_camera() {
    if (!m_is_hmd_active) {
        m_needs_camera_restore = false;
        return;
    }

    if (inside_on_end) {
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
        return;
    }

    update_camera_origin();

    auto projection_matrix = get_current_projection_matrix(true);

    // Steps towards getting lens flares and volumetric lighting working
    // Get the FOV from the projection matrix
    const auto vfov = glm::degrees(2.0f * std::atan(1.0f / projection_matrix[1][1]));
    const auto aspect = projection_matrix[1][1] / projection_matrix[0][0];
    const auto hfov = vfov * aspect;
    
    //spdlog::info("vFOV: {}", vfov);
    //spdlog::info("Aspect: {}", aspect);

    sdk::call_object_func<void*>(camera, "set_FOV", sdk::get_thread_context(), camera, vfov);
    sdk::call_object_func<void*>(camera, "set_VerticalEnable", sdk::get_thread_context(), camera, true);
    sdk::call_object_func<void*>(camera, "set_AspectRatio", sdk::get_thread_context(), camera, aspect);

    m_nearz = sdk::call_object_func<float>(camera, "get_NearClipPlane", sdk::get_thread_context(), camera);
    m_farz = sdk::call_object_func<float>(camera, "get_FarClipPlane", sdk::get_thread_context(), camera);

    // Disable lens distortion
    set_lens_distortion(false);
    // Disable certain effects like the 3D overlay during the sewer gators
    // section in RE7
    disable_bad_effects();

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
    }
    
    const auto current_hmd_rotation = glm::quat{get_rotation(0)};
    const auto new_rotation = m_original_camera_rotation * current_hmd_rotation;
    
    const auto current_relative_eye_transform = get_current_eye_transform(false);
    const auto current_relative_eye_pos = current_hmd_rotation * current_relative_eye_transform[3];

    auto current_relative_pos = (get_position(0) - m_standing_origin) /*+ current_relative_eye_pos*/;
    current_relative_pos.w = 0.0f;

    auto current_head_pos = m_original_camera_rotation * current_relative_pos;

    sdk::set_joint_rotation(camera_joint, new_rotation);
    sdk::set_joint_position(camera_joint, m_original_camera_position + current_head_pos);
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
#ifdef RE7
    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    static auto lens_distortion_tdef = sdk::RETypeDB::get()->find_type(game_namespace("LensDistortionController"));
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
        auto tdef = sdk::RETypeDB::get()->find_type(name);
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

int32_t VR::get_frame_count() const {
    return get_game_frame_count();
}

int32_t VR::get_game_frame_count() const {
    static auto renderer_type = sdk::RETypeDB::get()->find_type("via.render.Renderer");

    if (renderer_type == nullptr) {
        renderer_type = sdk::RETypeDB::get()->find_type("via.render.Renderer");
        spdlog::warn("VR: Failed to find renderer type, trying again next time");
        return 0;
    }

    auto renderer = renderer_type->get_instance();

    if (renderer == nullptr) {
        return 0;
    }

    return sdk::call_object_func<int32_t>(renderer, renderer_type, "get_RenderFrame", sdk::get_thread_context(), renderer);
}

float VR::get_standing_height() {
    std::shared_lock _{ m_pose_mtx };

    return m_standing_origin.y;
}

Vector4f VR::get_standing_origin() {
    std::shared_lock _{ m_pose_mtx };

    return m_standing_origin;
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

Matrix4x4f VR::get_current_rotation_offset() {
    if (!m_is_hmd_active) {
        return glm::identity<Matrix4x4f>();
    }

    return glm::extractMatrixRotation(get_current_eye_transform());
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

void VR::on_post_frame() {
    if (m_wgp_initialized) {
        const auto hmd_activity = m_hmd->GetTrackedDeviceActivityLevel(vr::k_unTrackedDeviceIndex_Hmd);
        m_is_hmd_active = hmd_activity == vr::k_EDeviceActivityLevel_UserInteraction || hmd_activity == vr::k_EDeviceActivityLevel_UserInteraction_Timeout;

        if (!m_is_hmd_active) {
            return;
        }
    } else {
        m_is_hmd_active = true; // We need to force out an initial WaitGetPoses call
    }

    if (m_frame_count == m_last_frame_count) {
        return;
    }

    m_main_view = sdk::get_main_view();
    m_submitted = false;

    const auto renderer = g_framework->get_renderer_type();

    if (renderer == REFramework::RendererType::D3D11) {
        m_d3d11.on_frame(this);
    } else if (renderer == REFramework::RendererType::D3D12) {
        m_d3d12.on_frame(this);
    }
}

void VR::on_post_present() {
    if (!m_is_hmd_active) {
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

    m_last_frame_count = m_frame_count;

    if (m_submitted || m_needs_wgp_update) {
        if (m_submitted) {
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

void VR::on_pre_gui_draw_element(REComponent* gui_element, void* primitive_context) {
    inside_gui_draw = true;

    if (!m_is_hmd_active) {
        return;
    }

    auto game_object = utility::re_component::get_game_object(gui_element);

    if (game_object != nullptr && game_object->transform != nullptr) {
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
        case "GenomeCodexGUI"_fnv:
            return;

        default:
            break;
        };

        //spdlog::info("VR: on_pre_gui_draw_element: {}", name);
        //spdlog::info("VR: on_pre_gui_draw_element: {} {:x}", name, (uintptr_t)game_object);

        auto view = sdk::call_object_func<REComponent*>(gui_element, "get_View", sdk::get_thread_context(), gui_element);

        if (view != nullptr) {
            const auto current_view_type = sdk::call_object_func<uint32_t>(view, "get_ViewType", sdk::get_thread_context(), view);

            if (current_view_type == (uint32_t)via::gui::ViewType::Screen) {
                auto& restore_data = g_elements_to_reset.emplace_back(std::make_unique<GUIRestoreData>());

                restore_data->element = gui_element;
                restore_data->view = view;
                restore_data->original_position = game_object->transform->worldTransform[3];
                restore_data->overlay = sdk::call_object_func<bool>(view, "get_Overlay", sdk::get_thread_context(), view);
                restore_data->detonemap = sdk::call_object_func<bool>(view, "get_Detonemap", sdk::get_thread_context(), view);
                restore_data->view_type = (via::gui::ViewType)current_view_type;

                // Set view type to world
                sdk::call_object_func<void*>(view, "set_ViewType", sdk::get_thread_context(), view, (uint32_t)via::gui::ViewType::World);

                // Set overlay = true (fixes double vision in VR)
                sdk::call_object_func<void*>(view, "set_Overlay", sdk::get_thread_context(), view, true);

                // Set detonemap = true (fixes weird tint)
                sdk::call_object_func<void*>(view, "set_Detonemap", sdk::get_thread_context(), view, true);

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
                        auto camera_transform = camera_object->transform;

                        const auto& camera_matrix = utility::re_transform::get_joint_matrix_by_index(*camera_transform, 0);
                        const auto& camera_position = camera_matrix[3];
                        
                        auto& gui_matrix = game_object->transform->worldTransform;

                        gui_matrix = glm::extractMatrixRotation(camera_matrix) * Matrix4x4f {
                            -1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, -1, 0,
                            0, 0, 0, 1
                        };
                        
                        gui_matrix[3] = camera_position + (-camera_matrix[2] * m_ui_scale);
                        gui_matrix[3].w = 1.0f;

                        auto child = sdk::call_object_func<REManagedObject*>(view, "get_Child", sdk::get_thread_context(), view);

                        if (child && utility::re_managed_object::get_field<wchar_t*>(child, "Name") == std::wstring_view(L"c_interact")) {
                            /*auto math = sdk::get_native_singleton("via.math");
                            auto math_t = sdk::RETypeDB::get()->find_type("via.math");

                            Vector4f current_2d_pos{};
                            sdk::call_object_func<Vector4f*>(child, "get_Position", &current_2d_pos, sdk::get_thread_context(), child);

                            spdlog::info("{} {}", current_2d_pos.x, current_2d_pos.y);

                            // Translate 2d position to 3d position (screen to world)
                            Matrix4x4f proj{}, view{};
                            float screen_size[2]{};
                            camera_get_projection_matrix_hook(camera, &proj);
                            camera_get_view_matrix_hook(camera, &view);
                            get_size_hook(sdk::get_main_view(), (float*)&screen_size);

                            float rect[4]{};

                            rect[2] = screen_size[0];
                            rect[3] = screen_size[1];

                            Vector4f pos{};
                            sdk::call_object_func<Vector4f*>(math, math_t, "screenPos2WorldPos", &pos, sdk::get_thread_context(), &current_2d_pos, &proj, &view, &rect);

                            const auto dir = glm::normalize(pos - camera_position);*/

                            auto world_pos_attach_comp = utility::re_component::find(game_object->transform, "app.UIWorldPosAttach");

                            // Fix the world position of the gui element
                            if (world_pos_attach_comp != nullptr) {
                                const auto& target_pos = *sdk::get_object_field<Vector4f>(world_pos_attach_comp, "_NowTargetPos");
                                auto dir = glm::normalize(target_pos - camera_position);

                                dir.w = 0.0f;
                                gui_matrix[3] = camera_position + (dir * m_ui_scale);
                                gui_matrix[3].w = 1.0f;

                                regenny::via::Size gui_size{};
                                sdk::call_object_func<void*>(view, "get_ScreenSize", &gui_size, sdk::get_thread_context(), view);

                                Vector3f half_size{ gui_size.w / 2.0f, gui_size.h / 2.0f, 0.0f };
                                sdk::call_object_func<void*>(child, "set_Position", sdk::get_thread_context(), child, &half_size);

                                //spdlog::info("c_interact");
                            }
                        }
                    }
                }
            }
        }
    } else {
        spdlog::info("VR: on_pre_gui_draw_element: nullptr gameobject");
    }
}

void VR::on_gui_draw_element(REComponent* gui_element, void* primitive_context) {
    //spdlog::info("VR: on_gui_draw_element");

    // Restore elements back to original states
    for (auto& data : g_elements_to_reset) {
        sdk::call_object_func<void*>(data->view, "set_ViewType", sdk::get_thread_context(), data->view, (uint32_t)via::gui::ViewType::Screen);
        sdk::call_object_func<void*>(data->view, "set_Overlay", sdk::get_thread_context(), data->view, data->overlay);
        sdk::call_object_func<void*>(data->view, "set_Detonemap", sdk::get_thread_context(), data->view, data->detonemap);
        
        auto game_object = utility::re_component::get_game_object(data->element);

        if (game_object != nullptr && game_object->transform != nullptr) {
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

    /*static auto transparent_layer_t = sdk::RETypeDB::get()->find_type("via.render.layer.Transparent");
    auto transparent_layer = sdk::renderer::find_layer(transparent_layer_t->type);

    spdlog::info("transparent layer: {:x}", (uintptr_t)transparent_layer);

    if (transparent_layer == nullptr) {
        return;
    }

    static auto scene_layer_t = sdk::RETypeDB::get()->find_type("via.render.layer.Scene");
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
    m_in_render = true;
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

    if (!inside_on_end && m_frame_count % 2 == m_left_eye_interval) {
        update_camera();
    } else if (inside_on_end) {
        update_camera_origin();
    }
}

void VR::on_begin_rendering(void* entry) {
    //spdlog::info("BeginRendering");
}

void VR::on_pre_end_rendering(void* entry) {

    //spdlog::info("EndRendering");
}

void VR::on_end_rendering(void* entry) {
    if ((!m_is_hmd_active || m_needs_wgp_update) && !inside_on_end) {
        restore_camera();

        inside_on_end = false;
        m_in_render = false;
        return;
    }

    if (m_use_afr || inside_on_end) {
        if (m_use_afr) {
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
    switch (hash) {
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
    switch (hash) {
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

void VR::openvr_input_to_game(REManagedObject* input_system) {
    // Get OpenVR input system
    auto openvr_input = vr::VRInput();

    if (openvr_input == nullptr) {
        spdlog::error("[VR] Failed to get OpenVR input system.");
        return;
    }

    auto lstick = sdk::call_object_func<REManagedObject*>(input_system, "get_LStick", sdk::get_thread_context());
    auto rstick = sdk::call_object_func<REManagedObject*>(input_system, "get_RStick", sdk::get_thread_context());

    if (lstick == nullptr || rstick == nullptr) {
        return;
    }

    auto button_bits_obj = sdk::call_object_func<REManagedObject*>(input_system, "get_ButtonBits", sdk::get_thread_context(), input_system);
    //auto button_bits_obj = utility::re_managed_object::get_field<REManagedObject*>(input_system, "ButtonBits");

    if (button_bits_obj == nullptr) {
        return;
    }

    auto input_unit_obj = sdk::call_object_func<REManagedObject*>(input_system, "getActiveUserInputUnit", sdk::get_thread_context(), input_system);

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
    set_button_state(app::ropeway::InputDefine::Kind::SUPPORT_HOLD, is_left_grip_down);
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
    set_button_state(app::ropeway::InputDefine::Kind::RESET_CAMERA, is_right_joystick_click_down);

    // Left B: Inventory, PRESS_START
    set_button_state(app::ropeway::InputDefine::Kind::INVENTORY, is_left_b_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::PRESS_START, is_left_b_button_down);

    // Left A: QUICK_TURN, PRESS_START, CANCEL, DIALOG_CANCEL
    set_button_state(app::ropeway::InputDefine::Kind::QUICK_TURN, is_left_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::PRESS_START, is_left_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::CANCEL, is_left_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::DIALOG_CANCEL, is_left_a_button_down);
    
    // Right A: Action, ITEM, PRESS_START, DECIDE, DIALOG_DECIDE, (1 << 51)
    set_button_state(app::ropeway::InputDefine::Kind::ACTION, is_right_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::ITEM, is_right_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::PRESS_START, is_right_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::DECIDE, is_right_a_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::DIALOG_DECIDE, is_right_a_button_down);
    set_button_state((app::ropeway::InputDefine::Kind)((uint64_t)1 << 51), is_right_a_button_down);
    
    // Right B: Reload, Skip Event, UI_EXCHANGE, UI_RESET, (1 << 52) (that one is RE3 only? don't see it in the enum)
    set_button_state(app::ropeway::InputDefine::Kind::RELOAD, is_right_b_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::SKIP_EVENT, is_right_b_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::UI_EXCHANGE, is_right_b_button_down);
    set_button_state(app::ropeway::InputDefine::Kind::UI_RESET, is_right_b_button_down);
    set_button_state((app::ropeway::InputDefine::Kind)((uint64_t)1 << 52), is_right_b_button_down);

    // Fixes QTE bound to triggers
    if (is_using_controller) {
        const auto left_trigger_state = is_left_trigger_down ? 1.0f : 0.0f;
        const auto right_trigger_state = is_trigger_down ? 1.0f : 0.0f;
        sdk::call_object_func<void*>(input_unit_obj, "set__AnalogL", sdk::get_thread_context(), input_unit_obj, left_trigger_state);
        sdk::call_object_func<void*>(input_unit_obj, "set__AnalogR", sdk::get_thread_context(), input_unit_obj, right_trigger_state);
    }

    const auto left_axis = get_left_stick_axis();
    const auto right_axis = get_right_stick_axis();
    const auto left_axis_len = glm::length(left_axis);
    const auto right_axis_len = glm::length(right_axis);

    bool moved_sticks = false;

    if (left_axis_len > CONTROLLER_DEADZONE) {
        moved_sticks = true;

        // Override the left stick's axis values to the VR controller's values
        Vector3f axis{ left_axis.x, left_axis.y, 0.0f };
        sdk::call_object_func<void*>(lstick, "update", sdk::get_thread_context(), lstick, &axis, &axis);
    }

    if (right_axis_len > CONTROLLER_DEADZONE) {
        moved_sticks = true;

        // Override the right stick's axis values to the VR controller's values
        Vector3f axis{ right_axis.x, right_axis.y, 0.0f };
        sdk::call_object_func<void*>(rstick, "update", sdk::get_thread_context(), rstick, &axis, &axis);
    }

    if (moved_sticks) {
        auto new_pos = get_position(vr::k_unTrackedDeviceIndex_Hmd);

        new_pos.y = m_standing_origin.y;
        // Don't set the Y because it would look really strange
        m_standing_origin = glm::lerp(m_standing_origin, new_pos, 1 / 90.0f);
    }

    set_button_state(app::ropeway::InputDefine::Kind::MOVE, left_axis_len > CONTROLLER_DEADZONE);
    set_button_state(app::ropeway::InputDefine::Kind::UI_L_STICK, left_axis_len > CONTROLLER_DEADZONE);

    set_button_state(app::ropeway::InputDefine::Kind::WATCH, right_axis_len > CONTROLLER_DEADZONE);
    set_button_state(app::ropeway::InputDefine::Kind::UI_R_STICK, right_axis_len > CONTROLLER_DEADZONE);
    //set_button_state(app::ropeway::InputDefine::Kind::RUN, right_axis_len > 0.01f);

    // Causes the right stick to take effect properly
    if (is_using_controller) {
        sdk::call_object_func<void*>(input_system, "set_InputMode", sdk::get_thread_context(), input_system, app::ropeway::InputDefine::InputMode::Pad);
    }
}

void VR::on_draw_ui() {
    // create VR tree entry in menu (imgui)
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);

    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    // draw VR tree entry in menu (imgui)
    ImGui::Text("VR");
    ImGui::Separator();
    ImGui::Text("Recommended render target size: %d x %d", m_w, m_h);
    ImGui::Separator();
    ImGui::DragFloat4("Raw Left", (float*)&m_raw_projections[0], 0.01f, -100.0f, 100.0f);
    ImGui::DragFloat4("Raw Right", (float*)&m_raw_projections[1], 0.01f, -100.0f, 100.0f);
    ImGui::Separator();

    if (ImGui::Button("Set Standing Height")) {
        m_standing_origin.y = get_position(0).y;
    }

    if (ImGui::Button("Set Standing Origin")) {
        m_standing_origin = get_position(0);
    }

    ImGui::DragFloat4("Right Bounds", (float*)&m_right_bounds, 0.005f, -2.0f, 2.0f);
    ImGui::DragFloat4("Left Bounds", (float*)&m_left_bounds, 0.005f, -2.0f, 2.0f);

    if (ImGui::Checkbox("Use AFR", &m_use_afr)) {
    }

    ImGui::Checkbox("Use Predicted Poses", &m_use_predicted_poses);
    ImGui::Checkbox("Use Eye Rotation", &m_use_rotation);
    ImGui::Checkbox("Invert Eye Rotation", &m_invert);
    ImGui::DragFloat("UI Scale", &m_ui_scale, 0.005f, 0.0f, 100.0f);
}

void VR::on_device_reset() {
    spdlog::info("VR: on_device_reset");
    m_d3d11.on_reset(this);
    m_d3d12.on_reset(this);
}

void VR::on_config_load(const utility::Config& cfg) {
    
}

void VR::on_config_save(utility::Config& cfg) {
    
}

Vector4f VR::get_position(uint32_t index) {
    if (index >= vr::k_unMaxTrackedDeviceCount) {
        return Vector4f{};
    }

    std::shared_lock _{ m_pose_mtx };

    return get_position_unsafe(index);
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

Matrix4x4f VR::get_rotation(uint32_t index) {
    if (index >= vr::k_unMaxTrackedDeviceCount) {
        return glm::identity<Matrix4x4f>();
    }

    std::shared_lock _{ m_pose_mtx };

    auto& pose = get_poses()[index];
    auto matrix = Matrix4x4f{ *(Matrix3x4f*)&pose.mDeviceToAbsoluteTracking };
    return glm::extractMatrixRotation(glm::rowMajor4(matrix));
}

bool VR::is_action_active(vr::VRActionHandle_t action, vr::VRInputValueHandle_t source) const {
    vr::InputDigitalActionData_t data{};
	vr::VRInput()->GetDigitalActionData(action, &data, sizeof(data), source);

    return data.bActive && data.bState;
}

Vector2f VR::get_joystick_axis(vr::VRInputValueHandle_t handle) const {
    vr::InputAnalogActionData_t data{};
    vr::VRInput()->GetAnalogActionData(m_action_joystick, &data, sizeof(data), handle);

    return Vector2f{ data.x, data.y };
}

Vector2f VR::get_left_stick_axis() const {
    return get_joystick_axis(m_left_joystick);
}

Vector2f VR::get_right_stick_axis() const {
    return get_joystick_axis(m_right_joystick);
}
