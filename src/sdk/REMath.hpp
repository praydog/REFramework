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

}