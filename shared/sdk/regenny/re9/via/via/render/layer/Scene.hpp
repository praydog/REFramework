#pragma once
#include "..\RenderLayer.hpp"
namespace regenny::via {
struct Camera;
}
namespace regenny::via::via::render {
struct Mirror;
}
namespace regenny::via::via::render {
struct TargetState;
}
namespace regenny::via::via::render::layer {
#pragma pack(push, 1)
struct Scene : public regenny::via::via::render::RenderLayer {
    regenny::via::Camera* camera; // 0x88
    regenny::via::via::render::Mirror* mirror; // 0x90
    regenny::via::via::render::TargetState* buffer_pre_z; // 0x98
    private: char pad_a0[0x18]; public:
    regenny::via::via::render::TargetState* hdr_target; // 0xb8
    regenny::via::via::render::TargetState* hdr_target_with_velocity; // 0xc0
    regenny::via::via::render::TargetState* hdr_target_with_user_attribute_custom_params; // 0xc8
    regenny::via::via::render::TargetState* hdr_target_with_readonly_depth; // 0xd0
    private: char pad_d8[0x30]; public:
    regenny::via::via::render::TargetState* depth_target; // 0x108
    private: char pad_110[0x8]; public:
    regenny::via::via::render::TargetState* velocity_target; // 0x118
    regenny::via::via::render::TargetState* g_buffer_target; // 0x120
    regenny::via::via::render::TargetState* non_decal_g_buffer_target; // 0x128
    private: char pad_130[0x8]; public:
    regenny::via::via::render::TargetState* post_main_target; // 0x138
    regenny::via::via::render::TargetState* post_depth_target; // 0x140
    private: char pad_148[0x16d0]; public:
    float lod_bias[2]; // 0x1818
    private: char pad_1820[0x2a0]; public:
}; // Size: 0x1ac0
#pragma pack(pop)
}
