#pragma once
#include <cstdint>

class REObjectInfo;

#pragma pack(push, 1)
class REObject {
public:
    REObjectInfo* info;  // 0x00 — runtime type information
};
static_assert(sizeof(REObject) == 0x08);
#pragma pack(pop)
