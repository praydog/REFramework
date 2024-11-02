#include <sdk/REMath.hpp>
#include <spdlog/spdlog.h>

#include "Enums_Internal.hpp"
#include "REString.hpp"
#include "RETransform.hpp"

namespace sdk {
Vector4f sdk::get_transform_position(RETransform* transform) {
    static auto get_position_method = sdk::find_type_definition("via.Transform")->get_method("get_Position");

    __declspec(align(16)) Vector4f out{};
    get_position_method->call<Vector4f*>(&out, sdk::get_thread_context(), transform);

    return out;
}

glm::quat sdk::get_transform_rotation(RETransform* transform) {
    static auto get_rotation_method = sdk::find_type_definition("via.Transform")->get_method("get_Rotation");

    __declspec(align(16)) glm::quat out{};
    get_rotation_method->call<glm::quat*>(&out, sdk::get_thread_context(), transform);

    return out;
}

REJoint* get_transform_joint_by_hash(RETransform* transform, uint32_t hash) {
    static auto get_joint_by_hash_method = sdk::find_type_definition("via.Transform")->get_method("getJointByHash");
    
    return get_joint_by_hash_method->call<REJoint*>(sdk::get_thread_context(), transform, hash);
}

REJoint* get_transform_joint_by_name(RETransform* transform, std::wstring_view name) {
    static auto get_joint_by_name_method = sdk::find_type_definition("via.Transform")->get_method("getJointByName");

    return get_joint_by_name_method->call<REJoint*>(sdk::get_thread_context(), transform, sdk::VM::create_managed_string(name));
}

sdk::SystemArray* get_transform_joints(RETransform* transform) {
    static auto get_joints_method = sdk::find_type_definition("via.Transform")->get_method("get_Joints");

    return get_joints_method->call<sdk::SystemArray*>(sdk::get_thread_context(), transform);
}

void set_transform_position(RETransform* transform, const Vector4f& pos, bool no_dirty) {
    if (!no_dirty) {
        static auto set_position_method = sdk::find_type_definition("via.Transform")->get_method("set_Position");

        __declspec(align(16)) Vector4f p = pos;
        set_position_method->call<void*>(sdk::get_thread_context(), transform, &p);
    } else {
        static auto get_parent_method = sdk::find_type_definition("via.Transform")->get_method("get_Parent");
        const auto parent_transform = get_parent_method->call<RETransform*>(sdk::get_thread_context(), transform);

        if (parent_transform != nullptr) {
            const auto parent_position = sdk::get_transform_position(parent_transform);
            const auto parent_rotation = sdk::get_transform_rotation(parent_transform);
            const auto local_diff = pos - parent_position;
            
            transform->position = glm::vec4{glm::inverse(parent_rotation) * glm::vec3{local_diff}, 1.0f};
        } else {
            transform->position = pos;
        }

        transform->worldTransform[3] = pos;
        transform->worldTransform[3].w = 1.0f;
    }
}

void set_transform_rotation(RETransform* transform, const glm::quat& rot) {
    static auto set_rotation_method = sdk::find_type_definition("via.Transform")->get_method("set_Rotation");

    __declspec(align(16)) glm::quat q = rot;
    set_rotation_method->call<void*>(sdk::get_thread_context(), transform, &q);
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

::RETransform* sdk::get_joint_owner(::REJoint* joint) {
    if (joint == nullptr) {
        return nullptr;
    }

    static auto get_owner_method = sdk::find_method_definition("via.Joint", "get_Owner");

    return get_owner_method->call<::RETransform*>(sdk::get_thread_context(), joint);
}

uint32_t get_joint_hash(REJoint* joint) {
    static auto get_hash_method = sdk::find_type_definition("via.Joint")->get_method("get_NameHash");

    return get_hash_method->call<uint32_t>(sdk::get_thread_context(), joint);
}

void sdk::set_joint_position(REJoint* joint, const Vector4f& position) {
    static auto set_position_method = sdk::find_type_definition("via.Joint")->get_method("set_Position");

    __declspec(align(16)) Vector4f pos = position;
    set_position_method->call<void*>(sdk::get_thread_context(), joint, &pos);
};

void sdk::set_joint_rotation(REJoint* joint, const glm::quat& rotation) {
    static auto set_rotation_method = sdk::find_type_definition("via.Joint")->get_method("set_Rotation");

    __declspec(align(16)) glm::quat rot = rotation;
    set_rotation_method->call<void*>(sdk::get_thread_context(), joint, &rot);
};

glm::quat sdk::get_joint_rotation(REJoint* joint) {
    static auto get_rotation_method = sdk::find_type_definition("via.Joint")->get_method("get_Rotation");

    __declspec(align(16)) glm::quat rotation{};
    get_rotation_method->call<glm::quat*>(&rotation, sdk::get_thread_context(), joint);

    return rotation;
};

Vector4f sdk::get_joint_position(REJoint* joint) {
    static auto get_position_method = sdk::find_type_definition("via.Joint")->get_method("get_Position");

    __declspec(align(16)) Vector4f position{};
    get_position_method->call<Vector4f*>(&position, sdk::get_thread_context(), joint);

    return position;
};

glm::quat sdk::get_joint_local_rotation(REJoint* joint) {
    static auto get_local_rotation_method = sdk::find_type_definition("via.Joint")->get_method("get_LocalRotation");

    __declspec(align(16)) glm::quat rotation{};
    get_local_rotation_method->call<glm::quat*>(&rotation, sdk::get_thread_context(), joint);

    return rotation;
};

Vector4f sdk::get_joint_local_position(REJoint* joint) {
    static auto get_local_position_method = sdk::find_type_definition("via.Joint")->get_method("get_LocalPosition");

    __declspec(align(16)) Vector4f position{};
    get_local_position_method->call<Vector4f*>(&position, sdk::get_thread_context(), joint);

    return position;
};

void sdk::set_joint_local_rotation(REJoint* joint, const glm::quat& rotation) {
    static auto set_local_rotation_method = sdk::find_type_definition("via.Joint")->get_method("set_LocalRotation");

    __declspec(align(16)) glm::quat rot = rotation;
    set_local_rotation_method->call<void*>(sdk::get_thread_context(), joint, &rot);
};

void sdk::set_joint_local_position(REJoint* joint, const Vector4f& position) {
    static auto set_local_position_method = sdk::find_type_definition("via.Joint")->get_method("set_LocalPosition");

    __declspec(align(16)) Vector4f pos = position;
    set_local_position_method->call<void*>(sdk::get_thread_context(), joint, &pos);
};

std::string sdk::get_joint_name(REJoint* joint) {
    static auto get_name_method = sdk::find_type_definition("via.Joint")->get_method("get_Name");

    auto sname = get_name_method->call<SystemString*>(sdk::get_thread_context(), joint);

    std::string name{};

    if (sname != nullptr) {
        name = utility::re_string::get_string(*sname);
    }

    return name;
}
}


namespace utility::re_transform {
REJoint* get_joint(const ::RETransform& transform, uint32_t index) {
#if TDB_VER < 69
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
    static auto get_joints_method = sdk::find_method_definition("via.Transform", "get_Joints");
    auto joints = get_joints_method->call<REArrayBase*>(sdk::get_thread_context(), &transform);

    return utility::re_array::get_element<REJoint>(joints, index);
#endif
}

glm::mat4 calculate_base_transform(const ::RETransform& transform, REJoint* target) {
    static auto get_base_local_rotation_method = sdk::find_type_definition("via.Joint")->get_method("get_BaseLocalRotation");
    static auto get_base_local_position_method = sdk::find_type_definition("via.Joint")->get_method("get_BaseLocalPosition");

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

    __declspec(align(16)) glm::quat base_rotation{};
    get_base_local_rotation_method->call<glm::quat*>(&base_rotation, sdk::get_thread_context(), target);

    __declspec(align(16)) Vector4f base_position{};
    get_base_local_position_method->call<Vector4f*>(&base_position, sdk::get_thread_context(), target);

    // Convert to matrix
    auto base_transform = glm::translate(glm::mat4(1.0f), glm::vec3(base_position.x, base_position.y, base_position.z)) * glm::mat4_cast(base_rotation);

    return parent_transform * base_transform;
}

void calculate_base_transforms(const ::RETransform& transform, REJoint* target, std::unordered_map<REJoint*, glm::mat4>& out) {
    static auto get_base_local_rotation_method = sdk::find_type_definition("via.Joint")->get_method("get_BaseLocalRotation");
    static auto get_base_local_position_method = sdk::find_type_definition("via.Joint")->get_method("get_BaseLocalPosition");

    if (auto it = out.find(target); it != out.end()) {
        return;
    }

    if (target == nullptr) {
        out[target] = glm::identity<glm::mat4>();
        return;
    }

    auto parent = target->info->parentJoint;

    if (parent == -1) {
        out[target] = glm::identity<glm::mat4>();
        return;
    }

    auto parent_joint = get_joint(transform, parent);

    if (parent_joint == nullptr) {
        out[target] = glm::identity<glm::mat4>();
        return;
    }

    calculate_base_transforms(transform, parent_joint, out);

    auto it = out.find(parent_joint);

    if (it == out.end()) {
        out[target] = glm::identity<glm::mat4>();
        return;
    }

    const auto& parent_transform = it->second;

    __declspec(align(16)) glm::quat base_rotation{};
    get_base_local_rotation_method->call<glm::quat*>(&base_rotation, sdk::get_thread_context(), target);

    __declspec(align(16)) Vector4f base_position{};
    get_base_local_position_method->call<Vector4f*>(&base_position, sdk::get_thread_context(), target);

    // Convert to matrix
    const auto base_transform = glm::translate(glm::mat4(1.0f), glm::vec3(base_position.x, base_position.y, base_position.z)) * glm::mat4_cast(base_rotation);

    out[target] = parent_transform * base_transform;
}

Vector4f calculate_tpose_pos_world(::RETransform& transform, REJoint* joint, uint32_t depth) {
    if (depth == 0) {
        depth = 1;
    }

    const auto player_pos = sdk::get_transform_position(&transform);
    const auto player_rot = sdk::get_transform_rotation(&transform);

    std::vector<::REJoint*> joints{};

    auto cur_joint = joint;

    for (auto i = 0; i < depth; ++i) {
        cur_joint = sdk::get_joint_parent(cur_joint);
        joints.push_back(cur_joint);
    }

    std::unordered_map<REJoint*, glm::mat4> known_joints{};
    utility::re_transform::calculate_base_transforms(transform, joint, known_joints);
    utility::re_transform::calculate_base_transforms(transform, cur_joint, known_joints);

    auto parent_pos = sdk::get_joint_position(cur_joint);
    auto parent_rot = sdk::get_joint_rotation(cur_joint);
    auto original_parent_pos = player_pos + (player_rot * known_joints[cur_joint][3]);

    for (auto i = 0; i < depth; ++i) {
        auto joint = joints[depth-i];

        utility::re_transform::calculate_base_transforms(transform, cur_joint, known_joints);

        auto original_pos = player_pos + (player_rot * known_joints[joint][3]);
        const auto diff = original_pos - original_parent_pos;
        const auto updated_pos = parent_pos + diff;
        
        original_parent_pos = original_pos;
        parent_pos = updated_pos;
    }

    const auto original_pos = player_pos + (player_rot * known_joints[joint][3]);
    const auto diff = original_pos - original_parent_pos;

    return parent_pos + diff;
}

void apply_joints_tpose(::RETransform& transform, const std::vector<REJoint*>& joints_initial, uint32_t additional_parents) {
    if (joints_initial.empty() || joints_initial[0] == nullptr) {
        spdlog::info("No joints to apply tpose");
        return;
    }
    
    auto joints = joints_initial;

    auto player_pos = sdk::get_transform_position(&transform);
    auto player_rot = sdk::get_transform_rotation(&transform);

    joints.insert(joints.begin(), sdk::get_joint_parent(joints[0]));

    for (auto i = 0; i < additional_parents; i++) {
        auto parent = sdk::get_joint_parent(joints[0]);

        if (parent == nullptr) {
            break;
        }

        joints.insert(joints.begin(), parent);
    }

    std::vector<Vector3f> original_positions(joints.size());
    std::vector<glm::quat> original_rotations(joints.size());
    std::vector<Vector3f> current_positions(joints.size());

    std::unordered_map<REJoint*, glm::mat4> base_transforms{};

    for (auto i = 0; i < joints.size(); i++) {
        auto joint = joints[i];

        if (joint == nullptr) {
            continue;
        }

        utility::re_transform::calculate_base_transforms(transform, joint, base_transforms);

        auto it = base_transforms.find(joint);

        if (it == base_transforms.end()) {
            continue;
        }

        const auto& base_transform = it->second;

        original_positions[i] = player_pos + (player_rot * base_transform[3]);
        original_rotations[i] = player_rot * glm::quat_cast(base_transform);
        current_positions[i] = sdk::get_joint_position(joints[i]);
    }

    // second pass
    for (auto i = 0; i < joints.size() - 1; i++) {
        auto joint = joints[i];

        if (joint == nullptr) {
            continue;
        }

        auto next_joint = joints[i + 1];

        if (next_joint != nullptr) {
            auto diff = original_positions[i + 1] - original_positions[i];
            auto updated_pos = Vector4f{current_positions[i] + diff, 1.0f};

            sdk::set_joint_position(next_joint, updated_pos);
            sdk::set_joint_rotation(next_joint, original_rotations[i+1]);

            current_positions[i + 1] = updated_pos;
        }
    }
}
}