#pragma once

#include "VRRuntime.hpp"

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

namespace runtimes{
struct OpenXR : public VRRuntime {
    OpenXR() {
        this->custom_stage = SynchronizeStage::EARLY;
    }

    std::string get_result_string(XrResult result);
    std::string get_structure_string(XrStructureType type);

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

    XrResult begin_frame();
    XrResult end_frame();

    float prediction_scale{0.0f};
    bool session_ready{false};
    bool frame_began{false};

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

    XrSpaceLocation view_space_location{XR_TYPE_SPACE_LOCATION};

    std::vector<XrViewConfigurationView> view_configs{};
    std::vector<Swapchain> swapchains{};
    std::vector<XrView> views{};
    std::vector<XrView> stage_views{};
};
}