#pragma once

#include <cstdint>

namespace utility {
    void relocate_pointers(uint8_t* scan_start, uintptr_t old_start, uintptr_t old_end, uintptr_t new_start, int32_t depth = 0, uint32_t skip_length = sizeof(void*), uint32_t scan_size = 0x1000);
}
