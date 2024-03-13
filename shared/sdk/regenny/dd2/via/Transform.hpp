#pragma once
#include "Component.hpp"
#include "mat4.hpp"
#include "vec4.hpp"
namespace regenny::via {
struct Scene;
}
namespace regenny::via {
struct JointArray;
}
namespace regenny::via {
#pragma pack(push, 1)
struct Transform : public Component {
    regenny::via::vec4 Position; // 0x30
    regenny::via::vec4 Rotation; // 0x40
    regenny::via::vec4 Scale; // 0x50
    regenny::via::Scene* Scene; // 0x60
    regenny::via::Transform* Child; // 0x68
    regenny::via::Transform* Next; // 0x70
    regenny::via::Transform* Parent; // 0x78
    regenny::via::mat4 WorldTransform; // 0x80
    void* JointHashTbl; // 0xc0
    int32_t ParentJointIndex; // 0xc8
    int32_t UpdateFrame; // 0xcc
    bool SameJointsConstraint; // 0xd0
    bool DirtySelf; // 0xd1
    bool DirtyUpwards; // 0xd2
    bool AbsoluteScaling; // 0xd3
    private: char pad_d4[0x4]; public:
    regenny::via::JointArray* JointTbl; // 0xd8
    regenny::via::mat4* JointMatrixTbl; // 0xe0
    private: char pad_e8[0x28]; public:
}; // Size: 0x110
#pragma pack(pop)
}
