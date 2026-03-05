#pragma once
namespace regenny::via::motion {
#pragma pack(push, 1)
struct TransitionData {
    uint32_t id; // 0x0
    uint32_t end_type : 4; // 0x4
    uint32_t interpolation_mode : 4; // 0x4
    uint32_t prev_move_to_end : 1; // 0x4
    uint32_t start_type : 4; // 0x4
    uint32_t elapsed_time_zero : 1; // 0x4
    uint32_t cont_on_layer : 1; // 0x4
    uint32_t cont_on_layer_interp_curve : 4; // 0x4
    private: uint32_t pad_bitfield_4_13 : 13; public:
    float exit_frame; // 0x8
    float start_frame; // 0xc
    float interpolation_frame; // 0x10
    uint32_t pad; // 0x14
    uint32_t pad2; // 0x18
    uint32_t pad3; // 0x1c
    uint32_t pad4; // 0x20
}; // Size: 0x24
#pragma pack(pop)
}
