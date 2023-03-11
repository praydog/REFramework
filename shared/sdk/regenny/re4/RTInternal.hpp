#pragma once
#include "via\vec3.hpp"
namespace regenny {
#pragma pack(push, 1)
struct RTInternal {
    char pad_0[0x168];
    // Metadata: utf8*
    char* name; // 0x168
    char pad_170[0x10];
    uint32_t unkint; // 0x180
    uint32_t unkint2; // 0x184
    uint32_t RayTracingMode; // 0x188
    uint32_t Spp; // 0x18c
    uint8_t unk2; // 0x190
    char pad_191[0xb];
    regenny::via::vec3 params; // 0x19c
    char pad_1a8[0x4];
    bool use_transparent; // 0x1ac
    char pad_1ad[0x1];
    bool specular_secondary_bounce; // 0x1ae
    bool use_no_movement_depth_comp; // 0x1af
    float normal_compare_thresh; // 0x1b0
    bool use_diffuse_bounce_intensity; // 0x1b4
    char pad_1b5[0x3];
    float diffuse_bounce_intensity_mult; // 0x1b8
    bool use_specular_bounce_intensity; // 0x1bc
    char pad_1bd[0x3];
    float specular_bounce_intensity; // 0x1c0
    bool ray_trace_ambient_occlusion; // 0x1c4
    char pad_1c5[0x3];
    float ray_trace_ratio; // 0x1c8
    float unkflooat; // 0x1cc
    uint32_t sampling_pattern; // 0x1d0
    char pad_1d4[0x8];
    float surfel_size; // 0x1dc
    float asdfasdgadsg; // 0x1e0
    char pad_1e4[0xe1c];
}; // Size: 0x1000
#pragma pack(pop)
}
