#pragma once
#include "..\..\Component.hpp"
#include "SceneArray2.hpp"
namespace regenny::via::via::render {
#pragma pack(push, 1)
struct RenderOutput : public regenny::via::Component {
    private: char pad_30[0x68]; public:
    regenny::via::via::render::SceneArray2 scenes; // 0x98
    private: char pad_a4[0xf5c]; public:
}; // Size: 0x1000
#pragma pack(pop)
}
