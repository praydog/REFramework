#pragma once
#include "..\mat4.hpp"
#include "..\vec3.hpp"
#include "..\vec4.hpp"
namespace regenny::via::render {
#pragma pack(push, 1)
struct SceneInfo {
    regenny::via::mat4 view_proj_matrix; // 0x0
    regenny::via::mat4 view_matrix; // 0x40
    regenny::via::mat4 inverse_view_matrix; // 0x80
    regenny::via::mat4 projection_matrix; // 0xc0
    regenny::via::mat4 inverse_projection_matrix; // 0x100
    regenny::via::mat4 inverse_view_projection_matrix; // 0x140
    regenny::via::mat4 old_view_projection_matrix; // 0x180
    regenny::via::vec3 camera_pos; // 0x1c0
    float z_near; // 0x1cc
    regenny::via::vec3 camera_dir; // 0x1d0
    float z_far; // 0x1dc
    regenny::via::vec4 view_frustum[6]; // 0x1e0
    regenny::via::vec3 z_linear; // 0x240
    float pixel_cull; // 0x24c
    float screen_size[2]; // 0x250
    float screen_size_inverse[2]; // 0x258
    regenny::via::vec4 clip_plane; // 0x260
    float subdivision; // 0x270
    float variable_rate_shading_threshold; // 0x274
}; // Size: 0x278
#pragma pack(pop)
}
