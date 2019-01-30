#pragma once

namespace utility {
    namespace RETransform {
        static Matrix4x4f invalidMatrix{};

        // Get a bone/joint by name
        static REJoint* getJoint(const ::RETransform& transform, std::wstring_view name) {
            auto& jointArray = transform.joints;

            if (jointArray.size <= 0 || jointArray.numAllocated <= 0 || jointArray.data == nullptr || jointArray.matrices == nullptr) {
                return nullptr;
            }

            for (int32_t i = 0; i < jointArray.size; ++i) {
                auto joint = jointArray.data->joints[i];

                if (joint == nullptr) {
                    continue;
                }

                auto jointInfo = joint->info;

                if (jointInfo == nullptr || jointInfo->name == nullptr) {
                    continue;
                }

                if (name == jointInfo->name) {
                    return joint;
                }
            }

            return nullptr;
        }

        // Get a bone/joint matrix by name
        static Matrix4x4f& getJointMatrix(const ::RETransform& transform, std::wstring_view name) {
            auto joint = getJoint(transform, name);

            if (joint != nullptr) {
                return transform.joints.matrices->data[joint->info->jointNumber].worldMatrix;
            }

            return invalidMatrix;
        }
    }
}