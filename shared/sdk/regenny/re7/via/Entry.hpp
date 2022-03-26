#pragma once
#include ".\Object.hpp"
namespace regenny::via {
#pragma pack(push, 1)
struct Entry : public Object {
    char pad_8[0xf8];
}; // Size: 0x100
#pragma pack(pop)
}
