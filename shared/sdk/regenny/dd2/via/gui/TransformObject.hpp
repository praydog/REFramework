#pragma once
#include "PlayObject.hpp"
namespace regenny::via::gui {
#pragma pack(push, 1)
struct TransformObject : public PlayObject {
    private: char pad_50[0xd0]; public:
}; // Size: 0x120
#pragma pack(pop)
}
