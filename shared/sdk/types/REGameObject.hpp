#pragma once
#include <string>
#include "REManagedObject.hpp"

class RETransform;
class REFolder;
class SystemString;

#pragma pack(push, 1)
class REGameObject : public REManagedObject {
public:
#ifdef REFRAMEWORK_UNIVERSAL
    RETransform* get_transform() const;
    bool get_shouldDraw() const;
    void set_shouldDraw(bool v);
    bool get_shouldUpdate() const;
    void set_shouldUpdate(bool v);
    REFolder* get_folder() const;
    SystemString* get_name_field() const;

    static uintptr_t offset_of_transform();
    static uintptr_t offset_of_folder();

    std::string get_name() const;
#else
    RETransform* get_transform() const { return m_transform; }
    bool get_shouldDraw() const { return m_shouldDraw; }
    void set_shouldDraw(bool v) { m_shouldDraw = v; }
    bool get_shouldUpdate() const { return m_shouldUpdate; }
    void set_shouldUpdate(bool v) { m_shouldUpdate = v; }
    REFolder* get_folder() const { return m_folder; }
    SystemString* get_name_field() const { return m_name; }

    static uintptr_t offset_of_transform() { return offsetof(REGameObject, m_transform); }
    static uintptr_t offset_of_folder()    { return offsetof(REGameObject, m_folder); }

    std::string get_name() const;
#endif

private:
    char            _pad_0010[2];       // 0x10
    bool            m_shouldUpdate;     // 0x12
    bool            m_shouldDraw;       // 0x13
    bool            m_shouldUpdateSelf; // 0x14
    bool            m_shouldDrawSelf;   // 0x15
    bool            m_shouldSelect;     // 0x16
    char            _pad_0017[1];       // 0x17
    RETransform*    m_transform;        // 0x18 — 0x20 on SF6/RE4
    REFolder*       m_folder;           // 0x20 — 0x28 on SF6/RE4
    SystemString*   m_name;             // 0x28 — type varies: embedded REString (0x20 bytes) on legacy games
    uint32_t        _unk_0030;          // 0x30
    float           m_timescale;        // 0x34
    char            _pad_0038[16];      // 0x38
};
static_assert(sizeof(REGameObject) == 0x48);
#pragma pack(pop)
