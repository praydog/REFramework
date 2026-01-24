#pragma once

#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <windows.h>

#include "VRRuntime.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <json.hpp>
#include <sdk/Math.hpp>

namespace runtimes {
struct XrDriver final : public VRRuntime {
    XrDriver() { this->custom_stage = SynchronizeStage::EARLY; }

    virtual ~XrDriver() { this->destroy(); }

    std::string_view name() const override { return "XrDriver"; }

    VRRuntime::Type type() const override { return VRRuntime::Type::XR_DRIVER; }

    VRRuntime::Error synchronize_frame() override;
    VRRuntime::Error update_poses() override;
    VRRuntime::Error update_render_target_size() override;

    uint32_t get_width() const override;
    uint32_t get_height() const override;

    VRRuntime::Error consume_events(std::function<void(void*)> callback) override;
    VRRuntime::Error update_matrices(float nearz, float farz) override;

    void destroy() override;

    struct XrDriverData {
        double posx = 0.0;
        double posy = 0.0;
        double posz = 0.0;
        double qx = 0.0;
        double qy = 0.0;
        double qz = 0.0;
        double qw = 0.0;
    };

    // Stored pose data for VR::get_rotation() and get_position()
    glm::quat m_hmd_rotation{};
    Vector4f m_hmd_position{};

private:
    void load_dll_data();
    bool load_xr_driver_dll();
    void unload_xr_driver_dll();

    // DLL loading
    HMODULE m_dll_handle{nullptr};
    std::string m_dll_path{"xr_driver.dll"};

    // Function pointer types for DLL exports
    typedef void (*GetXrDriverDataFunc)(double* posx, double* posy, double* posz, double* qx, double* qy, double* qz, double* qw);
    typedef void (*StartTrackingFunc)();
    GetXrDriverDataFunc m_get_data_func{nullptr};
    StartTrackingFunc m_start_tracking_func{nullptr};

    XrDriverData m_track_data{};
    std::mutex m_track_data_mutex{};
    std::chrono::steady_clock::time_point m_last_read_time{};
    std::chrono::milliseconds m_read_interval{16}; // ~60fps default

    // VR render target size - typical per-eye resolution for modern VR headsets
    // Quest 2/3: ~2016x2240 per eye, but we'll use a reasonable default
    uint32_t w{1920};
    uint32_t h{1080};
};
} // namespace runtimes
