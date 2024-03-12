#pragma once
#include "..\IntArray.hpp"
#include "..\UIntArray.hpp"
#include "..\UIntArrayArray.hpp"
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct TreeNodeData {
    uint32_t id; // 0x0
    uint32_t id_2; // 0x4
    uint32_t attr; // 0x8
    bool is_branch; // 0xc
    bool is_end; // 0xd
    bool has_selector; // 0xe
    private: char pad_f[0x25]; public:
    uint32_t parent; // 0x34
    uint32_t parent_2; // 0x38
    private: char pad_3c[0x4]; public:
    regenny::via::UIntArray children; // 0x40
    regenny::via::UIntArray children2; // 0x50
    regenny::via::IntArray conditions; // 0x60
    regenny::via::UIntArray states; // 0x70
    regenny::via::UIntArray states_2; // 0x80
    regenny::via::IntArray transition_conditions; // 0x90
    regenny::via::UIntArray transition_ids; // 0xa0
    regenny::via::UIntArray transition_attributes; // 0xb0
    regenny::via::UIntArray actions; // 0xc0
    regenny::via::UIntArray actions_2; // 0xd0
    private: char pad_e0[0x50]; public:
    regenny::via::IntArray start_transitions; // 0x130
    regenny::via::UIntArray start_states; // 0x140
    regenny::via::UIntArray start_states_2; // 0x150
    regenny::via::UIntArrayArray transition_events; // 0x160
    regenny::via::UIntArrayArray start_transition_events; // 0x170
    regenny::via::UIntArrayArray unkarray3; // 0x180
    regenny::via::UIntArray tags; // 0x190
    // Metadata: utf16*
    wchar_t name[12]; // 0x1a0
    uint32_t name_len; // 0x1b8
    uint32_t name_capacity; // 0x1bc
    uint32_t name_offset; // 0x1c0
    uint32_t name_murmur_hash; // 0x1c4
    uint32_t full_name_murmur_hash; // 0x1c8
    uint32_t pad__; // 0x1cc
}; // Size: 0x1d0
#pragma pack(pop)
}
