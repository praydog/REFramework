#pragma once
#include ".\mat4.hpp"
#include ".\vec4.hpp"
#include ".\JointArray.hpp"
#include ".\Component.hpp"
namespace regenny::via {
struct Scene;
}
namespace regenny::via {
struct Transform;
}
namespace regenny::via {
#pragma pack(push, 1)
struct Transform : public Component {
    regenny::via::vec4 Position; // 0x40
    regenny::via::vec4 Rotation; // 0x50
    regenny::via::vec4 Scale; // 0x60
    regenny::via::Scene* Scene; // 0x70
    regenny::via::Transform* Child; // 0x78
    regenny::via::Transform* Next; // 0x80
    regenny::via::Transform* Parent; // 0x88
    regenny::via::mat4 WorldTransform; // 0x90
    void* JointHashTbl; // 0xd0
    int32_t ParentJointIndex; // 0xd8
    int32_t UpdateFrame; // 0xdc
    bool SameJointsConstraint; // 0xe0
    bool DirtySelf; // 0xe1
    bool DirtyUpwards; // 0xe2
    bool AbsoluteScaling; // 0xe3
    char pad_e4[0x4];
    regenny::via::JointArray JointTbl; // 0xe8
    regenny::via::mat4* JointMatrixTbl; // 0xf8
    char pad_100[0x10];
}; // Size: 0x110
#pragma pack(pop)
}
