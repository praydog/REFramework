#include "types/REClassInfo.hpp"
#include "GameIdentity.hpp"

RETypeCLR* REClassInfo::get_type() const {
    auto ver = sdk::GameIdentity::get().tdb_ver();
    if (ver <= 67) {
        // TDB67 (DMC5): type at offset 0x68
        return *reinterpret_cast<RETypeCLR* const*>(reinterpret_cast<uintptr_t>(this) + 0x68);
    }
    if (ver >= 81) {
        // TDB81+ (MHWILDS/RE9): type at offset 0x38
        return *reinterpret_cast<RETypeCLR* const*>(reinterpret_cast<uintptr_t>(this) + 0x38);
    }
    return m_type; // TDB69-80: at offset 0x40
}

REObjectInfo* REClassInfo::get_parent_info() const {
    auto ver = sdk::GameIdentity::get().tdb_ver();
    if (ver <= 67) {
        // TDB67 (DMC5): parentInfo at offset 0x70
        return *reinterpret_cast<REObjectInfo* const*>(reinterpret_cast<uintptr_t>(this) + 0x70);
    }
    if (ver >= 81) {
        // TDB81+ (MHWILDS/RE9): parentInfo at offset 0x40
        return *reinterpret_cast<REObjectInfo* const*>(reinterpret_cast<uintptr_t>(this) + 0x40);
    }
    return m_parentInfo; // TDB69-80: at offset 0x48
}

uint32_t REClassInfo::get_size() const {
    auto ver = sdk::GameIdentity::get().tdb_ver();
    if (ver <= 67) {
        return *reinterpret_cast<const uint32_t*>(reinterpret_cast<uintptr_t>(this) + 0x30);
    }
    if (ver >= 81) {
        return *reinterpret_cast<const uint32_t*>(reinterpret_cast<uintptr_t>(this) + 0x10);
    }
    return m_size; // TDB69-80: at offset 0x14
}

uint32_t REClassInfo::get_fqn_hash() const {
    auto ver = sdk::GameIdentity::get().tdb_ver();
    if (ver <= 67) {
        return *reinterpret_cast<const uint32_t*>(reinterpret_cast<uintptr_t>(this) + 0x08);
    }
    if (ver >= 81) {
        return *reinterpret_cast<const uint32_t*>(reinterpret_cast<uintptr_t>(this) + 0x14);
    }
    return m_fqnHash; // TDB69-80: at offset 0x18
}

uint32_t REClassInfo::get_type_crc() const {
    auto ver = sdk::GameIdentity::get().tdb_ver();
    if (ver <= 67) {
        return *reinterpret_cast<const uint32_t*>(reinterpret_cast<uintptr_t>(this) + 0x0C);
    }
    if (ver >= 81) {
        return *reinterpret_cast<const uint32_t*>(reinterpret_cast<uintptr_t>(this) + 0x18);
    }
    return m_typeCRC; // TDB69-80: at offset 0x1C
}

uint8_t REClassInfo::get_object_flags() const {
    auto ver = sdk::GameIdentity::get().tdb_ver();
    if (ver <= 67) {
        // TDB67: no objectFlags byte at 0x07. Return 0.
        return 0;
    }
    return m_objectFlags; // TDB69+: at offset 0x07
}

uint8_t REClassInfo::get_object_type() const {
    auto ver = sdk::GameIdentity::get().tdb_ver();
    if (ver <= 67) {
        // TDB67: objectType is a direct uint8 at offset 0x26
        return *reinterpret_cast<const uint8_t*>(reinterpret_cast<uintptr_t>(this) + 0x26);
    }
    // TDB69+: derived from objectFlags >> 5
    return m_objectFlags >> 5;
}
