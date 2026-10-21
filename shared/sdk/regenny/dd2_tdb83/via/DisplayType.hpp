#pragma once
namespace regenny::via {
#pragma pack(push, 1)
enum class DisplayType : uint32_t {
    Fit = 0,
    Uniform4x3 = 1,
    Uniform16x9 = 2,
    Uniform16x10 = 3,
    Uniform21x9 = 4,
    Uniform32x9 = 5,
    Uniform48x9 = 6,
    Fix480p = 7,
    Fix720p = 8,
    Fix1080p = 9,
    Fix4K = 10,
    Fix8K = 11,
    FixResolution = 12,
    FixResolution16x9 = 13,
    NintendoSwitch = 14,
    Fix684x384 = 15,
    Fix1368x768 = 16,
    Nexus6P = 17,
    GalaxyS9 = 18,
    XperiaZ5Premium = 19,
    iPhoneX = 20,
    iPhone7 = 21,
    ZenPadS8 = 22,
};
#pragma pack(pop)
}
