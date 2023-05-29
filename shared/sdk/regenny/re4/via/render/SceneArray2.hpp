#pragma once
namespace regenny::via {
struct Scene;
}
namespace regenny::via::render {
#pragma pack(push, 1)
struct SceneArray2 {
    regenny::via::Scene** elements; // 0x0
    uint32_t count; // 0x8
}; // Size: 0xc
#pragma pack(pop)
}
