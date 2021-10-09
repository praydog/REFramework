#pragma once
#include "clr\ManagedObject.hpp"
namespace regenny::via {
struct GameObject;
}
namespace regenny::via {
struct Component;
}
namespace regenny::via {
#pragma pack(push, 1)
struct Component : public clr::ManagedObject {
    regenny::via::GameObject* GameObject; // 0x20
    regenny::via::Component* ChildComponent; // 0x28
    regenny::via::Component* PrevComponent; // 0x30
    regenny::via::Component* NextComponent; // 0x38
}; // Size: 0x40
#pragma pack(pop)
}
