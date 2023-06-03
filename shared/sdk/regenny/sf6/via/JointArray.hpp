#pragma once
#include "clr\ManagedObject.hpp"
namespace regenny::via::typeinfo {
struct TypeInfo;
}
namespace regenny::via {
struct Joint;
}
namespace regenny::via {
#pragma pack(push, 1)
struct JointArray : public clr::ManagedObject {
    regenny::via::typeinfo::TypeInfo* array_type; // 0x10
    int32_t count; // 0x18
    int32_t num_allocated; // 0x1c
    regenny::via::Joint* joints[256]; // 0x20
}; // Size: 0x820
#pragma pack(pop)
}
