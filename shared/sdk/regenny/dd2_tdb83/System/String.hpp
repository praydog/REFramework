#pragma once
#include "..\via\clr\ManagedObject.hpp"
namespace regenny::System {
#pragma pack(push, 1)
struct String : public regenny::via::clr::ManagedObject {
    int32_t len; // 0x10
    // Metadata: utf16*
    char data[256]; // 0x14
}; // Size: 0x114
#pragma pack(pop)
}
