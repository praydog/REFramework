#pragma once
#include "..\via\clr\ManagedObject.hpp"
namespace regenny::game {
#pragma pack(push, 1)
struct InputSystem : public regenny::via::clr::ManagedObject {
    char pad_20[0x150];
}; // Size: 0x170
#pragma pack(pop)
}
