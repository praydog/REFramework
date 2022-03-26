#pragma once
#include "..\via\motion\AxisDirection.hpp"
#include "..\via\Range.hpp"
#include "..\via\mat4.hpp"
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
        regenny::via::Joint* ApplyJoint; // 0x20
        regenny::via::motion::AxisDirection Dir; // 0x28
        regenny::via::motion::AxisDirection Up; // 0x2c
        regenny::via::motion::AxisDirection Rot; // 0x30
        float L0; // 0x34
        float L1; // 0x38
    }; // Size: 0x3c

    struct ArmData : public regenny::via::clr::ManagedObject {
        float BlendRate; // 0x20
        regenny::via::Range ReachRate; // 0x24
        char pad_2c[0x4];
        regenny::System::String* ApplyHandName; // 0x30
        bool LeftHand; // 0x38
        char pad_39[0x3];
        float DefaultHandAngle; // 0x3c
        regenny::via::Range AxisYRange; // 0x40
        regenny::via::Range AxisZRange; // 0x48
        regenny::via::mat4 TargetMatrix; // 0x50
    }; // Size: 0x90

    char pad_0[0x100];
}; // Size: 0x100
#pragma pack(pop)
}
