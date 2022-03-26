#pragma once
#include ".\LayerList.hpp"
#include "..\clr\ManagedObject.hpp"
namespace regenny::via::render {
#pragma pack(push, 1)
struct RenderLayer : public regenny::via::clr::ManagedObject {
    char pad_20[0x20];
    regenny::via::render::LayerList layers; // 0x40
    char pad_50[0x28];
}; // Size: 0x78
#pragma pack(pop)
}
