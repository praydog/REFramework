#pragma once
#include "..\RenderLayer.hpp"
namespace regenny::via {
struct Camera;
}
namespace regenny::via::render {
struct Mirror;
}
namespace regenny::via::render {
struct TargetState;
}
namespace regenny::via::render::layer {
#pragma pack(push, 1)
struct Scene : public regenny::via::render::RenderLayer {
    regenny::via::Camera* camera; // 0x88
    regenny::via::render::Mirror* mirror; // 0x90
    regenny::via::render::TargetState* buffer_pre_z; // 0x98
    regenny::via::render::TargetState* hdr_target; // 0xa0
    regenny::via::render::TargetState* hdr_target_with_velocity; // 0xa8
    regenny::via::render::TargetState* hdr_target_with_user_attribute_custom_params; // 0xb0
    regenny::via::render::TargetState* hdr_target_with_readonly_depth; // 0xb8
    char pad_c0[0x48];
    regenny::via::render::TargetState* depth_target; // 0x108
    char pad_110[0x8];
    regenny::via::render::TargetState* velocity_target; // 0x118
    regenny::via::render::TargetState* g_buffer_target; // 0x120
    regenny::via::render::TargetState* non_decal_g_buffer_target; // 0x128
    char pad_130[0x8];
    regenny::via::render::TargetState* post_main_target; // 0x138
    regenny::via::render::TargetState* post_depth_target; // 0x140
    char pad_148[0x1418];
}; // Size: 0x1560
#pragma pack(pop)
}
