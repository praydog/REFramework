#pragma once
#include "..\Component.hpp"
#include "SceneArray2.hpp"
namespace regenny::via::render {
#pragma pack(push, 1)
struct RenderOutput : public regenny::via::Component {
    char pad_30[0x68];
    regenny::via::render::SceneArray2 scenes; // 0x98
    char pad_a4[0xf5c];
}; // Size: 0x1000
#pragma pack(pop)
}
