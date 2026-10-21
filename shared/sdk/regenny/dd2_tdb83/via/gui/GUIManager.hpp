#pragma once
#include "..\JobSet.hpp"
namespace regenny::via::gui {
#pragma pack(push, 1)
struct GUIManager {
    private: char pad_0[0xe0]; public:
    regenny::via::JobSet update_task; // 0xe0
    private: char pad_e8[0x38]; public:
    regenny::via::JobSet draw_task; // 0x120
    private: char pad_128[0x78]; public:
}; // Size: 0x1a0
#pragma pack(pop)
}
