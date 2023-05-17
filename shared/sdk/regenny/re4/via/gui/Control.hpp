#pragma once
#include "TransformObject.hpp"
#include "..\vec3.hpp"
#include "..\vec4.hpp"
namespace regenny::via::gui {
#pragma pack(push, 1)
struct Control : public TransformObject {
    char pad_120[0x4];
    uint32_t StatePattern; // 0x124
    float PlayFrame; // 0x128
    regenny::via::vec4 ColorScale; // 0x12c
    regenny::via::vec3 ColorOffset; // 0x13c
    float Saturation; // 0x148
    char pad_14c[0x3c];
    bool UseGUISound; // 0x188
    char pad_189[0x27];
}; // Size: 0x1b0
#pragma pack(pop)
}
