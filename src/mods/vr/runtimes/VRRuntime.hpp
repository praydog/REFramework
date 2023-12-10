#pragma once

#include <cstdint>
#include <string_view>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <optional>
#include <array>

#include <spdlog/spdlog.h>
#include <sdk/Math.hpp>

struct VRRuntime {
    enum class Error : int64_t {
        UNSPECIFIED = -1,
        SUCCESS = 0,
        // rest of the error codes will be from the specific VR runtime
    };

    enum class Type : uint8_t {
        NONE,
        OPENXR,
        OPENVR,
    };

    enum class Eye : uint8_t {
        LEFT,
        RIGHT,
    };

    enum class SynchronizeStage : int32_t { 
        EARLY,
        LATE,
        VERY_LATE
    };

    enum Hand : uint8_t {
        LEFT,
        RIGHT,
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

    virtual void destroy() {
        this->loaded = false;
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
        return this->custom_stage;
    }

    virtual Error update_input() {
        return Error::SUCCESS;
    }

    bool is_openxr() const {
        return this->type() == Type::OPENXR;
    }

    bool is_openvr() const {
        return this->type() == Type::OPENVR;
    }

    bool loaded{false};
    bool wants_reinitialize{false};
    bool dll_missing{false};

    // in the case of OpenVR we always need at least one initial WaitGetPoses before the game will render
    // even if we don't have anything to submit yet, otherwise the compositor
    // will return VRCompositorError_DoNotHaveFocus
    bool needs_pose_update{true};
    bool got_first_poses{false};
    bool got_first_valid_poses{false};
    bool got_first_sync{false};
    bool handle_pause{false};
    bool wants_reset_origin{true};

    std::optional<std::string> error{};

    std::array<Matrix4x4f, 2> projections{};
    std::array<Matrix4x4f, 2> eyes{};

    mutable std::shared_mutex projections_mtx{};
    mutable std::shared_mutex eyes_mtx{};
    mutable std::shared_mutex pose_mtx{};

    Vector4f raw_projections[2]{};

    SynchronizeStage custom_stage{SynchronizeStage::EARLY};
};