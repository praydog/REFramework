#pragma once
#include "clr\ManagedObject.hpp"
namespace regenny::via {
struct GameObject;
}
namespace regenny::via {
#pragma pack(push, 1)
struct Component : public clr::ManagedObject {
    regenny::via::GameObject* GameObject; // 0x10
    regenny::via::Component* ChildComponent; // 0x18
    regenny::via::Component* PrevComponent; // 0x20
    regenny::via::Component* NextComponent; // 0x28
}; // Size: 0x30
#pragma pack(pop)
}
