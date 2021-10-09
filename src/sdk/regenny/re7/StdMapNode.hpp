#pragma once
#include ".\StdMapData.hpp"
namespace regenny {
struct StdMapNode;
}
namespace regenny {
#pragma pack(push, 1)
struct StdMapNode {
    regenny::StdMapNode* _Left; // 0x0
    regenny::StdMapNode* _Parent; // 0x8
    regenny::StdMapNode* _Right; // 0x10
    void* unk; // 0x18
    regenny::StdMapData data; // 0x20
}; // Size: 0x98
#pragma pack(pop)
}
