#pragma once
#include "REManagedObject.hpp"

class REGameObject;

#pragma pack(push, 1)
class REComponent : public REManagedObject {
private:
    REGameObject*   m_owner;            // 0x10
    REComponent*    m_child;            // 0x18
    REComponent*    m_prev;             // 0x20
    REComponent*    m_next;             // 0x28

public:
    REGameObject* get_game_object() const { return m_owner; }
    REComponent* get_child_component() const { return m_child; }
    REComponent* get_prev_component() const { return m_prev; }
    REComponent* get_next_component() const { return m_next; }
    REComponent*& child_component_ref() { return m_child; }

    static constexpr uintptr_t offset_of_game_object()     { return 0x10; }
    static constexpr uintptr_t offset_of_child_component() { return 0x18; }
    static constexpr uintptr_t offset_of_prev_component()  { return 0x20; }
    static constexpr uintptr_t offset_of_next_component()  { return 0x28; }
};
static_assert(sizeof(REComponent) == 0x30);

class RECamera : public REComponent {
public:
    float       nearClipPlane;      // 0x30
    float       farClipPlane;       // 0x34
    float       fov;                // 0x38
    float       lookAtDistance;     // 0x3C
    bool        verticalEnable;     // 0x40
    char        _pad_0041[3];       // 0x41
    float       aspectRatio;        // 0x44
    int32_t     _unk_0048;          // 0x48
    char        _pad_004C[4];       // 0x4C
    int32_t     cameraType;         // 0x50
    char        _pad_0054[12];      // 0x54
    wchar_t*    cameraName;         // 0x60
    uint32_t    _unk_0068;          // 0x68
    char        _pad_006C[32];      // 0x6C
    float       _unk_008C;          // 0x8C
    float       _unk_0090;          // 0x90
    float       _unk_0094;          // 0x94
    float       _unk_0098;          // 0x98
    char        _pad_009C[252];     // 0x9C
};
static_assert(sizeof(RECamera) == 0x198);
#pragma pack(pop)
