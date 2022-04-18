#pragma once

#include <chrono>
#include <bitset>
#include <memory>
#include <shared_mutex>

#include <openvr.h>

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


#include "utility/Patch.hpp"
#include "sdk/Math.hpp"
#include "sdk/helpers/NativeObject.hpp"
#include "sdk/Renderer.hpp"
#include "vr/D3D11Component.hpp"
#include "vr/D3D12Component.hpp"
#include "vr/OverlayComponent.hpp"

#include "Mod.hpp"

class REManagedObject;

struct VRRuntime {
    enum class Error : uint64_t {
        SUCCESS = 0,
        // rest of the error codes will be from the specific VR runtime
    };

    enum class Type : uint8_t {
        NONE,
        OPENXR,
        OPENVR,
        OCULUS, // Not implemented
    };

    enum class Eye : uint8_t {
        LEFT,
        RIGHT,
    };

    enum class SynchronizeStage : uint8_t { 
        EARLY,
        LATE
    };

    virtual ~VRRuntime() {};

    virtual std::string_view name() const {
        return "NONE";
    }

    virtual bool ready() const {
        return this->loaded;
    }

    virtual Type type() const { 
        return Type::NONE;
    }

    virtual Error synchronize_frame() {
        return Error::SUCCESS;
    }

    virtual Error update_poses() {
        return Error::SUCCESS;
    }

    virtual Error update_render_target_size() {
        return Error::SUCCESS;
    }

    virtual Error consume_events(std::function<void(void*)> callback) {
        return Error::SUCCESS;
    }

    virtual uint32_t get_width() const {
        return 0;
    }

    virtual uint32_t get_height() const {
        return 0;
    }

    virtual Error update_matrices(float nearz, float farz) {
        return Error::SUCCESS;
    }

    virtual SynchronizeStage get_synchronize_stage() const {
        return SynchronizeStage::EARLY;
    }

    bool is_openxr() const {
        return this->type() == Type::OPENXR;
    }

    bool is_openvr() const {
        return this->type() == Type::OPENVR;
    }

    bool is_oculus() const {
        return this->type() == Type::OCULUS;
    }

    bool loaded{false};
    bool wants_reinitialize{false};
    bool dll_missing{false};

    // in the case of OpenVR we always need at least one initial WaitGetPoses before the game will render
    // even if we don't have anything to submit yet, otherwise the compositor
    // will return VRCompositorError_DoNotHaveFocus
    bool needs_pose_update{true};
    bool got_first_poses{false};
    bool handle_pause{false};
    bool wants_reset_origin{true};

    std::optional<std::string> error{};

    std::array<Matrix4x4f, 2> projections{};
    std::array<Matrix4x4f, 2> eyes{};

    std::shared_mutex projections_mtx{};
    std::shared_mutex eyes_mtx{};

    Vector4f raw_projections[2]{};
};

class VR : public Mod {
public:
    static std::shared_ptr<VR>& get();

    std::string_view get_name() const override { return "VR"; }

    // Called when the mod is initialized
    std::optional<std::string> on_initialize() override;

    void on_lua_state_created(sol::state& lua) override;

    void on_pre_imgui_frame() override;
    void on_present() override;
    void on_post_present() override;
    void on_update_transform(RETransform* transform) override;
    void on_update_camera_controller(RopewayPlayerCameraController* controller) override;
    bool on_pre_gui_draw_element(REComponent* gui_element, void* primitive_context) override;
    void on_gui_draw_element(REComponent* gui_element, void* primitive_context) override;
    void on_pre_update_before_lock_scene(void* ctx) override;
    void on_pre_lightshaft_draw(void* shaft, void* render_context) override;
    void on_lightshaft_draw(void* shaft, void* render_context) override;

    void on_pre_application_entry(void* entry, const char* name, size_t hash) override;
    void on_application_entry(void* entry, const char* name, size_t hash) override;

    void on_draw_ui() override;
    void on_device_reset() override;

    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    // Application entries
    void on_pre_update_hid(void* entry);
    void on_update_hid(void* entry);
    void on_pre_begin_rendering(void* entry);
    void on_begin_rendering(void* entry);
    void on_pre_end_rendering(void* entry);
    void on_end_rendering(void* entry);
    void on_pre_wait_rendering(void* entry);
    void on_wait_rendering(void* entry);

    template<typename T = VRRuntime>
    T* get_runtime() const {
        if (m_openvr.loaded) {
            return (T*)&m_openvr;
        } else if (m_openxr.loaded) {
            return (T*)&m_openxr;
        }
        
        return (T*)&m_null_runtime;
    }

    auto get_hmd() const {
        return m_openvr.hmd;
    }

    auto& get_openvr_poses() const {
        return m_openvr.render_poses;
    }

    auto get_hmd_width() const {
        return get_runtime()->get_width();
    }

    auto get_hmd_height() const {
        return get_runtime()->get_height();
    }

    auto get_last_controller_update() const {
        return m_last_controller_update;
    }

    int32_t get_frame_count() const;
    int32_t get_game_frame_count() const;

    bool is_using_afr() const {
        return m_use_afr->value();
    }

    // Functions that generally use a mutex or have more complex logic
    float get_standing_height();
    Vector4f get_standing_origin();
    void set_standing_origin(const Vector4f& origin);

    glm::quat get_rotation_offset();
    void set_rotation_offset(const glm::quat& offset);
    void recenter_view();

    glm::quat get_gui_rotation_offset();
    void set_gui_rotation_offset(const glm::quat& offset);
    void recenter_gui(const glm::quat& from);

    Vector4f get_current_offset();

    Matrix4x4f get_current_eye_transform(bool flip = false);
    Matrix4x4f get_current_projection_matrix(bool flip = false);

    auto& get_controllers() const {
        return m_controllers;
    }

    bool is_using_controllers() const {
        return !m_controllers.empty() && (std::chrono::steady_clock::now() - m_last_controller_update) <= std::chrono::seconds((int32_t)m_motion_controls_inactivity_timer->value());
    }

    bool is_hmd_active() const {
        return get_runtime()->ready();
    }
    
    bool is_openvr_loaded() const {
        return m_openvr.loaded;
    }

    bool is_openxr_loaded() const {
        return m_openxr.loaded;
    }

    bool is_using_hmd_oriented_audio() {
        return m_hmd_oriented_audio->value();
    }

    void toggle_hmd_oriented_audio() {
        m_hmd_oriented_audio->toggle();
    }

    const Matrix4x4f& get_last_render_matrix() {
        return m_render_camera_matrix;
    }

    Vector4f get_position(uint32_t index);
    Vector4f get_velocity(uint32_t index);
    Vector4f get_angular_velocity(uint32_t index);
    Matrix4x4f get_rotation(uint32_t index);
    Matrix4x4f get_transform(uint32_t index);
    vr::HmdMatrix34_t get_raw_transform(uint32_t index);

    auto& get_pose_mutex() {
        return m_pose_mtx;
    }

    const auto& get_eyes() const {
        return get_runtime()->eyes;
    }
    
    bool is_action_active(vr::VRActionHandle_t action, vr::VRInputValueHandle_t source = vr::k_ulInvalidInputValueHandle) const;
    Vector2f get_joystick_axis(vr::VRInputValueHandle_t handle) const;

    Vector2f get_left_stick_axis() const;
    Vector2f get_right_stick_axis() const;

    void trigger_haptic_vibration(float seconds_from_now, float duration, float frequency, float amplitude, vr::VRInputValueHandle_t source = vr::k_ulInvalidInputValueHandle);
    
    auto get_action_set() const { return m_action_set; }
    auto& get_active_action_set() const { return m_active_action_set; }
    auto get_action_trigger() const { return m_action_trigger; }
    auto get_action_grip() const { return m_action_grip; }
    auto get_action_joystick() const { return m_action_joystick; }
    auto get_action_joystick_click() const { return m_action_joystick_click; }
    auto get_action_a_button() const { return m_action_a_button; }
    auto get_action_b_button() const { return m_action_b_button; }
    auto get_action_weapon_dial() const { return m_action_weapon_dial; }
    auto get_action_minimap() const { return m_action_minimap; }
    auto get_action_block() const { return m_action_block; }
    auto get_action_dpad_up() const { return m_action_dpad_up; }
    auto get_action_dpad_down() const { return m_action_dpad_down; }
    auto get_action_dpad_left() const { return m_action_dpad_left; }
    auto get_action_dpad_right() const { return m_action_dpad_right; }
    auto get_action_heal() const { return m_action_heal; }
    auto get_left_joystick() const { return m_left_joystick; }
    auto get_right_joystick() const { return m_right_joystick; }

    const auto& get_action_handles() const { return m_action_handles;}

    auto get_ui_scale() const { return m_ui_scale_option->value(); }
    const auto& get_raw_projections() const { return get_runtime()->raw_projections; }

    template<typename T=sdk::renderer::RenderLayer>
    struct RenderLayerHook {
        RenderLayerHook() = delete;
        RenderLayerHook(std::string_view name)
            : name{name}
        {

        }

        static void draw(T* layer, void* render_context);
        static void update(T* layer, void* render_context);

        std::unique_ptr<FunctionHook> draw_hook{};
        std::unique_ptr<FunctionHook> update_hook{};
        std::string name{};

        virtual bool hook_draw(Address target) {
            draw_hook = std::make_unique<FunctionHook>(target, &RenderLayerHook<T>::draw);
            return draw_hook->create();
        }

        virtual bool hook_update(Address target) {
            update_hook = std::make_unique<FunctionHook>(target, &RenderLayerHook<T>::update);
            return update_hook->create();
        }

        operator RenderLayerHook<sdk::renderer::RenderLayer>&() {
            return *(RenderLayerHook<sdk::renderer::RenderLayer>*)this;
        }
    };

private:
    Vector4f get_position_unsafe(uint32_t index);
    Vector4f get_velocity_unsafe(uint32_t index);
    Vector4f get_angular_velocity_unsafe(uint32_t index);

private:
    // Hooks
    static float* get_size_hook(REManagedObject* scene_view, float* result);
    static void inputsystem_update_hook(void* ctx, REManagedObject* input_system);
    static Matrix4x4f* camera_get_projection_matrix_hook(REManagedObject* camera, Matrix4x4f* result);
    static Matrix4x4f* gui_camera_get_projection_matrix_hook(REManagedObject* camera, Matrix4x4f* result);
    static Matrix4x4f* camera_get_view_matrix_hook(REManagedObject* camera, Matrix4x4f* result);
    static void overlay_draw_hook(sdk::renderer::RenderLayer* layer, void* render_context);
    static void post_effect_draw_hook(sdk::renderer::RenderLayer* layer, void* render_context);
    static void wwise_listener_update_hook(void* listener);

    //static float get_sharpness_hook(void* tonemapping);

    // initialization functions
    std::optional<std::string> initialize_openvr();
    std::optional<std::string> initialize_openvr_input();
    std::optional<std::string> initialize_openxr();
    std::optional<std::string> hijack_resolution();
    std::optional<std::string> hijack_input();
    std::optional<std::string> hijack_camera();
    std::optional<std::string> hijack_render_layer(VR::RenderLayerHook<sdk::renderer::RenderLayer>& hook);
    std::optional<std::string> hijack_wwise_listeners(); // audio hook

    std::optional<std::string> reinitialize_openvr() {
        spdlog::info("Reinitializing openvr");

        vr::VR_Shutdown();

        // Reinitialize openvr input, hopefully this fixes the issue
        m_controllers.clear();
        m_controllers_set.clear();

        auto input_error = initialize_openvr();

        if (input_error) {
            spdlog::error("Failed to reinitialize openvr: {}", *input_error);
        }

        return input_error;
    }

    bool detect_controllers();
    bool is_any_action_down();
    void update_hmd_state();
    void update_action_states();
    void update_camera(); // if not in firstperson mode
    void update_camera_origin(); // every frame
    void apply_hmd_transform(glm::quat& rotation, Vector4f& position);
    void apply_hmd_transform(::REJoint* camera_joint);
    void update_audio_camera();
    void update_render_matrix();
    void restore_audio_camera(); // after wwise listener update
    void restore_camera(); // After rendering
    void set_lens_distortion(bool value);
    void disable_bad_effects();
    void fix_temporal_effects();

    // input functions
    // Purpose: "Emulate" OpenVR input to the game
    // By setting things like input flags based on controller state
    void openvr_input_to_re2_re3(REManagedObject* input_system);
    void openvr_input_to_re_engine(); // generic, can be used on any game

    // Sets overlay layer to return instantly
    // causes world-space gui elements to render properly
    Patch::Ptr m_overlay_draw_patch{};

    struct {
        RenderLayerHook<sdk::renderer::layer::Overlay> overlay{"via.render.layer.Overlay"};
        RenderLayerHook<sdk::renderer::layer::PostEffect> post_effect{"via.render.layer.PostEffect"};
        RenderLayerHook<sdk::renderer::layer::Scene> scene{"via.render.layer.Scene"};
    } m_layer_hooks;
    
    std::recursive_mutex m_openvr_mtx{};
    std::recursive_mutex m_wwise_mtx{};
    std::shared_mutex m_pose_mtx{};
    std::shared_mutex m_gui_mtx{};
    std::shared_mutex m_rotation_mtx{};

    vr::VRTextureBounds_t m_right_bounds{ 0.0f, 0.0f, 1.0f, 1.0f };
    vr::VRTextureBounds_t m_left_bounds{ 0.0f, 0.0f, 1.0f, 1.0f };

    glm::vec3 m_overlay_rotation{-1.550f, 0.0f, -1.330f};
    glm::vec4 m_overlay_position{0.0f, 0.06f, -0.07f, 1.0f};

    float m_nearz{ 0.1f };
    float m_farz{ 3000.0f };

    VRRuntime m_null_runtime{};

    struct OpenXR : public VRRuntime {
        std::string get_result_string(XrResult result) {
            std::string result_string{};
            result_string.resize(XR_MAX_RESULT_STRING_SIZE);
            xrResultToString(this->instance, result, result_string.data());

            return result_string;
        }

        std::string get_structure_string(XrStructureType type) {
            std::string structure_string{};
            structure_string.resize(XR_MAX_STRUCTURE_NAME_SIZE);
            xrStructureTypeToString(this->instance, type, structure_string.data());

            return structure_string;
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

        VRRuntime::Error synchronize_frame() override {
            // cant sync frame between begin and endframe
            if (!this->session_ready || this->frame_began) {
                return VRRuntime::Error::SUCCESS;
            }

            XrFrameWaitInfo frame_wait_info{XR_TYPE_FRAME_WAIT_INFO};
            this->frame_state = {XR_TYPE_FRAME_STATE};
            auto result = xrWaitFrame(this->session, &frame_wait_info, &this->frame_state);

            if (result != XR_SUCCESS) {
                spdlog::error("[VR] xrWaitFrame failed: {}", this->get_result_string(result));
                return (VRRuntime::Error)result;
            }

            return VRRuntime::Error::SUCCESS;
        }

        VRRuntime::Error update_poses() override {
            if (!this->session_ready) {
                return VRRuntime::Error::SUCCESS;
            }

            if (!this->needs_pose_update) {
                return VRRuntime::Error::SUCCESS;
            }

            this->view_state = {XR_TYPE_VIEW_STATE};
            this->stage_view_state = {XR_TYPE_VIEW_STATE};

            uint32_t view_count{};

            XrViewLocateInfo view_locate_info{XR_TYPE_VIEW_LOCATE_INFO};
            view_locate_info.viewConfigurationType = this->view_config;
            view_locate_info.displayTime = this->frame_state.predictedDisplayTime;
            view_locate_info.space = this->view_space;

            auto result = xrLocateViews(this->session, &view_locate_info, &this->view_state, (uint32_t)this->views.size(), &view_count, this->views.data());

            if (result != XR_SUCCESS) {
                spdlog::error("[VR] xrLocateViews for view space failed: {}", this->get_result_string(result));
                return (VRRuntime::Error)result;
            }

            view_locate_info = {XR_TYPE_VIEW_LOCATE_INFO};
            view_locate_info.viewConfigurationType = this->view_config;
            view_locate_info.displayTime = this->frame_state.predictedDisplayTime;
            view_locate_info.space = this->stage_space;

            result = xrLocateViews(this->session, &view_locate_info, &this->stage_view_state, (uint32_t)this->stage_views.size(), &view_count, this->stage_views.data());

            if (result != XR_SUCCESS) {
                spdlog::error("[VR] xrLocateViews for stage space failed: {}", this->get_result_string(result));
                return (VRRuntime::Error)result;
            }

            this->needs_pose_update = false;
            this->got_first_poses = true;
            return VRRuntime::Error::SUCCESS;
        }

        VRRuntime::Error update_render_target_size() override {
            uint32_t view_count{};
            auto result = xrEnumerateViewConfigurationViews(this->instance, this->system, this->view_config, 0, &view_count, nullptr); 
            if (result != XR_SUCCESS) {
                this->error = "Could not get view configuration properties: " + this->get_result_string(result);
                spdlog::error("[VR] {}", this->error.value());

                return (VRRuntime::Error)result;
            }

            this->view_configs.resize(view_count, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
            result = xrEnumerateViewConfigurationViews(this->instance, this->system, this->view_config, view_count, &view_count, this->view_configs.data());
            if (result != XR_SUCCESS) {
                this->error = "Could not get view configuration properties: " + this->get_result_string(result);
                spdlog::error("[VR] {}", this->error.value());

                return (VRRuntime::Error)result;
            }

            return VRRuntime::Error::SUCCESS;
        }

        uint32_t get_width() const override {
            if (this->view_configs.empty()) {
                return 0;
            }

            return this->view_configs[0].recommendedImageRectWidth;
        }

        uint32_t get_height() const override {
            if (this->view_configs.empty()) {
                return 0;
            }

            return this->view_configs[0].recommendedImageRectHeight;
        }

        VRRuntime::Error consume_events(std::function<void(void*)> callback) override {
            XrEventDataBuffer edb{XR_TYPE_EVENT_DATA_BUFFER};
            auto result = xrPollEvent(this->instance, &edb);

            const auto bh = (XrEventDataBaseHeader*)&edb;

            if (result == XR_SUCCESS) {
                spdlog::info("VR: xrEvent: {}", this->get_structure_string(bh->type));

                if (callback) {
                    callback(&edb);
                }

                if (bh->type == XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED) {
                    const auto ev = (XrEventDataSessionStateChanged*)&edb;

                    spdlog::info("VR: XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED {}", ev->state);

                    if (ev->state == XR_SESSION_STATE_READY) {
                        spdlog::info("VR: XR_SESSION_STATE_READY");
                        
                        // Begin the session
                        XrSessionBeginInfo session_begin_info{XR_TYPE_SESSION_BEGIN_INFO};
                        session_begin_info.primaryViewConfigurationType = this->view_config;

                        result = xrBeginSession(this->session, &session_begin_info);

                        if (result != XR_SUCCESS) {
                            this->error = std::string{"xrBeginSessionFailed: "} + this->get_result_string(result);
                            spdlog::error("VR: xrBeginSession failed: {}", this->get_result_string(result));
                        } else {
                            this->session_ready = true;
                        }
                    } else if (ev->state == XR_SESSION_STATE_LOSS_PENDING) {
                        spdlog::info("VR: XR_SESSION_STATE_LOSS_PENDING");
                        this->wants_reinitialize = true;
                    } else if (ev->state == XR_SESSION_STATE_STOPPING) {
                        spdlog::info("VR: XR_SESSION_STATE_STOPPING");

                        if (this->ready()) {
                            xrEndSession(this->session);
                            this->session_ready = false;

                            if (this->wants_reinitialize) {
                                //initialize_openxr();
                            }
                        }
                    }
                }
            } else if (result != XR_EVENT_UNAVAILABLE) {
                spdlog::error("VR: xrPollEvent failed: {}", this->get_result_string(result));
                return (VRRuntime::Error)result;
            }

            return VRRuntime::Error::SUCCESS;
        }

        VRRuntime::Error update_matrices(float nearz, float farz) override {
            if (!this->session_ready || this->views.empty()) {
                return VRRuntime::Error::SUCCESS;
            }

            std::unique_lock __{ this->eyes_mtx };

            for (auto i = 0; i < 2; ++i) {
                const auto& pose = this->views[i].pose;
                const auto& fov = this->views[i].fov;

                // Update projection matrix
                XrMatrix4x4f_CreateProjection((XrMatrix4x4f*)&this->projections[i], GRAPHICS_D3D, tan(fov.angleLeft), tan(fov.angleRight), tan(fov.angleUp), tan(fov.angleDown), nearz, farz);

                // Update view matrix
                this->eyes[i] = Matrix4x4f{*(glm::quat*)&pose.orientation};
                this->eyes[i][3] = Vector4f{*(Vector3f*)&pose.position, 1.0f};
            }

            return VRRuntime::Error::SUCCESS;
        }

        SynchronizeStage get_synchronize_stage() const override {
            return this->custom_stage;
        }

        XrResult begin_frame() {
            if (!this->ready()) {
                //spdlog::info("VR: begin_frame: not ready");
                return XR_ERROR_SESSION_NOT_READY;
            }

            if (this->frame_began) {
                spdlog::info("[VR] begin_frame called while frame already began");
                return XR_SUCCESS;
            }

            XrFrameBeginInfo frame_begin_info{XR_TYPE_FRAME_BEGIN_INFO};
            auto result = xrBeginFrame(this->session, &frame_begin_info);

            if (result != XR_SUCCESS) {
                spdlog::error("[VR] xrBeginFrame failed: {}", this->get_result_string(result));
            }

            this->frame_began = result == XR_SUCCESS || result == XR_FRAME_DISCARDED; // discarded means endFrame was not called

            return result;
        }

        XrResult end_frame() {
            if (!this->frame_began) {
                spdlog::info("[VR] end_frame called while frame not begun");
                return XR_ERROR_CALL_ORDER_INVALID;
            }

            std::vector<XrCompositionLayerBaseHeader*> layers{};
            std::vector<XrCompositionLayerProjectionView> projection_layer_views{};

            // we CANT push the layers every time, it cause some layer error
            // in xrEndFrame, so we must only do it when shouldRender is true
            if (this->frame_state.shouldRender == XR_TRUE) {
                projection_layer_views.resize(this->stage_views.size(), {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW});

                for (auto i = 0; i < projection_layer_views.size(); ++i) {
                    const auto& swapchain = this->swapchains[i];

                    projection_layer_views[i].type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
                    projection_layer_views[i].pose = this->stage_views[i].pose;
                    projection_layer_views[i].fov = this->stage_views[i].fov;
                    projection_layer_views[i].subImage.swapchain = swapchain.handle;
                    projection_layer_views[i].subImage.imageRect.offset = {0, 0};
                    projection_layer_views[i].subImage.imageRect.extent = {swapchain.width, swapchain.height};
                }

                XrCompositionLayerProjection layer{XR_TYPE_COMPOSITION_LAYER_PROJECTION};
                layer.space = this->stage_space;
                layer.viewCount = (uint32_t)projection_layer_views.size();
                layer.views = projection_layer_views.data();
                layers.push_back((XrCompositionLayerBaseHeader*)&layer);
            }

            XrFrameEndInfo frame_end_info{XR_TYPE_FRAME_END_INFO};
            frame_end_info.displayTime = this->frame_state.predictedDisplayTime;
            frame_end_info.environmentBlendMode = this->blend_mode;
            frame_end_info.layerCount = (uint32_t)layers.size();
            frame_end_info.layers = layers.data();

            //spdlog::info("[VR] Ending frame, {} layers", frame_end_info.layerCount);
            //spdlog::info("[VR] Ending frame, layer ptr: {:x}", (uintptr_t)frame_end_info.layers);

            auto result = xrEndFrame(this->session, &frame_end_info);
            
            this->frame_began = false;

            return result;
        }

        bool session_ready{false};
        bool frame_began{false};

        SynchronizeStage custom_stage{SynchronizeStage::EARLY};

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

        std::vector<XrViewConfigurationView> view_configs{};
        std::vector<Swapchain> swapchains{};
        std::vector<XrView> views{};
        std::vector<XrView> stage_views{};
    } m_openxr;

    struct OpenVR : public VRRuntime {
        std::string_view name() const override {
            return "OpenVR";
        }

        VRRuntime::Type type() const override { 
            return VRRuntime::Type::OPENVR;
        }

        bool ready() const override {
            return VRRuntime::ready() && this->is_hmd_active && this->got_first_poses;
        }

        VRRuntime::Error synchronize_frame() override {
            /*if (!this->needs_pose_update) {
                return VRRuntime::Error::SUCCESS;
            }*/

            vr::VRCompositor()->SetTrackingSpace(vr::TrackingUniverseStanding);
            auto ret = vr::VRCompositor()->WaitGetPoses(this->real_render_poses.data(), vr::k_unMaxTrackedDeviceCount, this->real_game_poses.data(), vr::k_unMaxTrackedDeviceCount);

            return (VRRuntime::Error)ret;
        }

        VRRuntime::Error update_poses() override {
            if (!this->needs_pose_update) {
                return VRRuntime::Error::SUCCESS;
            }

            std::unique_lock _{ VR::get()->m_pose_mtx };

            memcpy(this->render_poses.data(), this->real_render_poses.data(), sizeof(this->render_poses));
            this->needs_pose_update = false;
            return VRRuntime::Error::SUCCESS;
        }

        VRRuntime::Error update_render_target_size() override {
            this->hmd->GetRecommendedRenderTargetSize(&this->w, &this->h);

            return VRRuntime::Error::SUCCESS;
        }

        uint32_t get_width() const override {
            return this->w;
        }

        uint32_t get_height() const override {
            return this->h;
        }

        VRRuntime::Error consume_events(std::function<void(void*)> callback) override {
            // Process OpenVR events
            vr::VREvent_t event{};
            while (this->hmd->PollNextEvent(&event, sizeof(event))) {
                if (callback) {
                    callback(&event);
                }

                switch ((vr::EVREventType)event.eventType) {
                    // Detect whether video settings changed
                    case vr::VREvent_SteamVRSectionSettingChanged: {
                        spdlog::info("VR: VREvent_SteamVRSectionSettingChanged");
                        update_render_target_size();
                    } break;

                    // Detect whether SteamVR reset the standing/seated pose
                    case vr::VREvent_SeatedZeroPoseReset: [[fallthrough]];
                    case vr::VREvent_StandingZeroPoseReset: {
                        spdlog::info("VR: VREvent_SeatedZeroPoseReset");
                        this->wants_reset_origin = true;
                    } break;

                    case vr::VREvent_DashboardActivated: {
                        this->handle_pause = true;
                    } break;

                    default:
                        spdlog::info("VR: Unknown event: {}", (uint32_t)event.eventType);
                        break;
                }
            }
            
            return VRRuntime::Error::SUCCESS;
        }

        VRRuntime::Error update_matrices(float nearz, float farz) override{
            std::unique_lock __{ this->eyes_mtx };
            const auto local_left = this->hmd->GetEyeToHeadTransform(vr::Eye_Left);
            const auto local_right = this->hmd->GetEyeToHeadTransform(vr::Eye_Right);

            this->eyes[vr::Eye_Left] = glm::rowMajor4(Matrix4x4f{ *(Matrix3x4f*)&local_left } );
            this->eyes[vr::Eye_Right] = glm::rowMajor4(Matrix4x4f{ *(Matrix3x4f*)&local_right } );

            auto pleft = this->hmd->GetProjectionMatrix(vr::Eye_Left, nearz, farz);
            auto pright = this->hmd->GetProjectionMatrix(vr::Eye_Right, nearz, farz);

            this->projections[vr::Eye_Left] = glm::rowMajor4(Matrix4x4f{ *(Matrix4x4f*)&pleft } );
            this->projections[vr::Eye_Right] = glm::rowMajor4(Matrix4x4f{ *(Matrix4x4f*)&pright } );

            this->hmd->GetProjectionRaw(vr::Eye_Left, &this->raw_projections[vr::Eye_Left][0], &this->raw_projections[vr::Eye_Left][1], &this->raw_projections[vr::Eye_Left][2], &this->raw_projections[vr::Eye_Left][3]);
            this->hmd->GetProjectionRaw(vr::Eye_Right, &this->raw_projections[vr::Eye_Right][0], &this->raw_projections[vr::Eye_Right][1], &this->raw_projections[vr::Eye_Right][2], &this->raw_projections[vr::Eye_Right][3]);

            return VRRuntime::Error::SUCCESS;
        }

        SynchronizeStage get_synchronize_stage() const override {
            return SynchronizeStage::EARLY;
        }

        bool is_hmd_active{false};
        bool was_hmd_active{true};

        uint32_t w{0};
        uint32_t h{0};

        vr::IVRSystem* hmd{nullptr};

        std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> real_render_poses;
        std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> real_game_poses;

        std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> render_poses;
        std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> game_poses;
    } m_openvr;

    Vector4f m_standing_origin{ 0.0f, 1.5f, 0.0f, 0.0f };
    glm::quat m_rotation_offset{ glm::identity<glm::quat>() };
    glm::quat m_gui_rotation_offset{ glm::identity<glm::quat>() };

    std::vector<int32_t> m_controllers{};
    std::unordered_set<int32_t> m_controllers_set{};

    // Action set handles
    vr::VRActionSetHandle_t m_action_set{};
    vr::VRActiveActionSet_t m_active_action_set{};

    // Action handles
    vr::VRActionHandle_t m_action_trigger{ };
    vr::VRActionHandle_t m_action_grip{ };
    vr::VRActionHandle_t m_action_joystick{};
    vr::VRActionHandle_t m_action_joystick_click{};
    vr::VRActionHandle_t m_action_a_button{};
    vr::VRActionHandle_t m_action_b_button{};
    vr::VRActionHandle_t m_action_dpad_up{};
    vr::VRActionHandle_t m_action_dpad_right{};
    vr::VRActionHandle_t m_action_dpad_down{};
    vr::VRActionHandle_t m_action_dpad_left{};
    vr::VRActionHandle_t m_action_system_button{};
    vr::VRActionHandle_t m_action_weapon_dial{};
    vr::VRActionHandle_t m_action_re3_dodge{};
    vr::VRActionHandle_t m_action_re2_quickturn{};
    vr::VRActionHandle_t m_action_re2_firstperson_toggle{};
    vr::VRActionHandle_t m_action_re2_reset_view{};
    vr::VRActionHandle_t m_action_re2_change_ammo{};
    vr::VRActionHandle_t m_action_re2_toggle_flashlight{};
    vr::VRActionHandle_t m_action_minimap{};
    vr::VRActionHandle_t m_action_block{};
    vr::VRActionHandle_t m_action_haptic{};
    vr::VRActionHandle_t m_action_heal{};

    bool m_was_firstperson_toggle_down{false};
    bool m_was_flashlight_toggle_down{false};
    
    
    std::unordered_map<std::string, std::reference_wrapper<vr::VRActionHandle_t>> m_action_handles {
        { "/actions/default/in/Trigger", m_action_trigger },
        { "/actions/default/in/Grip", m_action_grip },
        { "/actions/default/in/Joystick", m_action_joystick },
        { "/actions/default/in/JoystickClick", m_action_joystick_click },
        { "/actions/default/in/AButton", m_action_a_button },
        { "/actions/default/in/BButton", m_action_b_button },
        { "/actions/default/in/DPad_Up", m_action_dpad_up },
        { "/actions/default/in/DPad_Right", m_action_dpad_right },
        { "/actions/default/in/DPad_Down", m_action_dpad_down },
        { "/actions/default/in/DPad_Left", m_action_dpad_left },
        { "/actions/default/in/SystemButton", m_action_system_button },
        { "/actions/default/in/WeaponDial_Start", m_action_weapon_dial },
        { "/actions/default/in/RE3_Dodge", m_action_re3_dodge },
        { "/actions/default/in/RE2_Quickturn", m_action_re2_quickturn },
        { "/actions/default/in/RE2_FirstPerson_Toggle", m_action_re2_firstperson_toggle },
        { "/actions/default/in/RE2_Reset_View", m_action_re2_reset_view },
        { "/actions/default/in/RE2_Change_Ammo", m_action_re2_change_ammo },
        { "/actions/default/in/RE2_Toggle_Flashlight", m_action_re2_toggle_flashlight },
        { "/actions/default/in/MiniMap", m_action_minimap },
        { "/actions/default/in/Block", m_action_block },
        { "/actions/default/in/Heal", m_action_heal },

        // Out
        { "/actions/default/out/Haptic", m_action_haptic },
    };

    // Input sources
    vr::VRInputValueHandle_t m_left_joystick{};
    vr::VRInputValueHandle_t m_right_joystick{};

    // Input system history
    std::bitset<64> m_button_states_down{};
    std::bitset<64> m_button_states_on{};
    std::bitset<64> m_button_states_up{};
    std::chrono::steady_clock::time_point m_last_controller_update{};
    std::chrono::steady_clock::time_point m_last_interaction_display{};
    uint32_t m_backbuffer_inconsistency_start{};
    std::chrono::nanoseconds m_last_input_delay{};
    std::chrono::nanoseconds m_avg_input_delay{};

    std::condition_variable m_present_finished_cv{};
    std::mutex m_present_finished_mtx{};
    
    Vector4f m_raw_projections[2]{};

    vrmod::D3D11Component m_d3d11{};
    vrmod::D3D12Component m_d3d12{};
    vrmod::OverlayComponent m_overlay_component{};

    Vector4f m_original_camera_position{ 0.0f, 0.0f, 0.0f, 0.0f };
    glm::quat m_original_camera_rotation{ glm::identity<glm::quat>() };

    Matrix4x4f m_original_camera_matrix{ glm::identity<Matrix4x4f>() };

    Vector4f m_original_audio_camera_position{ 0.0f, 0.0f, 0.0f, 0.0f };
    glm::quat m_original_audio_camera_rotation{ glm::identity<glm::quat>() };

    Matrix4x4f m_render_camera_matrix{ glm::identity<Matrix4x4f>() };

    sdk::helpers::NativeObject m_via_hid_gamepad{ "via.hid.GamePad" };

    // options
    int m_frame_count{};
    int m_last_frame_count{-1};
    int m_left_eye_frame_count{0};
    int m_right_eye_frame_count{0};

    bool m_submitted{false};
    bool m_present_finished{false};
    //bool m_disable_sharpening{true};

    bool m_needs_camera_restore{false};
    bool m_needs_audio_restore{false};
    bool m_in_render{false};
    bool m_in_lightshaft{false};
    bool m_positional_tracking{true};
    bool m_is_d3d12{false};
    bool m_backbuffer_inconsistency{false};

    // on the backburner
    bool m_depth_aided_reprojection{false};

    // == 1 or == 0
    uint8_t m_left_eye_interval{0};
    uint8_t m_right_eye_interval{1};

    static std::string actions_json;
    static std::string binding_rift_json;
    static std::string bindings_oculus_touch_json;
    static std::string binding_vive;
    static std::string bindings_vive_controller;
    static std::string bindings_knuckles;

    const std::unordered_map<std::string, std::string> m_binding_files {
        { "actions.json", actions_json },
        { "binding_rift.json", binding_rift_json },
        { "bindings_oculus_touch.json", bindings_oculus_touch_json },
        { "binding_vive.json", binding_vive },
        { "bindings_vive_controller.json", bindings_vive_controller },
        { "bindings_knuckles.json", bindings_knuckles }
    };

    const ModKey::Ptr m_set_standing_key{ ModKey::create(generate_name("SetStandingOriginKey")) };
    const ModKey::Ptr m_recenter_view_key{ ModKey::create(generate_name("RecenterViewKey")) };
    const ModToggle::Ptr m_decoupled_pitch{ ModToggle::create(generate_name("DecoupledPitch"), false) };
    const ModToggle::Ptr m_use_afr{ ModToggle::create(generate_name("AlternateFrameRendering"), false) };
    const ModToggle::Ptr m_use_custom_view_distance{ ModToggle::create(generate_name("UseCustomViewDistance"), false) };
    const ModToggle::Ptr m_hmd_oriented_audio{ ModToggle::create(generate_name("HMDOrientedAudio"), true) };
    const ModSlider::Ptr m_view_distance{ ModSlider::create(generate_name("CustomViewDistance"), 10.0f, 3000.0f, 500.0f) };
    const ModSlider::Ptr m_motion_controls_inactivity_timer{ ModSlider::create(generate_name("MotionControlsInactivityTimer"), 10.0f, 100.0f, 10.0f) };
    const ModSlider::Ptr m_joystick_deadzone{ ModSlider::create(generate_name("JoystickDeadzone"), 0.01f, 0.9f, 0.15f) };
    const ModSlider::Ptr m_ui_scale_option{ ModSlider::create(generate_name("2DUIScale"), 1.0f, 100.0f, 12.0f) };
    const ModSlider::Ptr m_ui_distance_option{ ModSlider::create(generate_name("2DUIDistance"), 0.01f, 100.0f, 1.0f) };
    const ModSlider::Ptr m_world_ui_scale_option{ ModSlider::create(generate_name("WorldSpaceUIScale"), 1.0f, 100.0f, 15.0f) };

    const ModToggle::Ptr m_force_fps_settings{ ModToggle::create(generate_name("ForceFPS"), true) };
    const ModToggle::Ptr m_force_aa_settings{ ModToggle::create(generate_name("ForceAntiAliasing"), true) };
    const ModToggle::Ptr m_force_motionblur_settings{ ModToggle::create(generate_name("ForceMotionBlur"), true) };
    const ModToggle::Ptr m_force_vsync_settings{ ModToggle::create(generate_name("ForceVSync"), true) };
    const ModToggle::Ptr m_force_lensdistortion_settings{ ModToggle::create(generate_name("ForceLensDistortion"), true) };
    const ModToggle::Ptr m_force_volumetrics_settings{ ModToggle::create(generate_name("ForceVolumetrics"), true) };
    const ModToggle::Ptr m_force_lensflares_settings{ ModToggle::create(generate_name("ForceLensFlares"), true) };
    const ModToggle::Ptr m_force_dynamic_shadows_settings{ ModToggle::create(generate_name("ForceDynamicShadows"), true) };
    const ModToggle::Ptr m_allow_engine_overlays{ ModToggle::create(generate_name("AllowEngineOverlays"), true) };

    bool m_disable_projection_matrix_override{ false };
    bool m_disable_gui_camera_projection_matrix_override{ false };
    bool m_disable_view_matrix_override{false};
    bool m_disable_backbuffer_size_override{false};
    bool m_disable_temporal_fix{false};
    bool m_disable_post_effect_fix{false};
    bool m_enable_asynchronous_rendering{true};

    ValueList m_options{
        *m_set_standing_key,
        *m_recenter_view_key,
        *m_decoupled_pitch,
        *m_use_afr,
        *m_use_custom_view_distance,
        *m_hmd_oriented_audio,
        *m_view_distance,
        *m_motion_controls_inactivity_timer,
        *m_joystick_deadzone,
        *m_force_fps_settings,
        *m_force_aa_settings,
        *m_force_motionblur_settings,
        *m_force_vsync_settings,
        *m_force_lensdistortion_settings,
        *m_force_volumetrics_settings,
        *m_force_lensflares_settings,
        *m_force_dynamic_shadows_settings,
        *m_ui_scale_option,
        *m_ui_distance_option,
        *m_world_ui_scale_option,
        *m_allow_engine_overlays
    };

    bool m_use_rotation{true};

    friend class vrmod::D3D11Component;
    friend class vrmod::D3D12Component;
    friend class vrmod::OverlayComponent;
};
