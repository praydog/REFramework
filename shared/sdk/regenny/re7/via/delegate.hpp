#pragma once
#include "..\StdMap.hpp"
namespace regenny::via {
#pragma pack(push, 1)
struct delegate {
    void* unk; // 0x0
    regenny::StdMap functions; // 0x8
    void* mutex; // 0x18
}; // Size: 0x20
#pragma pack(pop)
}
