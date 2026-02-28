#pragma once
namespace regenny::via {
#pragma pack(push, 1)
struct Camera {
    private: char pad_0[0x48]; public:
    int32_t priority; // 0x48
    private: char pad_4c[0xfb4]; public:
}; // Size: 0x1000
#pragma pack(pop)
}
