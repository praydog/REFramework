#pragma once
namespace regenny::via::motion {
struct TransitionMap;
}
namespace regenny::via::motion {
struct TransitionData;
}
namespace regenny::via::motion {
struct Data;
}
namespace regenny::via::motion {
#pragma pack(push, 1)
struct MotionFsm2RawData {
    uint8_t hdr[16]; // 0x0
    uint8_t* file_data; // 0x10
    regenny::via::motion::TransitionMap* transition_map; // 0x18
    regenny::via::motion::TransitionData* transition_data; // 0x20
    regenny::via::motion::Data* data; // 0x28
    uint32_t transition_map_count; // 0x30
    uint32_t transition_data_count; // 0x34
    uint32_t transition_data_start; // 0x38
}; // Size: 0x3c
#pragma pack(pop)
}
