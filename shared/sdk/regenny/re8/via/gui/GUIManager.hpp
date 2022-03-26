#pragma once
#include "..\JobSet.hpp"
namespace regenny::via::gui {
#pragma pack(push, 1)
struct GUIManager {
    char pad_0[0xe0];
    regenny::via::JobSet update_task; // 0xe0
    char pad_e8[0x38];
    regenny::via::JobSet draw_task; // 0x120
    char pad_128[0x78];
}; // Size: 0x1a0
#pragma pack(pop)
}
