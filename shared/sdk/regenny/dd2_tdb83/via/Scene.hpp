#pragma once
#include "clr\ManagedObject.hpp"
namespace regenny::via {
#pragma pack(push, 1)
struct Scene : public clr::ManagedObject {
    char asdf; // 0x10
    private: char pad_11[0xf]; public:
    float timescale; // 0x20
}; // Size: 0x24
#pragma pack(pop)
}
