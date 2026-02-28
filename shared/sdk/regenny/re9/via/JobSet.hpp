#pragma once
namespace regenny::via {
struct Object;
}
namespace regenny::via {
#pragma pack(push, 1)
struct JobSet {
    struct Job {
    }; // Size: 0x0

    struct JobThing {
        void* func; // 0x0
        private: char pad_8[0xf8]; public:
    }; // Size: 0x100

    struct Unk {
        void** vtable; // 0x0
        regenny::via::Object* owner; // 0x8
        void* func; // 0x10
        private: char pad_18[0x8]; public:
        regenny::via::Object* owner2; // 0x20
        private: char pad_28[0xd8]; public:
    }; // Size: 0x100

    Unk* data; // 0x0
}; // Size: 0x8
#pragma pack(pop)
}
