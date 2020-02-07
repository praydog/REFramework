#pragma once

#include "Math.hpp"

namespace utility::re_transform {
    static Matrix4x4f invalid_matrix{};

    // Get a bone/joint by name
    static REJoint* get_joint(const ::RETransform& transform, std::wstring_view name) {
        auto& joint_array = transform.joints;

        if (joint_array.size <= 0 || joint_array.numAllocated <= 0 || joint_array.data == nullptr || joint_array.matrices == nullptr) {
            return nullptr;
        }

        for (int32_t i = 0; i < joint_array.size; ++i) {
            auto joint = joint_array.data->joints[i];

            if (joint == nullptr) {
                continue;
            }

            auto joint_info = joint->info;

            if (joint_info == nullptr || joint_info->name == nullptr) {
                continue;
            }

            if (name == joint_info->name) {
                return joint;
            }
        }

        return nullptr;
    }

    // Get a bone/joint matrix by name
    static Matrix4x4f& get_joint_matrix(const ::RETransform& transform, std::wstring_view name) {
        auto joint = get_joint(transform, name);

        if (joint != nullptr) {
            return transform.joints.matrices->data[joint->info->jointNumber].worldMatrix;
        }

        return invalid_matrix;
    }
}
