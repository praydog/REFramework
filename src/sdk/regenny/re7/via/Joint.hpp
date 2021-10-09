#pragma once
#include "clr\ManagedObject.hpp"
#include ".\vec4.hpp"
namespace regenny::via {
struct Transform;
}
namespace regenny::via {
struct JointDesc;
}
namespace regenny::via {
#pragma pack(push, 1)
struct Joint : public clr::ManagedObject {
    regenny::via::Transform* Owner; // 0x20
    regenny::via::JointDesc* Desc; // 0x28
    regenny::via::vec4 LocalPosition; // 0x30
    regenny::via::vec4 LocalRotation; // 0x40
    regenny::via::vec4 LocalScale; // 0x50
    int32_t ConstraintJointIndex; // 0x60
    int32_t JointIndex; // 0x64
}; // Size: 0x68
#pragma pack(pop)
}
