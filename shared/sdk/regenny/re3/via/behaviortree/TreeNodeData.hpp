#pragma once
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct TreeNodeData {
    struct UIntArray {
        uint32_t* data; // 0x0
        uint64_t count; // 0x8
    }; // Size: 0x10

    struct IntArray {
        int32_t* data; // 0x0
        uint64_t count; // 0x8
    }; // Size: 0x10

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
    UIntArray children; // 0x40
    UIntArray children2; // 0x50
    IntArray conditions; // 0x60
    UIntArray states; // 0x70
    UIntArray states_2; // 0x80
    IntArray transition_conditions; // 0x90
    UIntArray transition_ids; // 0xa0
    UIntArray transition_attributes; // 0xb0
    UIntArray actions; // 0xc0
    UIntArray actions_2; // 0xd0
    char pad_e0[0x50];
    IntArray start_transitions; // 0x130
    UIntArray start_states; // 0x140
    UIntArray start_states_2; // 0x150
    UIntArray unkarray2; // 0x160
    UIntArray unkarray3; // 0x170
    UIntArray tags; // 0x180
    // Metadata: utf16*
    wchar_t name[12]; // 0x190
    uint32_t name_len; // 0x1a8
    uint32_t name_capacity; // 0x1ac
    uint32_t name_offset; // 0x1b0
    uint32_t name_murmur_hash; // 0x1b4
    uint32_t full_name_murmur_hash; // 0x1b8
}; // Size: 0x1bc
#pragma pack(pop)
}
