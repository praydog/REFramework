#pragma once

#include <cstdint>

class REObjectInfo;
class RETypeCLR;

#pragma pack(push, 1)
class REClassInfo {
public:
    // Stable fields (same offset in ALL supported versions TDB67+)
    uint16_t get_type_index() const { return m_typeIndex; }

#ifdef REFRAMEWORK_UNIVERSAL
    // These fields move between TDB versions — need dispatch
    RETypeCLR* get_type() const;
    REObjectInfo* get_parent_info() const;
    uint32_t get_size() const;
    uint32_t get_fqn_hash() const;
    uint32_t get_type_crc() const;
    uint8_t get_object_flags() const;
    uint8_t get_object_type() const;
#else
    RETypeCLR* get_type() const { return m_type; }
    REObjectInfo* get_parent_info() const { return m_parentInfo; }
    uint32_t get_size() const { return m_size; }
    uint32_t get_fqn_hash() const { return m_fqnHash; }
    uint32_t get_type_crc() const { return m_typeCRC; }
    uint8_t get_object_flags() const { return m_objectFlags; }
    uint8_t get_object_type() const { return m_objectFlags >> 5; }
#endif

private:
    uint16_t m_typeIndex;        // 0x0000
    char     _pad_0002[5];       // 0x0002
    uint8_t  m_objectFlags;      // 0x0007
    uint32_t m_unk_0008;         // 0x0008
    uint32_t m_elementBitField;  // 0x000C
    uint32_t m_typeFlags;        // 0x0010
    uint32_t m_size;             // 0x0014
    uint32_t m_fqnHash;          // 0x0018
    uint32_t m_typeCRC;          // 0x001C
    uint32_t m_defaultCtor;      // 0x0020
    uint32_t m_vt;               // 0x0024
    uint32_t m_memberMethod;     // 0x0028
    uint32_t m_memberField;      // 0x002C
    uint32_t m_memberProp;       // 0x0030
    uint32_t m_memberEvent;      // 0x0034
    int32_t  m_interfaces;       // 0x0038
    int32_t  m_generics;         // 0x003C
    RETypeCLR*    m_type;        // 0x0040
    REObjectInfo* m_parentInfo;  // 0x0048
}; // Size: 0x0050
static_assert(sizeof(REClassInfo) == 0x50);
#pragma pack(pop)
