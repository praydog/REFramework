#pragma once
#include "..\Size.hpp"
#include "Window.hpp"
namespace regenny::via::gui {
#pragma pack(push, 1)
struct View : public Window {
    bool Overlay; // 0x1c0
    bool DepthTest; // 0x1c1
    bool DetoneMap; // 0x1c2
    bool CoordCorrection; // 0x1c3
    uint32_t ViewType; // 0x1c4
    private: char pad_1c8[0x4]; public:
    uint32_t ReprojectionType; // 0x1cc
    regenny::via::Size ScreenSize; // 0x1d0
    float BaseFps; // 0x1d8
    bool UseGUICamera; // 0x1dc
    private: char pad_1dd[0x3]; public:
}; // Size: 0x1e0
#pragma pack(pop)
}
