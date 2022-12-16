#pragma once
#include "IntArray.hpp"
#include "UIntArray.hpp"
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct TreeNodeData {
    uint32_t id; // 0x0
    uint32_t id_2; // 0x4
    uint32_t attr; // 0x8
    bool is_branch; // 0xc
    bool is_end; // 0xd
    bool has_selector; // 0xe
    char pad_f[0x1];
    uint32_t selector_id; // 0x10
    char pad_14[0x20];
    uint32_t parent; // 0x34
    uint32_t parent_2; // 0x38
    char pad_3c[0x4];
    regenny::via::behaviortree::UIntArray children; // 0x40
    regenny::via::behaviortree::UIntArray children2; // 0x50
    regenny::via::behaviortree::IntArray conditions; // 0x60
    regenny::via::behaviortree::UIntArray states; // 0x70
    regenny::via::behaviortree::UIntArray states_2; // 0x80
    regenny::via::behaviortree::IntArray transition_conditions; // 0x90
    regenny::via::behaviortree::UIntArray transition_ids; // 0xa0
    regenny::via::behaviortree::UIntArray transition_attributes; // 0xb0
    regenny::via::behaviortree::UIntArray actions; // 0xc0
    regenny::via::behaviortree::UIntArray actions_2; // 0xd0
    char pad_e0[0x50];
    regenny::via::behaviortree::IntArray start_transitions; // 0x130
    regenny::via::behaviortree::UIntArray start_states; // 0x140
    regenny::via::behaviortree::UIntArray start_states_2; // 0x150
    regenny::via::behaviortree::UIntArray transition_events; // 0x160
    regenny::via::behaviortree::UIntArray unkarray3; // 0x170
    regenny::via::behaviortree::UIntArray tags; // 0x180
    // Metadata: utf16*
    wchar_t name[12]; // 0x190
    uint32_t name_len; // 0x1a8
    uint32_t name_capacity; // 0x1ac
    uint32_t name_offset; // 0x1b0
    uint32_t name_murmur_hash; // 0x1b4
    uint32_t full_name_murmur_hash; // 0x1b8
    uint32_t pad_; // 0x1bc
}; // Size: 0x1c0
#pragma pack(pop)
}
