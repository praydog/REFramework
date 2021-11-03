#pragma once

#include <vector>

#include "Math.hpp"

#ifdef RE8
#include "REArray.hpp"
#endif

#include "RETypeDB.hpp"

namespace sdk {
// because the ReClass version looks like ass
struct Joint : public REManagedObject {
    ::RETransform* Owner;
    ::REJointDesc* Desc;
    Vector4f LocalPosition;
    Vector4f LocalRotation;
    Vector4f LocalScale;
    int32_t ConstraintJointIndex;
    // for some reason this doesn't work on RE2?
    int32_t JointIndex;

    int32_t get_joint_index() const {
#ifndef RE2
        return JointIndex;
#else
        if (Owner == nullptr) {
            return 0;
        }

        auto& joint_array = Owner->joints;

        if (joint_array.size <= 0 || joint_array.numAllocated <= 0 || joint_array.data == nullptr || joint_array.matrices == nullptr) {
            return 0;
        }

        for (auto i = 0; i < joint_array.size; i++) {
            auto joint = joint_array.data->joints[i];

            if ((sdk::Joint*)joint == this) {
                return i;
            }
        }

        return 0;
#endif
    }
};
}

namespace utility::re_transform {
    static Matrix4x4f invalid_matrix{};

    static REJoint* get_joint(const ::RETransform& transform, uint32_t index) {
#if !defined(RE8) && !defined(MHRISE)
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
#else
        return utility::re_array::get_element<REJoint>(transform.joints.data, index);
#endif
    }

    // Get a bone/joint by name
    static REJoint* get_joint(const ::RETransform& transform, std::wstring_view name) {
#if !defined(RE8) && !defined(MHRISE)
        auto& joint_array = transform.joints;

        if (joint_array.size <= 0 || joint_array.numAllocated <= 0 || joint_array.data == nullptr || joint_array.matrices == nullptr) {
            return nullptr;
        }
#endif

#if !defined(RE8) && !defined(MHRISE)
        for (int32_t i = 0; i < joint_array.size; ++i) {
            auto joint = joint_array.data->joints[i];
#else
        if (transform.joints.data == nullptr) {
            return nullptr;
        }

        for (int32_t i = 0; i < transform.joints.data->numElements; ++i) {

            auto joint = utility::re_array::get_element<REJoint>(transform.joints.data, i);
#endif

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

    static Matrix4x4f& get_joint_matrix_by_index(const ::RETransform& transform, uint32_t index) {
        return transform.joints.matrices->data[index].worldMatrix;
    }

    // Get a bone/joint matrix by name
    static Matrix4x4f& get_joint_matrix(const ::RETransform& transform, std::wstring_view name) {
        auto joint = get_joint(transform, name);

        if (joint != nullptr && joint->info != nullptr) {
            return transform.joints.matrices->data[((sdk::Joint*)joint)->get_joint_index()].worldMatrix;
        }

        return invalid_matrix;
    }

    static Matrix4x4f& get_joint_matrix(const ::RETransform& transform, REJoint* joint) {
        if (joint != nullptr && joint->info != nullptr) {
            return transform.joints.matrices->data[((sdk::Joint*)joint)->get_joint_index()].worldMatrix;
        }

        return invalid_matrix;
    }

    static std::vector<REJoint*> get_all_children(const ::RETransform& transform, REJoint* parent, std::unordered_set<REJoint*>& visited) {
        if (parent->info == nullptr || visited.contains(parent)) {
            return {};
        }

#if defined(RE8) || defined(MHRISE)
        if (transform.joints.data == nullptr) {
            return {};
        }
#endif
        
        visited.insert(parent);

        std::vector<REJoint*> children{};

#if !defined(RE8) && !defined(MHRISE)
        for (int32_t i = 0; i < transform.joints.size; ++i) {
            auto joint = transform.joints.data->joints[i]; 
#else
        for (int32_t i = 0; i < transform.joints.data->numElements; ++i) {
            auto joint = utility::re_array::get_element<REJoint>(transform.joints.data, i);
#endif

            if (joint == nullptr || joint == parent) {
                continue;
            }

            auto joint_info = joint->info;

            if (joint_info == nullptr || joint_info->name == nullptr) {
                continue;
            }

            if (joint->info->parentJoint == ((sdk::Joint*)parent)->get_joint_index()) {
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

#if defined(RE8) || defined(MHRISE)
        if (transform.joints.data == nullptr) {
            return {};
        }
#endif

        std::vector<REJoint*> children{};

#if !defined(RE8) && !defined(MHRISE)
        for (int32_t i = 0; i < transform.joints.size; ++i) {
            auto joint = transform.joints.data->joints[i]; 
#else
        for (int32_t i = 0; i < transform.joints.data->numElements; ++i) {
            auto joint = utility::re_array::get_element<REJoint>(transform.joints.data, i);
#endif

            if (joint == nullptr || joint == parent) {
                continue;
            }

            auto joint_info = joint->info;

            if (joint_info == nullptr || joint_info->name == nullptr) {
                continue;
            }

            if (joint->info->parentJoint == ((sdk::Joint*)parent)->get_joint_index()) {
                children.push_back(joint);
            }
        }

        return children;
    }

    static std::vector<REJoint*> get_all_parents(const ::RETransform& transform, REJoint* child) {
        if (child->info == nullptr) {
            return {};
        }

        std::vector<REJoint*> parents{};

        auto parent = child->info->parentJoint;

        while (parent != -1) {
            auto joint = get_joint(transform, parent);

            if (joint != nullptr) {
                parents.push_back(joint);
                parent = joint->info->parentJoint;
            } else {
                break;
            }
        }

        return parents;
    }

    static glm::mat4 calculate_base_transform(const ::RETransform& transform, REJoint* target) {
        if (target == nullptr) {
            return glm::identity<glm::mat4>();
        }

        auto parent = target->info->parentJoint;

        if (parent == -1) {
            return glm::identity<glm::mat4>();
        }

        auto parent_joint = get_joint(transform, parent);

        if (parent_joint == nullptr) {
            return glm::identity<glm::mat4>();
        }

        auto parent_transform = calculate_base_transform(transform, parent_joint);

        glm::quat base_rotation{};
        sdk::call_object_func<glm::quat*>(target, "get_BaseLocalRotation", &base_rotation, sdk::get_thread_context(), target);

        Vector4f base_position{};
        sdk::call_object_func<Vector4f*>(target, "get_BaseLocalPosition", &base_position, sdk::get_thread_context(), target);

        // Convert to matrix
        auto base_transform = glm::translate(glm::mat4(1.0f), glm::vec3(base_position.x, base_position.y, base_position.z)) * glm::mat4_cast(base_rotation);

        return parent_transform * base_transform;
    }
}

namespace sdk {
static Vector4f get_transform_position(RETransform* transform) {
    Vector4f out{};

    sdk::call_object_func<Vector4f*>(transform, "get_Position", &out, sdk::get_thread_context(), transform);

    return out;
}

static glm::quat get_transform_rotation(RETransform* transform) {
    glm::quat out{};

    sdk::call_object_func<glm::quat*>(transform, "get_Rotation", &out, sdk::get_thread_context(), transform);

    return out;
}

static REJoint* get_joint_parent(REJoint* joint) {
    if (joint == nullptr || joint->info == nullptr || joint->info->parentJoint == -1) {
        return nullptr;
    }

    auto joint_transform = joint->parentTransform;

    // what da heck
    if (joint_transform == nullptr) {
        return nullptr;
    }

    return utility::re_transform::get_joint(*joint_transform, joint->info->parentJoint);
};

static void set_joint_position(REJoint* joint, const Vector4f& position) {
    sdk::call_object_func<void*>(joint, "set_Position", sdk::get_thread_context(), joint, &position);
};

static void set_joint_rotation(REJoint* joint, const glm::quat& rotation) {
    sdk::call_object_func<void*>(joint, "set_Rotation", sdk::get_thread_context(), joint, &rotation);
};

static glm::quat get_joint_rotation(REJoint* joint) {
    glm::quat rotation{};
    sdk::call_object_func<glm::quat*>(joint, "get_Rotation", &rotation, sdk::get_thread_context(), joint);
    return rotation;
};

static Vector4f get_joint_position(REJoint* joint) {
    Vector4f position{};
    sdk::call_object_func<Vector4f*>(joint, "get_Position", &position, sdk::get_thread_context(), joint);
    return position;
};

static glm::quat get_joint_local_rotation(REJoint* joint) {
    glm::quat rotation{};
    sdk::call_object_func<glm::quat*>(joint, "get_LocalRotation", &rotation, sdk::get_thread_context(), joint);
    return rotation;
};

static Vector4f get_joint_local_position(REJoint* joint) {
    Vector4f position{};
    sdk::call_object_func<Vector4f*>(joint, "get_LocalPosition", &position, sdk::get_thread_context(), joint);
    return position;
};

static void set_joint_local_rotation(REJoint* joint, const glm::quat& rotation) {
    sdk::call_object_func<void*>(joint, "set_LocalRotation", sdk::get_thread_context(), joint, &rotation);
};

static void set_joint_local_position(REJoint* joint, const Vector4f& position) {
    sdk::call_object_func<void*>(joint, "set_LocalPosition", sdk::get_thread_context(), joint, &position);
};
}