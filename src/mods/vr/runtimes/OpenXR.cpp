#include <json.hpp>
#include <utility/String.hpp>

#include "OpenXR.hpp"

using namespace nlohmann;

namespace runtimes {
VRRuntime::Error OpenXR::synchronize_frame() {
    std::scoped_lock _{sync_mtx};

    // cant sync frame between begin and endframe
    if (!this->session_ready || this->frame_began) {
        return (VRRuntime::Error)-1;
    }

    this->begin_profile();

    XrFrameWaitInfo frame_wait_info{XR_TYPE_FRAME_WAIT_INFO};
    this->frame_state = {XR_TYPE_FRAME_STATE};
    auto result = xrWaitFrame(this->session, &frame_wait_info, &this->frame_state);

    this->end_profile("xrWaitFrame");

    if (result != XR_SUCCESS) {
        spdlog::error("[VR] xrWaitFrame failed: {}", this->get_result_string(result));
        return (VRRuntime::Error)result;
    } else {
        this->got_first_sync = true;
    }

    return VRRuntime::Error::SUCCESS;
}

VRRuntime::Error OpenXR::update_poses() {
    std::scoped_lock _{ this->sync_mtx };
    std::unique_lock __{ this->pose_mtx };

    if (!this->session_ready) {
        return VRRuntime::Error::SUCCESS;
    }

    /*if (!this->needs_pose_update) {
        return VRRuntime::Error::SUCCESS;
    }*/

    this->view_state = {XR_TYPE_VIEW_STATE};
    this->stage_view_state = {XR_TYPE_VIEW_STATE};

    uint32_t view_count{};

    const auto display_time = this->frame_state.predictedDisplayTime + (this->frame_state.predictedDisplayPeriod * this->prediction_scale);

    XrViewLocateInfo view_locate_info{XR_TYPE_VIEW_LOCATE_INFO};
    view_locate_info.viewConfigurationType = this->view_config;
    view_locate_info.displayTime = display_time;
    view_locate_info.space = this->view_space;

    auto result = xrLocateViews(this->session, &view_locate_info, &this->view_state, (uint32_t)this->views.size(), &view_count, this->views.data());

    if (result != XR_SUCCESS) {
        spdlog::error("[VR] xrLocateViews for view space failed: {}", this->get_result_string(result));
        return (VRRuntime::Error)result;
    }

    view_locate_info = {XR_TYPE_VIEW_LOCATE_INFO};
    view_locate_info.viewConfigurationType = this->view_config;
    view_locate_info.displayTime = display_time;
    view_locate_info.space = this->stage_space;

    result = xrLocateViews(this->session, &view_locate_info, &this->stage_view_state, (uint32_t)this->stage_views.size(), &view_count, this->stage_views.data());

    if (result != XR_SUCCESS) {
        spdlog::error("[VR] xrLocateViews for stage space failed: {}", this->get_result_string(result));
        return (VRRuntime::Error)result;
    }

    result = xrLocateSpace(this->view_space, this->stage_space, display_time, &this->view_space_location);

    if (result != XR_SUCCESS) {
        spdlog::error("[VR] xrLocateSpace for view space failed: {}", this->get_result_string(result));
        return (VRRuntime::Error)result;
    }

    this->needs_pose_update = false;
    this->got_first_poses = true;
    return VRRuntime::Error::SUCCESS;
}

VRRuntime::Error OpenXR::update_render_target_size() {
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

uint32_t OpenXR::get_width() const {
    if (this->view_configs.empty()) {
        return 0;
    }

    return this->view_configs[0].recommendedImageRectWidth;
}

uint32_t OpenXR::get_height() const {
    if (this->view_configs.empty()) {
        return 0;
    }

    return this->view_configs[0].recommendedImageRectHeight;
}

VRRuntime::Error OpenXR::consume_events(std::function<void(void*)> callback) {
    std::scoped_lock _{sync_mtx};

    XrEventDataBuffer edb{XR_TYPE_EVENT_DATA_BUFFER};
    auto result = xrPollEvent(this->instance, &edb);

    const auto bh = (XrEventDataBaseHeader*)&edb;

    while (result == XR_SUCCESS) {
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

        edb = {XR_TYPE_EVENT_DATA_BUFFER};
        result = xrPollEvent(this->instance, &edb);
    } 
    
    if (result != XR_EVENT_UNAVAILABLE) {
        spdlog::error("VR: xrPollEvent failed: {}", this->get_result_string(result));
        return (VRRuntime::Error)result;
    }

    return VRRuntime::Error::SUCCESS;
}

VRRuntime::Error OpenXR::update_matrices(float nearz, float farz) {
    if (!this->session_ready || this->views.empty()) {
        return VRRuntime::Error::SUCCESS;
    }

    std::unique_lock __{ this->eyes_mtx };
    std::unique_lock ___{ this->pose_mtx };

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

std::string OpenXR::get_result_string(XrResult result) {
    std::string result_string{};
    result_string.resize(XR_MAX_RESULT_STRING_SIZE);
    xrResultToString(this->instance, result, result_string.data());

    return result_string;
}

std::string OpenXR::get_structure_string(XrStructureType type) {
    std::string structure_string{};
    structure_string.resize(XR_MAX_STRUCTURE_NAME_SIZE);
    xrStructureTypeToString(this->instance, type, structure_string.data());

    return structure_string;
}

std::optional<std::string> OpenXR::initialize_actions(const std::string& json_string) {
    spdlog::info("[VR] Initializing actions");

    if (auto result = xrStringToPath(this->instance, "/user/hand/left", &this->hands[VRRuntime::Hand::LEFT].path); result != XR_SUCCESS) {
        return "xrStringToPath failed (left): " + this->get_result_string(result);
    }

    if (auto result = xrStringToPath(this->instance, "/user/hand/right", &this->hands[VRRuntime::Hand::RIGHT].path); result != XR_SUCCESS) {
        return "xrStringToPath failed (right): " + this->get_result_string(result);
    }

    if (json_string.empty()) {
        return std::nullopt;
    }

    spdlog::info("[VR] Creating action set");

    XrActionSetCreateInfo action_set_create_info{XR_TYPE_ACTION_SET_CREATE_INFO};
    strcpy(action_set_create_info.actionSetName, "default");
    strcpy(action_set_create_info.localizedActionSetName, "Default");
    action_set_create_info.priority = 0;

    if (auto result = xrCreateActionSet(this->instance, &action_set_create_info, &this->action_set.handle); result != XR_SUCCESS) {
        return "xrCreateActionSet failed: " + this->get_result_string(result);
    }

    // Parse the JSON string using nlohmann
    json actions_json{};

    try {
        actions_json = json::parse(json_string);
    } catch (const std::exception& e) {
        return std::string{"json parse failed: "} + e.what();
    }

    if (actions_json.count("actions") == 0) {
        return "json missing actions";
    }

    auto actions_list = actions_json["actions"];

    bool has_pose_action = false;

    for (auto& action : actions_list) {
        XrActionCreateInfo action_create_info{XR_TYPE_ACTION_CREATE_INFO};
        auto action_name = action["name"].get<std::string>();

        if (auto it = action_name.find_last_of("/"); it != std::string::npos) {
            action_name = action_name.substr(it + 1);
        }

        auto localized_action_name = action_name;
        std::transform(action_name.begin(), action_name.end(), action_name.begin(), ::tolower);

        strcpy(action_create_info.actionName, action_name.c_str());
        strcpy(action_create_info.localizedActionName, localized_action_name.c_str());

        if (action_name == "pose") {
            has_pose_action = true;
        }

        // Translate the OpenVR action types to OpenXR action types
        switch (utility::hash(action["type"].get<std::string>())) {
            case "bool"_fnv:
                action_create_info.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
                break;
            case "skeleton"_fnv: // idk what this is in OpenXR
                continue;
            case "pose"_fnv:
                action_create_info.actionType = XR_ACTION_TYPE_POSE_INPUT;
                break;
            case "vector1"_fnv:
                action_create_info.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
                break;
            case "vector2"_fnv:
                action_create_info.actionType = XR_ACTION_TYPE_VECTOR2F_INPUT;
                break;
            case "vibration"_fnv:
                action_create_info.actionType = XR_ACTION_TYPE_VIBRATION_OUTPUT;
                break;
        }
        
        // Create the action
        XrAction xr_action{};
        if (auto result = xrCreateAction(this->action_set.handle, &action_create_info, &xr_action); result != XR_SUCCESS) {
            return "xrCreateAction failed for " + action_name + ": " + this->get_result_string(result);
        }

        spdlog::info("[VR] Created action {}", action_name);

        this->action_set.actions.push_back(xr_action);
        this->action_set.action_map[action_name] = xr_action;
    }

    if (!has_pose_action) {
        return "json missing pose action";
    }

    // TODO: Suggest bindings
    /*
    do the suggest bindings
    */

    // Create the action spaces for each hand
    for (auto i = 0; i < 2; ++i) {
        spdlog::info("[VR] Creating action space for hand {}", i);
        
        XrActionSpaceCreateInfo action_space_create_info{XR_TYPE_ACTION_SPACE_CREATE_INFO};
        action_space_create_info.action = this->action_set.action_map["pose"];
        action_space_create_info.subactionPath = this->hands[i].path;
        action_space_create_info.poseInActionSpace.orientation.w = 1.0f;

        if (auto result = xrCreateActionSpace(this->session, &action_space_create_info, &this->hands[i].space); result != XR_SUCCESS) {
            return "xrCreateActionSpace failed (" + std::to_string(i) + ")" + this->get_result_string(result);
        }
    }

    // Attach the action set to the session
    spdlog::info("[VR] Attaching action set to session");

    XrSessionActionSetsAttachInfo action_sets_attach_info{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};
    action_sets_attach_info.countActionSets = 1;
    action_sets_attach_info.actionSets = &this->action_set.handle;

    if (auto result = xrAttachSessionActionSets(this->session, &action_sets_attach_info); result != XR_SUCCESS) {
        return "xrAttachSessionActionSets failed: " + this->get_result_string(result);
    }

    return std::nullopt;
}

XrResult OpenXR::begin_frame() {
    std::scoped_lock _{sync_mtx};

    if (!this->ready() || !this->got_first_poses) {
        //spdlog::info("VR: begin_frame: not ready");
        return XR_ERROR_SESSION_NOT_READY;
    }

    if (this->frame_began) {
        spdlog::info("[VR] begin_frame called while frame already began");
        return XR_SUCCESS;
    }

    this->begin_profile();

    XrFrameBeginInfo frame_begin_info{XR_TYPE_FRAME_BEGIN_INFO};
    auto result = xrBeginFrame(this->session, &frame_begin_info);

    this->end_profile("xrBeginFrame");

    if (result != XR_SUCCESS) {
        spdlog::error("[VR] xrBeginFrame failed: {}", this->get_result_string(result));
    }

    if (result == XR_ERROR_CALL_ORDER_INVALID) {
        synchronize_frame();
        result = xrBeginFrame(this->session, &frame_begin_info);
    }

    this->frame_began = result == XR_SUCCESS || result == XR_FRAME_DISCARDED; // discarded means endFrame was not called

    return result;
}

XrResult OpenXR::end_frame() {
    std::scoped_lock _{sync_mtx};

    if (!this->ready() || !this->got_first_poses) {
        return XR_ERROR_SESSION_NOT_READY;
    }

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

    this->begin_profile();
    auto result = xrEndFrame(this->session, &frame_end_info);
    this->end_profile("xrEndFrame");
    
    if (result != XR_SUCCESS) {
        spdlog::error("[VR] xrEndFrame failed: {}", this->get_result_string(result));
    }
    
    this->frame_began = false;

    return result;
}
}
