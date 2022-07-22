#pragma once

#include <cstdint>

namespace utility {
    bool isGoodPtr(uintptr_t ptr, size_t len, uint32_t access);
    bool isGoodReadPtr(uintptr_t ptr, size_t len);
    bool isGoodWritePtr(uintptr_t ptr, size_t len);
    bool isGoodCodePtr(uintptr_t ptr, size_t len);

    bool is_stub_code(uint8_t* code);

    void relocate_pointers(uint8_t* scan_start, uintptr_t old_start, uintptr_t old_end, uintptr_t new_start, int32_t depth = 0, uint32_t skip_length = sizeof(void*), uint32_t scan_size = 0x1000);
}
