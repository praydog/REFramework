#pragma once

class REClassInfo;

#pragma pack(push, 1)
class REObjectInfo {
public:
    REClassInfo* get_class_info() const { return m_classInfo; }

#ifdef REFRAMEWORK_UNIVERSAL
    // getType function pointer offset: 0x18 for TDB69+, 0x10 for TDB67
    void* get_type_fn() const;
#else
    void* get_type_fn() const { return m_getType; }
#endif

private:
    REClassInfo* m_classInfo;   // 0x0000 — stable across ALL versions
    void*        m_validator;   // 0x0008 — TDB69+ only (not in TDB67)
    void*        m_unk_0010;    // 0x0010
    void*        m_getType;     // 0x0018 — TDB69+. At 0x10 in TDB67.
    void*        m_toString;    // 0x0020
    void*        m_copy;        // 0x0028
    void*        m_unk_0030;    // 0x0030
    void*        m_unk_0038;    // 0x0038
    void*        m_unk_0040;    // 0x0040
    void*        m_unk_0048;    // 0x0048
    void*        m_unk_0050;    // 0x0050
    void*        m_getSize;     // 0x0058
    void*        m_unk_0060;    // 0x0060
    void*        m_unk_0068;    // 0x0068
    void*        m_unk_0070;    // 0x0070
    void*        m_unk_0078;    // 0x0078
    void*        m_unk_0080;    // 0x0080
    void*        m_unk_0088;    // 0x0088
    void*        m_unk_0090;    // 0x0090
    void*        m_unk_0098;    // 0x0098
    void*        m_unk_00A0;    // 0x00A0
    void*        m_unk_00A8;    // 0x00A8
}; // Size: 0x00B0
static_assert(sizeof(REObjectInfo) == 0xB0);
#pragma pack(pop)
