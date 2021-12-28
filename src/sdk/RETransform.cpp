#include "Enums_Internal.hpp"

#include "RETransform.hpp"

namespace sdk {
Vector4f sdk::get_transform_position(RETransform* transform) {
    static auto get_position_method = sdk::RETypeDB::get()->find_type("via.Transform")->get_method("get_Position");

    Vector4f out{};
    get_position_method->call<Vector4f*>(&out, sdk::get_thread_context(), transform);

    return out;
}

glm::quat sdk::get_transform_rotation(RETransform* transform) {
    static auto get_rotation_method = sdk::RETypeDB::get()->find_type("via.Transform")->get_method("get_Rotation");

    glm::quat out{};
    get_rotation_method->call<glm::quat*>(&out, sdk::get_thread_context(), transform);

    return out;
}

void set_transform_position(RETransform* transform, const Vector4f& pos) {
    static auto set_position_method = sdk::RETypeDB::get()->find_type("via.Transform")->get_method("set_Position");

    set_position_method->call<void*>(sdk::get_thread_context(), transform, &pos);
}

void set_transform_rotation(RETransform* transform, const glm::quat& rot) {
    static auto set_rotation_method = sdk::RETypeDB::get()->find_type("via.Transform")->get_method("set_Rotation");

    set_rotation_method->call<void*>(sdk::get_thread_context(), transform, &rot);
}

REJoint* sdk::get_joint_parent(REJoint* joint) {
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

void sdk::set_joint_position(REJoint* joint, const Vector4f& position) {
    static auto set_position_method = sdk::RETypeDB::get()->find_type("via.Joint")->get_method("set_Position");

    set_position_method->call<void*>(sdk::get_thread_context(), joint, &position);
};

void sdk::set_joint_rotation(REJoint* joint, const glm::quat& rotation) {
    static auto set_rotation_method = sdk::RETypeDB::get()->find_type("via.Joint")->get_method("set_Rotation");

    set_rotation_method->call<void*>(sdk::get_thread_context(), joint, &rotation);
};

glm::quat sdk::get_joint_rotation(REJoint* joint) {
    static auto get_rotation_method = sdk::RETypeDB::get()->find_type("via.Joint")->get_method("get_Rotation");

    glm::quat rotation{};
    get_rotation_method->call<glm::quat*>(&rotation, sdk::get_thread_context(), joint);

    return rotation;
};

Vector4f sdk::get_joint_position(REJoint* joint) {
    static auto get_position_method = sdk::RETypeDB::get()->find_type("via.Joint")->get_method("get_Position");

    Vector4f position{};
    get_position_method->call<Vector4f*>(&position, sdk::get_thread_context(), joint);

    return position;
};

glm::quat sdk::get_joint_local_rotation(REJoint* joint) {
    static auto get_local_rotation_method = sdk::RETypeDB::get()->find_type("via.Joint")->get_method("get_LocalRotation");

    glm::quat rotation{};
    get_local_rotation_method->call<glm::quat*>(&rotation, sdk::get_thread_context(), joint);

    return rotation;
};

Vector4f sdk::get_joint_local_position(REJoint* joint) {
    static auto get_local_position_method = sdk::RETypeDB::get()->find_type("via.Joint")->get_method("get_LocalPosition");

    Vector4f position{};
    get_local_position_method->call<Vector4f*>(&position, sdk::get_thread_context(), joint);

    return position;
};

void sdk::set_joint_local_rotation(REJoint* joint, const glm::quat& rotation) {
    static auto set_local_rotation_method = sdk::RETypeDB::get()->find_type("via.Joint")->get_method("set_LocalRotation");

    set_local_rotation_method->call<void*>(sdk::get_thread_context(), joint, &rotation);
};

void sdk::set_joint_local_position(REJoint* joint, const Vector4f& position) {
    static auto set_local_position_method = sdk::RETypeDB::get()->find_type("via.Joint")->get_method("set_LocalPosition");

    set_local_position_method->call<void*>(sdk::get_thread_context(), joint, &position);
};
}


namespace utility::re_transform {
glm::mat4 re_transform::calculate_base_transform(const ::RETransform& transform, REJoint* target) {
    static auto get_base_local_rotation_method = sdk::RETypeDB::get()->find_type("via.Joint")->get_method("get_BaseLocalRotation");
    static auto get_base_local_position_method = sdk::RETypeDB::get()->find_type("via.Joint")->get_method("get_BaseLocalPosition");

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
    get_base_local_rotation_method->call<glm::quat*>(&base_rotation, sdk::get_thread_context(), target);

    Vector4f base_position{};
    get_base_local_position_method->call<Vector4f*>(&base_position, sdk::get_thread_context(), target);

    // Convert to matrix
    auto base_transform = glm::translate(glm::mat4(1.0f), glm::vec3(base_position.x, base_position.y, base_position.z)) * glm::mat4_cast(base_rotation);

    return parent_transform * base_transform;
}
}