#pragma once
#include "..\via\clr\ManagedObject.hpp"
namespace regenny::game {
#pragma pack(push, 1)
struct InputSystem : public regenny::via::clr::ManagedObject {
    char pad_10[0x160];
}; // Size: 0x170
#pragma pack(pop)
}
