#pragma once
#include "clr\ManagedObject.hpp"
#include "vec4.hpp"
namespace regenny::via {
struct Transform;
}
namespace regenny::via {
struct JointDesc;
}
namespace regenny::via {
#pragma pack(push, 1)
struct Joint : public clr::ManagedObject {
    regenny::via::Transform* Owner; // 0x10
    regenny::via::JointDesc* Desc; // 0x18
    regenny::via::vec4 LocalPosition; // 0x20
    regenny::via::vec4 LocalRotation; // 0x30
    regenny::via::vec4 LocalScale; // 0x40
    int32_t ConstraintJointIndex; // 0x50
    int32_t JointIndex; // 0x54
    private: char pad_58[0x8]; public:
}; // Size: 0x60
#pragma pack(pop)
}
