#pragma once

#include <vector>

#include "Math.hpp"

namespace utility::re_transform {
    // because the ReClass version looks like ass
    struct Joint : public REManagedObject {
		::RETransform* Owner;
		::REJointDesc* Desc;
		Vector4f LocalPosition;
		Vector4f LocalRotation;
		Vector4f LocalScale;
		int32_t ConstraintJointIndex;
		int32_t JointIndex;
	};

    static Matrix4x4f invalid_matrix{};

    static REJoint* get_joint(const ::RETransform& transform, uint32_t index) {
        auto& joint_array = transform.joints;

        if (joint_array.size <= 0 || joint_array.numAllocated <= 0 || joint_array.data == nullptr || joint_array.matrices == nullptr) {
            return nullptr;
        }

        auto joint = joint_array.data->joints[index];

        if (joint == nullptr) {
            return nullptr;
        }

        auto joint_info = joint->info;

        if (joint_info == nullptr || joint_info->name == nullptr) {
            return nullptr;
        }

        return joint;
    }

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

        if (joint != nullptr && joint->info != nullptr) {
            return transform.joints.matrices->data[((Joint*)joint)->JointIndex].worldMatrix;
        }

        return invalid_matrix;
    }

    static Matrix4x4f& get_joint_matrix(const ::RETransform& transform, REJoint* joint) {
        if (joint != nullptr && joint->info != nullptr) {
            return transform.joints.matrices->data[((Joint*)joint)->JointIndex].worldMatrix;
        }

        return invalid_matrix;
    }

    static std::vector<REJoint*> get_all_children(const ::RETransform& transform, REJoint* parent, std::unordered_set<REJoint*>& visited) {
        if (parent->info == nullptr || visited.contains(parent)) {
            return {};
        }
        
        visited.insert(parent);

        std::vector<REJoint*> children{};

        for (int32_t i = 0; i < transform.joints.size; ++i) {
            auto joint = transform.joints.data->joints[i];

            if (joint == nullptr || joint == parent) {
                continue;
            }

            auto joint_info = joint->info;

            if (joint_info == nullptr || joint_info->name == nullptr) {
                continue;
            }

            if (joint->info->parentJoint == ((Joint*)parent)->JointIndex) {
                children.push_back(joint);

                auto child_children = get_all_children(transform, joint, visited);

                for (auto child_child : child_children) {
                    children.push_back(child_child);
                }
            }
        }

        return children;
    }

    static std::vector<REJoint*> get_all_children(const ::RETransform& transform, REJoint* parent) {
        std::unordered_set<REJoint*> visited{};

        return get_all_children(transform, parent, visited);
    }

    static std::vector<REJoint*> get_immediate_children(const ::RETransform& transform, REJoint* parent) {
        if (parent->info == nullptr) {
            return {};
        }

        std::vector<REJoint*> children{};

        for (int32_t i = 0; i < transform.joints.size; ++i) {
            auto joint = transform.joints.data->joints[i];

            if (joint == nullptr || joint == parent) {
                continue;
            }

            auto joint_info = joint->info;

            if (joint_info == nullptr || joint_info->name == nullptr) {
                continue;
            }

            if (joint->info->parentJoint == ((Joint*)parent)->JointIndex) {
                children.push_back(joint);
            }
        }

        return children;
    }
}
