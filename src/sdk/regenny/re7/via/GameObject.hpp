#pragma once
#include "clr\ManagedObject.hpp"
namespace regenny::via {
struct Folder;
}
namespace regenny::via {
struct Transform;
}
namespace regenny::via {
#pragma pack(push, 1)
struct GameObject : public clr::ManagedObject {
    char pad_20[0x2];
    bool Update; // 0x22
    bool Draw; // 0x23
    bool UpdateSelf; // 0x24
    bool DrawSelf; // 0x25
    char pad_26[0x2];
    regenny::via::Transform* Transform; // 0x28
    regenny::via::Folder* folder; // 0x30
    char pad_38[0x14];
    float timescale; // 0x4c
}; // Size: 0x50
#pragma pack(pop)
}
