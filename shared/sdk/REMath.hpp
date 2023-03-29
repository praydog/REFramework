#pragma once

#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "ReClass.hpp"

namespace utility::math {
using namespace glm;

static vec3 euler_angles(const glm::mat4& rot);
static float fix_angle(float ang);
static void fix_angles(const glm::vec3& angles);
static float clamp_pitch(float ang);

// RE engine's way of storing euler angles or I'm just an idiot.
static vec3 euler_angles(const glm::mat4& rot) {
    float pitch = 0.0f;
    float yaw = 0.0f;
    float roll = 0.0f;
    glm::extractEulerAngleYZX(rot, yaw, roll, pitch);

    return { pitch, yaw, roll };
}

static float fix_angle(float ang) {
    auto angDeg = glm::degrees(ang);

    while (angDeg > 180.0f) {
        angDeg -= 360.0f;
    }

    while (angDeg < -180.0f) {
        angDeg += 360.0f;
    }

    return glm::radians(angDeg);
}

static void fix_angles(glm::vec3& angles) {
    angles[0] = fix_angle(angles[0]);
    angles[1] = fix_angle(angles[1]);
    angles[2] = fix_angle(angles[2]);
}

float clamp_pitch(float ang) {
    return std::clamp(ang, glm::radians(-89.0f), glm::radians(89.0f));
}

static glm::mat4 remove_y_component(const glm::mat4& mat) {
    // Remove y component and normalize so we have the facing direction
    const auto forward_dir = glm::normalize(Vector3f{ mat[2].x, 0.0f, mat[2].z });

    return glm::rowMajor4(glm::lookAtLH(Vector3f{}, Vector3f{ forward_dir }, Vector3f(0.0f, 1.0f, 0.0f)));
}

static quat to_quat(const vec3& v) {
    const auto mat = glm::rowMajor4(glm::lookAtLH(Vector3f{0.0f, 0.0f, 0.0f}, v, Vector3f{0.0f, 1.0f, 0.0f}));

    return glm::quat{mat};
}

static quat flatten(const quat& q) {
    const auto forward = glm::normalize(glm::quat{q} * Vector3f{ 0.0f, 0.0f, 1.0f });
    const auto flattened_forward = glm::normalize(Vector3f{forward.x, 0.0f, forward.z});
    return utility::math::to_quat(flattened_forward);
}
}