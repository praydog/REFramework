#pragma once
#include "..\via\Range.hpp"
#include "..\via\clr\ManagedObject.hpp"
#include "..\via\mat4.hpp"
#include "..\via\motion\AxisDirection.hpp"
namespace regenny::via {
struct Joint;
}
namespace regenny::System {
struct String;
}
namespace regenny::game {
#pragma pack(push, 1)
struct IkArmFit {
    struct ArmSolver : public regenny::via::clr::ManagedObject {
        regenny::via::Joint* ApplyJoint; // 0x10
        regenny::via::motion::AxisDirection Dir; // 0x18
        regenny::via::motion::AxisDirection Up; // 0x1c
        regenny::via::motion::AxisDirection Rot; // 0x20
        float L0; // 0x24
        float L1; // 0x28
        private: char pad_2c[0x4]; public:
    }; // Size: 0x30

    struct ArmData : public regenny::via::clr::ManagedObject {
        float BlendRate; // 0x10
        regenny::via::Range ReachRate; // 0x14
        private: char pad_1c[0x4]; public:
        regenny::System::String* ApplyHandName; // 0x20
        bool LeftHand; // 0x28
        private: char pad_29[0x3]; public:
        float DefaultHandAngle; // 0x2c
        regenny::via::Range AxisYRange; // 0x30
        regenny::via::Range AxisZRange; // 0x38
        regenny::via::mat4 TargetMatrix; // 0x40
    }; // Size: 0x80

    private: char pad_0[0x100]; public:
}; // Size: 0x100
#pragma pack(pop)
}
