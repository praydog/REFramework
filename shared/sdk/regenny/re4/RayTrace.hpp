#pragma once
#include "RenderEntity.hpp"
namespace regenny {
struct RTInternal;
}
namespace regenny {
#pragma pack(push, 1)
struct RayTrace : public RenderEntity {
    regenny::RTInternal* impl; // 0x498
    char pad_4a0[0xb60];
}; // Size: 0x1000
#pragma pack(pop)
}
