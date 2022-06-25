#pragma once
namespace regenny::via::clr {
struct ManagedObject;
}
namespace regenny::via::behaviortree {
#pragma pack(push, 1)
struct ManagedObjectArray {
    regenny::via::clr::ManagedObject** objects; // 0x0
    uint32_t count; // 0x8
    uint32_t capacity; // 0xc
}; // Size: 0x10
#pragma pack(pop)
}
