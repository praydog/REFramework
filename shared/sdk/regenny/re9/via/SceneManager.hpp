#pragma once
#include "SceneArray.hpp"
namespace regenny::via {
struct SceneView;
}
namespace regenny::via {
struct Scene;
}
namespace regenny::via {
#pragma pack(push, 1)
struct SceneManager {
    private: char pad_0[0x48]; public:
    regenny::via::SceneView* main_view; // 0x48
    regenny::via::Scene* main_scene; // 0x50
    private: char pad_58[0x20]; public:
    regenny::via::SceneArray scenes; // 0x78
    private: char pad_88[0x150]; public:
}; // Size: 0x1d8
#pragma pack(pop)
}
