#pragma once
#include "clr\ManagedObject.hpp"
namespace regenny::via {
struct Transform;
}
namespace regenny::via {
struct Folder;
}
namespace regenny::via {
#pragma pack(push, 1)
struct GameObject : public clr::ManagedObject {
    private: char pad_10[0x2]; public:
    bool Update; // 0x12
    bool Draw; // 0x13
    bool UpdateSelf; // 0x14
    bool DrawSelf; // 0x15
    private: char pad_16[0x2]; public:
    regenny::via::Transform* Transform; // 0x18
    regenny::via::Folder* folder; // 0x20
    private: char pad_28[0x24]; public:
    float timescale; // 0x4c
}; // Size: 0x50
#pragma pack(pop)
}
