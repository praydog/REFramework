#pragma once

#include <unordered_set>

#include <d3d11.h>
#include <d3d12.h>
#include <dxgi.h>
#include <wrl.h>

#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_D3D11
#define XR_USE_GRAPHICS_API_D3D12
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#include <common/xr_linear.h>

#include "VRRuntime.hpp"

namespace runtimes{
struct OpenXR final : public VRRuntime {
    OpenXR() {
        this->custom_stage = SynchronizeStage::EARLY;
    }
    
    virtual ~OpenXR() {
        this->destroy();
    }

    struct Swapchain {
        XrSwapchain handle;
        int32_t width;
        int32_t height;
    };

    VRRuntime::Type type() const override { 
        return VRRuntime::Type::OPENXR;
    }

    std::string_view name() const override {
        return "OpenXR";
    }

    bool ready() const override {
        return VRRuntime::ready() && this->session_ready;
    }

    VRRuntime::Error synchronize_frame() override;
    VRRuntime::Error update_poses() override;
    VRRuntime::Error update_render_target_size() override;
    uint32_t get_width() const override;
    uint32_t get_height() const override;

    VRRuntime::Error consume_events(std::function<void(void*)> callback) override;

    VRRuntime::Error update_matrices(float nearz, float farz) override;
    VRRuntime::Error update_input() override;

    void destroy() override;

public: 
    // OpenXR specific methods
    std::string get_result_string(XrResult result) const;
    std::string get_structure_string(XrStructureType type) const;
    std::string get_path_string(XrPath path) const;
    std::string get_current_interaction_profile() const;
    XrPath get_current_interaction_profile_path() const;

    std::optional<std::string> initialize_actions(const std::string& json_string);

    XrResult begin_frame();
    XrResult end_frame();

    void begin_profile() {
        if (!this->profile_calls) {
            return;
        }

        this->profiler_start_time = std::chrono::high_resolution_clock::now();
    }

    void end_profile(std::string_view name) {
        if (!this->profile_calls) {
            return;
        }

        const auto end_time = std::chrono::high_resolution_clock::now();
        const auto dur = std::chrono::duration<float, std::milli>(end_time - this->profiler_start_time).count();

        spdlog::info("{} took {} ms", name, dur);
    }

    bool is_action_active(XrAction action, VRRuntime::Hand hand) const;
    bool is_action_active(std::string_view action_name, VRRuntime::Hand hand) const;
    bool is_action_active_once(std::string_view action_name, VRRuntime::Hand hand) const;
    std::string translate_openvr_action_name(std::string action_name) const;

    Vector2f get_left_stick_axis() const;
    Vector2f get_right_stick_axis() const;

    void trigger_haptic_vibration(float duration, float frequency, float amplitude, VRRuntime::Hand source) const;
    void display_bindings_editor();
    void save_bindings();

public: 
    // OpenXR specific fields
    float prediction_scale{0.0f};
    bool session_ready{false};
    bool frame_began{false};
    bool frame_synced{false};
    bool profile_calls{false};

    std::chrono::high_resolution_clock::time_point profiler_start_time{};

    std::recursive_mutex sync_mtx{};

    XrInstance instance{XR_NULL_HANDLE};
    XrSession session{XR_NULL_HANDLE};
    XrSpace stage_space{XR_NULL_HANDLE};
    XrSpace view_space{XR_NULL_HANDLE}; // for generating view matrices
    XrSystemId system{XR_NULL_SYSTEM_ID};
    XrFormFactor form_factor{XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY};
    XrViewConfigurationType view_config{XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO};
    XrEnvironmentBlendMode blend_mode{XR_ENVIRONMENT_BLEND_MODE_OPAQUE};
    XrViewState view_state{XR_TYPE_VIEW_STATE};
    XrViewState stage_view_state{XR_TYPE_VIEW_STATE};
    XrFrameState frame_state{XR_TYPE_FRAME_STATE};

    XrSessionState session_state{XR_SESSION_STATE_UNKNOWN};

    XrSpaceLocation view_space_location{XR_TYPE_SPACE_LOCATION};

    std::vector<XrViewConfigurationView> view_configs{};
    std::vector<Swapchain> swapchains{};
    std::vector<XrView> views{};
    std::vector<XrView> stage_views{};

    float resolution_scale{1.0f};

    struct Action {
        std::vector<XrAction> action_collection{};
    };

    struct ActionSet {
        XrActionSet handle;
        std::vector<XrAction> actions{};
        std::unordered_map<std::string, XrAction> action_map{}; // XrActions are handles so it's okay.
        std::unordered_map<XrAction, std::string> action_names{};

        std::unordered_set<XrAction> float_actions{};
        std::unordered_set<XrAction> vector2_actions{};
        std::unordered_set<XrAction> bool_actions{};
        std::unordered_set<XrAction> pose_actions{};
        std::unordered_set<XrAction> vibration_actions{};
    } action_set;

    struct HandData {
        XrSpace space{XR_NULL_HANDLE};
        XrPath path{XR_NULL_PATH};
        XrSpaceLocation location{XR_TYPE_SPACE_LOCATION};
        XrSpaceVelocity velocity{XR_TYPE_SPACE_VELOCITY};
        // interaction profile -> action -> path map
        std::unordered_map<std::string, std::unordered_map<std::string, XrPath>> path_map{};
        bool active{false};

        struct UI {
            char new_path_name[XR_MAX_PATH_LENGTH]{};
            uint32_t new_path_name_length{0};
            int action_combo_index{0};
        } ui;
    };

    std::array<HandData, 2> hands{};

public:
    struct InteractionBinding {
        std::string interaction_path_name{};
        std::string action_name{};
    };

    static inline std::vector<InteractionBinding> s_bindings_map {
        {"/user/hand/*/input/grip/pose", "pose"},
        {"/user/hand/*/input/trigger", "trigger"}, // oculus?
        {"/user/hand/*/input/squeeze", "grip"}, // oculus/vive/index
        {"/user/hand/*/input/x/click", "abutton"}, // oculus?
        {"/user/hand/*/input/y/click", "bbutton"}, // oculus?
        {"/user/hand/*/input/a/click", "abutton"}, // oculus?
        {"/user/hand/*/input/b/click", "bbutton"}, // oculus?
        {"/user/hand/*/input/thumbstick", "joystick"}, // oculus?
        {"/user/hand/*/input/thumbstick/click", "joystickclick"}, // oculus?
        {"/user/hand/*/input/system/click", "systembutton"}, // oculus/vive/index
        {"/user/hand/*/input/menu/click", "systembutton"}, // oculus/vive/index

        {"/user/hand/*/input/trackpad", "joystick"}, // vive & others
        {"/user/hand/*/input/trackpad/click", "joystickclick"}, // vive & others
        {"/user/hand/*/output/haptic", "haptic"}, // most of them

        {"/user/hand/right/input/a/click", "re3_dodge"},
        {"/user/hand/left/input/trigger", "weapondial_start"},
    };

    static inline std::vector<std::string> s_supported_controllers {
        "/interaction_profiles/khr/simple_controller",
        "/interaction_profiles/oculus/touch_controller",
        "/interaction_profiles/oculus/go_controller",
        "/interaction_profiles/valve/index_controller",
        "/interaction_profiles/microsoft/motion_controller",
        "/interaction_profiles/htc/vive_controller",
    };
};
}