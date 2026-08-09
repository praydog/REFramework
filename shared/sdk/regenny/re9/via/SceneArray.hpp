#pragma once
namespace regenny::via {
struct Scene;
}
namespace regenny::via {
#pragma pack(push, 1)
struct SceneArray {
    int32_t count; // 0x0
    private: char pad_4[0x4]; public:
    regenny::via::Scene** elements; // 0x8
}; // Size: 0x10
#pragma pack(pop)
}
