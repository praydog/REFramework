#include "types/REObjectInfo.hpp"
#include "GameIdentity.hpp"

void* REObjectInfo::get_type_fn() const {
    // TDB67 (DMC5): no validator field, getType at offset 0x10
    // TDB69+: validator inserted at 0x08, getType at 0x18
    if (sdk::GameIdentity::get().tdb_ver() <= 67) {
        return *reinterpret_cast<void* const*>(reinterpret_cast<uintptr_t>(this) + 0x10);
    }
    return m_getType;
}
