#pragma once
#include "REObject.hpp"

#pragma pack(push, 1)
class REManagedObject : public REObject {
public:
    uint32_t get_ref_count() const { return m_ref_count; }
    void set_ref_count(uint32_t v) { m_ref_count = v; }
    uint32_t* ref_count_ptr() { return &m_ref_count; }

private:
    uint32_t    m_ref_count;    // 0x08
    int16_t     _unk_000C;      // 0x0C
    char        _pad_000E[2];   // 0x0E
};
static_assert(sizeof(REManagedObject) == 0x10);
#pragma pack(pop)
