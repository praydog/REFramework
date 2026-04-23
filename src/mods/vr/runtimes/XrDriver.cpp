#include "XrDriver.hpp"
#include <cmath>
#include <sdk/Math.hpp>
#include <spdlog/spdlog.h>
#include <windows.h>

namespace runtimes {
VRRuntime::Error XrDriver::synchronize_frame() {
    load_dll_data();
    if (this->ready()) {
        std::unique_lock<std::shared_mutex> _{this->pose_mtx};
        std::lock_guard<std::mutex> data_lock{m_track_data_mutex};
        glm::quat q;
        q.w = (float)(m_track_data.qw);
        q.x = (float)(m_track_data.qx);
        q.y = (float)(m_track_data.qy);
        q.z = (float)(m_track_data.qz);
        q = glm::normalize(q);
        m_hmd_rotation = q;
        m_hmd_position = Vector4f{(float)m_track_data.posx, (float)m_track_data.posy, (float)m_track_data.posz, 1.0f};
    }

    if (this->got_first_poses) {
        return VRRuntime::Error::SUCCESS;
    }

    // Mark as having poses once we've read data at least once
    if (m_track_data.posx != 0.0 || m_track_data.posy != 0.0 || m_track_data.posz != 0.0 || m_track_data.qx != 0.0 ||
        m_track_data.qy != 0.0 || m_track_data.qz != 0.0 || m_track_data.qw != 0.0) {
        this->got_first_valid_poses = true;
        this->got_first_sync = true;
        this->got_first_poses = true;
    }

    return VRRuntime::Error::SUCCESS;
}

VRRuntime::Error XrDriver::update_poses() {
    if (!this->ready()) {
        return VRRuntime::Error::SUCCESS;
    }
    // TODO: check if this is needed
    load_dll_data();
    std::unique_lock<std::shared_mutex> _{this->pose_mtx};
    std::lock_guard<std::mutex> data_lock{m_track_data_mutex};
    glm::quat q;
    q.w = (float)(m_track_data.qw);
    q.x = (float)(m_track_data.qx);
    q.y = (float)(m_track_data.qy);
    q.z = (float)(m_track_data.qz);
    q = glm::normalize(q);
    m_hmd_rotation = q;
    this->needs_pose_update = false;
    return VRRuntime::Error::SUCCESS;
}

VRRuntime::Error XrDriver::update_render_target_size() {
    // Default render target size, can be configured via JSON if needed
    // w and h are already set from member variables
    return VRRuntime::Error::SUCCESS;
}

uint32_t XrDriver::get_width() const {
    return this->w;
}

uint32_t XrDriver::get_height() const {
    return this->h;
}

VRRuntime::Error XrDriver::consume_events(std::function<void(void*)> callback) {
    // No events for JSON file input
    return VRRuntime::Error::SUCCESS;
}

VRRuntime::Error XrDriver::update_matrices(float nearz, float farz) {
    std::unique_lock<std::shared_mutex> __{this->eyes_mtx};

    this->eyes[0] = glm::identity<Matrix4x4f>();
    this->eyes[1] = glm::identity<Matrix4x4f>();
    float fov = 90.0f * (3.14159f / 180.0f); // 90 degrees FOV
    float aspect = (float)this->w / (float)this->h;
    float f = 1.0f / tanf(fov / 2.0f);
    float z_range = farz - nearz;

    Matrix4x4f proj{};
    proj[0][0] = f / aspect;
    proj[1][1] = f;
    proj[2][2] = -(farz + nearz) / z_range;
    proj[2][3] = -1.0f;
    proj[3][2] = -(2.0f * farz * nearz) / z_range;
    proj[3][3] = 0.0f;

    this->projections[0] = proj;
    this->projections[1] = proj;

    this->raw_projections[0] = Vector4f{-1.0f, 1.0f, -1.0f, 1.0f};
    this->raw_projections[1] = Vector4f{-1.0f, 1.0f, -1.0f, 1.0f};

    return VRRuntime::Error::SUCCESS;
}

void XrDriver::destroy() {
    if (this->loaded) {
        unload_xr_driver_dll();
    }
}

bool XrDriver::load_xr_driver_dll() {
    if (m_dll_handle) {
        return true;
    }

    spdlog::info("[XrDriver] Loading DLL: {}", m_dll_path);
    m_dll_handle = LoadLibraryA(m_dll_path.c_str());
    if (!m_dll_handle) {
        DWORD error = GetLastError();
        spdlog::warn("[XrDriver] Failed to load {}: error code {}", m_dll_path, error);
        return false;
    }

    m_start_tracking_func = (StartTrackingFunc)GetProcAddress(m_dll_handle, "StartTracking");
    if (m_start_tracking_func) {
        spdlog::info("[XrDriver] Found StartTracking function, calling it");
        m_start_tracking_func();
    }

    m_get_data_func = (GetXrDriverDataFunc)GetProcAddress(m_dll_handle, "GetTrackingData");
    if (m_get_data_func) {
        spdlog::info("[XrDriver] Successfully loaded {} and found function {}", m_dll_path, "GetTrackingData");
        return true;
    }
    spdlog::error("[XrDriver] Loaded {} but could not find data function", m_dll_path);
    FreeLibrary(m_dll_handle);
    m_dll_handle = nullptr;
    m_start_tracking_func = nullptr;
    return false;
    if (m_dll_handle) {
        FreeLibrary(m_dll_handle);
        m_dll_handle = nullptr;
        m_get_data_func = nullptr;
        m_start_tracking_func = nullptr;
    }
}

void XrDriver::unload_xr_driver_dll() {
    if (m_dll_handle) {
        FreeLibrary(m_dll_handle);
        m_dll_handle = nullptr;
        m_get_data_func = nullptr;
        m_start_tracking_func = nullptr;
    }
}

void XrDriver::load_dll_data() {
    if (!m_dll_handle) {
        if (!load_xr_driver_dll()) {
            return;
        }
    }
    if (m_get_data_func) {
        double pos_x, pos_y, pos_z, qx, qy, qz, qw;
        m_get_data_func(&pos_x, &pos_y, &pos_z, &qx, &qy, &qz, &qw);
        std::lock_guard<std::mutex> lock{m_track_data_mutex};
        m_track_data.posx = pos_x;
        m_track_data.posy = pos_y;
        m_track_data.posz = pos_z;
        m_track_data.qx = qx;
        m_track_data.qy = qy;
        m_track_data.qz = qz;
        m_track_data.qw = qw;
    }
}
} // namespace runtimes
