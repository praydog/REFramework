#pragma once

namespace sdk {
namespace memory {
void* allocate(size_t size, bool zero_memory = true);
void deallocate(void* ptr);
}
}